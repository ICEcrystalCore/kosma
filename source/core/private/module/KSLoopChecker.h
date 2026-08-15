//
// Created by Jun on 2025/8/30.
//

#ifndef KOSMA_KSLOOPCHECKER_H
#define KOSMA_KSLOOPCHECKER_H

#include "KSModuleGraph.h"

#include <map>

namespace Kosma::Core {

// Detects whether adding a new node with given dependencies to the graph
// would introduce a directed cycle.
class LoopChecker {
public:
    // Returns true if registering 'name' → deps would create a cycle.
    static bool wouldCreateCycle(const ModuleGraph& graph, const Module::Name& name,
        const Vector<Module::Name>& deps);

private:
    enum class Color { White, Gray, Black };
    using ColorMap = std::map<Module::Name, Color>;

    // DFS visit; returns true if a back-edge (cycle) is found.
    static bool dfs(const ModuleGraph& graph, const Module::Name& node, ColorMap& colors);
};

}  // namespace Kosma::Core

#endif  // KOSMA_KSLOOPCHECKER_H
