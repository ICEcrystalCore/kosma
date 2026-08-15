//
// Created by Jun on 2025/8/30.
//

#include "KSLoopChecker.h"

namespace Kosma::Core {

bool LoopChecker::wouldCreateCycle(const ModuleGraph& graph, const Module::Name& name,
    const Vector<Module::Name>& deps)
{
    // Build a temporary graph that includes the candidate node.
    ModuleGraph tmp = graph;
    tmp.addNode(name, deps);

    // Run a full DFS cycle check on the augmented graph.
    ColorMap colors;
    for (const auto& node : tmp.allNodes()) {
        if (colors[node] == Color::White) {
            if (dfs(tmp, node, colors))
                return true;
        }
    }
    return false;
}

bool LoopChecker::dfs(const ModuleGraph& graph, const Module::Name& node, ColorMap& colors)
{
    colors[node] = Color::Gray;

    for (const auto& dep : graph.depsOf(node)) {
        Color& c = colors[dep];
        if (c == Color::Gray)
            return true;  // back-edge → cycle
        if (c == Color::White)
            if (dfs(graph, dep, colors))
                return true;
    }

    colors[node] = Color::Black;
    return false;
}

}  // namespace Kosma::Core
