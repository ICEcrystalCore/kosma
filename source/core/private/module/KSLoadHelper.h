//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSLOADHELPER_H
#define KOSMA_KSLOADHELPER_H

#include "KSModuleGraph.h"
#include "KSModuleStore.h"
#include "module/KSModule.h"

#include <functional>
#include <map>
#include <set>

namespace Kosma::Core {

// Orchestrates module loading (topological order) and unloading.
class LoadHelper {
public:
    using Name = Module::Name;
    using FactoryMap = std::map<Name, Module::Factory::Ptr>;

    // 'inject' is called after each module is created; it should set the module's
    // name and manager pointer. Provided by Impl (a nested class of ModuleManager)
    // which has access to the private injectModule() method.
    using InjectFn = std::function<void(Module&, const Name&)>;

    LoadHelper(ModuleGraph& graph, ModuleStore& store, FactoryMap& factories,
        InjectFn inject);

    // Loads 'name' and all missing transitive dependencies.
    // Returns false if 'name' or any dependency lacks a registered factory.
    bool load(const Name& name);

    // Shuts down 'name' and removes it from the store.
    void unload(const Name& name);

    // Loads every registered factory (each via load()).
    void loadAll();

    // Shuts down all loaded modules in reverse dependency order.
    void unloadAll();

private:
    // Recursive helper; 'visiting' guards against graph cycles at load time.
    bool loadRec(const Name& name, std::set<Name>& visiting);

    void unloadOne(const Name& name);

    // Builds a reverse-dependency shutdown order via DFS.
    void collectShutdownOrder(const Name& name, std::set<Name>& visited,
        std::vector<Name>& order);

    ModuleGraph& m_graph;
    ModuleStore& m_store;
    FactoryMap& m_factories;
    InjectFn m_inject;
};

}  // namespace Kosma::Core

#endif  // KOSMA_KSLOADHELPER_H
