//
// Created by Jun on 2025/8/30.
//

#include "KSLoadHelper.h"

namespace Kosma::Core {

LoadHelper::LoadHelper(ModuleGraph& graph, ModuleStore& store, FactoryMap& factories,
    InjectFn inject)
    : m_graph(graph), m_store(store), m_factories(factories), m_inject(std::move(inject))
{
}

bool LoadHelper::load(const Name& name)
{
    if (m_store.has(name))
        return true;
    std::set<Name> visiting;
    return loadRec(name, visiting);
}

bool LoadHelper::loadRec(const Name& name, std::set<Name>& visiting)
{
    if (m_store.has(name))
        return true;

    auto fit = m_factories.find(name);
    if (fit == m_factories.end())
        return false;

    visiting.insert(name);

    // Load dependencies first.
    for (const auto& dep : m_graph.depsOf(name)) {
        if (visiting.count(dep))
            continue;  // cycle guard (should not happen after registration checks)
        if (!loadRec(dep, visiting))
            return false;
        m_store.trackDependent(dep, name);
    }

    visiting.erase(name);

    // Create and start the module.
    SharedPtr<Module> mod = fit->second->create();
    if (!mod)
        return false;

    m_inject(*mod, name);
    mod->startup();
    m_store.add(name, std::move(mod));
    return true;
}

void LoadHelper::unload(const Name& name)
{
    if (!m_store.has(name))
        return;
    unloadOne(name);
}

void LoadHelper::loadAll()
{
    for (const auto& kv : m_factories)
        load(kv.first);
}

void LoadHelper::unloadAll()
{
    auto allLoaded = m_store.allLoaded();
    std::set<Name> visited;
    std::vector<Name> order;

    for (const auto& name : allLoaded)
        collectShutdownOrder(name, visited, order);

    for (const auto& name : order) {
        if (m_store.has(name))
            unloadOne(name);
    }
}

void LoadHelper::unloadOne(const Name& name)
{
    auto mod = m_store.get(name);
    if (!mod)
        return;

    mod->shutdown();
    m_store.remove(name);

    // Untrack this module as a dependent of its dependencies.
    for (const auto& dep : m_graph.depsOf(name))
        m_store.untrackDependent(dep, name);
}

void LoadHelper::collectShutdownOrder(const Name& name, std::set<Name>& visited,
    std::vector<Name>& order)
{
    if (visited.count(name))
        return;
    visited.insert(name);

    // Process modules that depend on 'name' first (they must shut down before 'name').
    for (const auto& dependent : m_store.dependentsOf(name)) {
        if (m_store.has(dependent))
            collectShutdownOrder(dependent, visited, order);
    }

    order.push_back(name);
}

}  // namespace Kosma::Core
