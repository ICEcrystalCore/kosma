//
// Created by Jun on 2025/8/30.
//

#include "KSModuleStore.h"

namespace Kosma::Core {

const std::set<ModuleStore::Name> ModuleStore::s_emptySet;

bool ModuleStore::has(const Name& name) const
{
    return m_modules.find(name) != m_modules.end();
}

SharedPtr<Module> ModuleStore::get(const Name& name) const
{
    auto it = m_modules.find(name);
    return it != m_modules.end() ? it->second : SharedPtr<Module>();
}

void ModuleStore::add(const Name& name, SharedPtr<Module> mod)
{
    m_modules.emplace(name, std::move(mod));
}

void ModuleStore::remove(const Name& name)
{
    m_modules.erase(name);
    m_dependents.erase(name);
}

std::vector<ModuleStore::Name> ModuleStore::allLoaded() const
{
    std::vector<Name> result;
    result.reserve(m_modules.size());
    for (const auto& kv : m_modules)
        result.push_back(kv.first);
    return result;
}

void ModuleStore::trackDependent(const Name& dep, const Name& dependent)
{
    m_dependents[dep].insert(dependent);
}

void ModuleStore::untrackDependent(const Name& dep, const Name& dependent)
{
    auto it = m_dependents.find(dep);
    if (it != m_dependents.end())
        it->second.erase(dependent);
}

const std::set<ModuleStore::Name>& ModuleStore::dependentsOf(const Name& dep) const
{
    auto it = m_dependents.find(dep);
    return it != m_dependents.end() ? it->second : s_emptySet;
}

}  // namespace Kosma::Core
