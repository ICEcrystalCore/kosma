//
// Created by Jun on 2025/8/30.
//

#include "module/KSModule.h"
#include "module/KSModuleManager.h"
#include "gtest/gtest.h"

using namespace Kosma::Core;
using Name = Module::Name;

// ---------------------------------------------------------------------------
// Concrete test modules
// ---------------------------------------------------------------------------

struct StartupLog {
    std::vector<std::string> startups;
    std::vector<std::string> shutdowns;
};

class AlphaModule : public ModuleOf<AlphaModule> {
public:
    explicit AlphaModule(StartupLog* log) : m_log(log) {}

    void startup() override { m_log->startups.push_back("Alpha"); }
    void shutdown() override { m_log->shutdowns.push_back("Alpha"); }

private:
    StartupLog* m_log;
};

class BetaModule : public ModuleOf<BetaModule> {
public:
    explicit BetaModule(StartupLog* log) : m_log(log) {}

    void startup() override { m_log->startups.push_back("Beta"); }
    void shutdown() override { m_log->shutdowns.push_back("Beta"); }

private:
    StartupLog* m_log;
};

// GammaModule depends on Alpha and Beta
class GammaModule : public ModuleOf<GammaModule> {
public:
    explicit GammaModule(StartupLog* log) : m_log(log) {}

    void startup() override { m_log->startups.push_back("Gamma"); }
    void shutdown() override { m_log->shutdowns.push_back("Gamma"); }

private:
    StartupLog* m_log;
};

// ---------------------------------------------------------------------------
// Concrete factory helpers
// ---------------------------------------------------------------------------

class AlphaFactory : public Module::Factory {
public:
    explicit AlphaFactory(StartupLog* log) : m_log(log) {}

    Name name() const override { return Name("Alpha"); }

    SharedPtr<Module> create() const override
    {
        return makeShared<AlphaModule>(m_log);
    }

private:
    StartupLog* m_log;
};

class BetaFactory : public Module::Factory {
public:
    explicit BetaFactory(StartupLog* log) : m_log(log) {}

    Name name() const override { return Name("Beta"); }

    SharedPtr<Module> create() const override
    {
        return makeShared<BetaModule>(m_log);
    }

private:
    StartupLog* m_log;
};

class GammaFactory : public Module::Factory {
public:
    explicit GammaFactory(StartupLog* log) : m_log(log) {}

    Name name() const override { return Name("Gamma"); }

    Vector<Name> requiredModules() const override
    {
        return {Name("Alpha"), Name("Beta")};
    }

    SharedPtr<Module> create() const override
    {
        return makeShared<GammaModule>(m_log);
    }

private:
    StartupLog* m_log;
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(ModuleManagerTest, BasicLoadUnload)
{
    StartupLog log;
    ModuleManager mgr;

    mgr.registerFactory(makeUnique<AlphaFactory>(&log));

    EXPECT_TRUE(mgr.hasFactory(Name("Alpha")));
    EXPECT_FALSE(mgr.isLoaded(Name("Alpha")));

    bool loaded = mgr.load(Name("Alpha"));
    EXPECT_TRUE(loaded);
    EXPECT_TRUE(mgr.isLoaded(Name("Alpha")));
    ASSERT_EQ(1u, log.startups.size());
    EXPECT_EQ("Alpha", log.startups[0]);

    mgr.unload(Name("Alpha"));
    EXPECT_FALSE(mgr.isLoaded(Name("Alpha")));
    ASSERT_EQ(1u, log.shutdowns.size());
    EXPECT_EQ("Alpha", log.shutdowns[0]);
}

TEST(ModuleManagerTest, DependencyLoadOrder)
{
    StartupLog log;
    ModuleManager mgr;

    mgr.registerFactory(makeUnique<AlphaFactory>(&log));
    mgr.registerFactory(makeUnique<BetaFactory>(&log));
    mgr.registerFactory(makeUnique<GammaFactory>(&log));

    // Loading Gamma should auto-load Alpha and Beta first.
    EXPECT_TRUE(mgr.load(Name("Gamma")));

    EXPECT_TRUE(mgr.isLoaded(Name("Alpha")));
    EXPECT_TRUE(mgr.isLoaded(Name("Beta")));
    EXPECT_TRUE(mgr.isLoaded(Name("Gamma")));

    // Alpha and Beta must have started before Gamma.
    ASSERT_EQ(3u, log.startups.size());
    EXPECT_EQ("Gamma", log.startups[2]);
}

TEST(ModuleManagerTest, CircularDependencyRejected)
{
    // A → B → A  (cycle)
    class AFactory : public Module::Factory {
    public:
        Name name() const override { return Name("A"); }
        Vector<Name> requiredModules() const override { return {Name("B")}; }
        SharedPtr<Module> create() const override { return {}; }
    };

    class BFactory : public Module::Factory {
    public:
        Name name() const override { return Name("B"); }
        Vector<Name> requiredModules() const override { return {Name("A")}; }
        SharedPtr<Module> create() const override { return {}; }
    };

    ModuleManager mgr;
    EXPECT_TRUE(mgr.registerFactory(makeUnique<AFactory>()));
    // B creates a cycle A→B→A, so registration must be rejected.
    EXPECT_FALSE(mgr.registerFactory(makeUnique<BFactory>()));
}

TEST(ModuleManagerTest, LoadAll)
{
    StartupLog log;
    ModuleManager mgr;

    mgr.registerFactory(makeUnique<AlphaFactory>(&log));
    mgr.registerFactory(makeUnique<BetaFactory>(&log));

    mgr.loadAll();

    EXPECT_TRUE(mgr.isLoaded(Name("Alpha")));
    EXPECT_TRUE(mgr.isLoaded(Name("Beta")));
    EXPECT_EQ(2u, log.startups.size());
}

TEST(ModuleManagerTest, UnloadAll)
{
    StartupLog log;
    ModuleManager mgr;

    mgr.registerFactory(makeUnique<AlphaFactory>(&log));
    mgr.registerFactory(makeUnique<BetaFactory>(&log));
    mgr.registerFactory(makeUnique<GammaFactory>(&log));

    mgr.loadAll();
    ASSERT_EQ(3u, log.startups.size());

    mgr.unloadAll();
    EXPECT_FALSE(mgr.isLoaded(Name("Alpha")));
    EXPECT_FALSE(mgr.isLoaded(Name("Beta")));
    EXPECT_FALSE(mgr.isLoaded(Name("Gamma")));
    EXPECT_EQ(3u, log.shutdowns.size());
    // Gamma (depends on Alpha/Beta) must shut down before Alpha and Beta.
    EXPECT_EQ("Gamma", log.shutdowns[0]);
}

TEST(ModuleManagerTest, FindAsTypeCast)
{
    StartupLog log;
    ModuleManager mgr;

    mgr.registerFactory(makeUnique<AlphaFactory>(&log));
    mgr.load(Name("Alpha"));

    auto alpha = mgr.findAs<AlphaModule>(Name("Alpha"));
    EXPECT_NE(nullptr, alpha.get());
}

TEST(ModuleManagerTest, ModuleKnowsItsName)
{
    StartupLog log;
    ModuleManager mgr;

    mgr.registerFactory(makeUnique<AlphaFactory>(&log));
    mgr.load(Name("Alpha"));

    auto mod = mgr.find(Name("Alpha"));
    ASSERT_NE(nullptr, mod.get());
    EXPECT_EQ(Name("Alpha"), mod->name());
}

TEST(ModuleManagerTest, ModuleKnowsItsManager)
{
    StartupLog log;
    ModuleManager mgr;

    mgr.registerFactory(makeUnique<AlphaFactory>(&log));
    mgr.load(Name("Alpha"));

    auto mod = mgr.find(Name("Alpha"));
    ASSERT_NE(nullptr, mod.get());
    EXPECT_EQ(&mgr, mod->manager());
}
