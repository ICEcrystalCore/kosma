//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSMODULEGRAPH_H
#define KOSMA_KSMODULEGRAPH_H

#include "Module/KSModule.h"

#include <map>
#include <vector>

namespace Kosma::Core {

// Directed dependency graph: records "A depends on B" edges.
// addNode(A, {B, C}) means A→B, A→C.
class ModuleGraph {
public:
    using Name = Module::Name;

    void addNode(const Name& name, const Vector<Name>& deps);
    void removeNode(const Name& name);
    bool hasNode(const Name& name) const;

    // Returns the direct dependencies of 'name' (empty if not found).
    const std::vector<Name>& depsOf(const Name& name) const;

    std::vector<Name> allNodes() const;

private:
    std::map<Name, std::vector<Name>> m_out;  // name → list of its dependencies

    static const std::vector<Name> s_empty;
};

}  // namespace Kosma::Core

#endif  // KOSMA_KSMODULEGRAPH_H
