//
// Created by Jun on 2025/8/30.
//

#include "KSModuleManagerImpl.h"

namespace Kosma::Core {

bool ModuleManager::Impl::registerFactory(Module::Factory::Ptr factory)
{
    if (!factory)
        return false;

    const auto& name = factory->name();
    const auto& deps = factory->requiredModules();

    if (LoopChecker::wouldCreateCycle(graph, name, deps))
        return false;

    graph.addNode(name, deps);
    factories.emplace(name, std::move(factory));
    return true;
}

void ModuleManager::Impl::unregisterFactory(const Module::Name& name)
{
    factories.erase(name);
    graph.removeNode(name);
}

// Inside Impl methods (Impl is a nested class of ModuleManager), the lambda
// defined here has access to private ModuleManager::injectModule() per
// C++11 nested class rules (access same as any member of enclosing class).

bool ModuleManager::Impl::load(const Module::Name& name, ModuleManager* mgr)
{
    LoadHelper helper(graph, store, factories,
        [mgr](Module& mod, const Module::Name& n) {
            ModuleManager::injectModule(mod, n, mgr);
        });
    return helper.load(name);
}

void ModuleManager::Impl::unload(const Module::Name& name, ModuleManager* mgr)
{
    LoadHelper helper(graph, store, factories,
        [mgr](Module& mod, const Module::Name& n) {
            ModuleManager::injectModule(mod, n, mgr);
        });
    helper.unload(name);
}

void ModuleManager::Impl::loadAll(ModuleManager* mgr)
{
    LoadHelper helper(graph, store, factories,
        [mgr](Module& mod, const Module::Name& n) {
            ModuleManager::injectModule(mod, n, mgr);
        });
    helper.loadAll();
}

void ModuleManager::Impl::unloadAll(ModuleManager* mgr)
{
    LoadHelper helper(graph, store, factories,
        [mgr](Module& mod, const Module::Name& n) {
            ModuleManager::injectModule(mod, n, mgr);
        });
    helper.unloadAll();
}

bool ModuleManager::Impl::isLoaded(const Module::Name& name) const
{
    return store.has(name);
}

bool ModuleManager::Impl::hasFactory(const Module::Name& name) const
{
    return factories.find(name) != factories.end();
}

SharedPtr<Module> ModuleManager::Impl::find(const Module::Name& name) const
{
    return store.get(name);
}

}  // namespace Kosma::Core
