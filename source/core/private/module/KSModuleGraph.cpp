//
// Created by Jun on 2025/8/30.
//

#include "KSModuleGraph.h"

namespace Kosma::Core {

const std::vector<ModuleGraph::Name> ModuleGraph::s_empty;

void ModuleGraph::addNode(const Name& name, const Vector<Name>& deps)
{
    auto& vec = m_out[name];
    vec.clear();
    for (size_t i = 0; i < deps.size(); ++i)
        vec.push_back(deps[i]);
}

void ModuleGraph::removeNode(const Name& name)
{
    m_out.erase(name);
}

bool ModuleGraph::hasNode(const Name& name) const
{
    return m_out.find(name) != m_out.end();
}

const std::vector<ModuleGraph::Name>& ModuleGraph::depsOf(const Name& name) const
{
    auto it = m_out.find(name);
    return it != m_out.end() ? it->second : s_empty;
}

std::vector<ModuleGraph::Name> ModuleGraph::allNodes() const
{
    std::vector<Name> result;
    result.reserve(m_out.size());
    for (const auto& kv : m_out)
        result.push_back(kv.first);
    return result;
}

}  // namespace Kosma::Core
