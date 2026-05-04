#pragma once

#include "graph.hpp"
#include "types.hpp"

namespace graph {
PathResult dfs_path(const Graph& graph, int start, int goal, int cols);
PathResult bfs_path(const Graph& graph, int start, int goal, int cols);
PathResult dijkstra_path(const Graph& graph, int start, int goal, int cols);
PathResult astar_path(const Graph& graph, int start, int goal, int cols);
int connected_components(const Graph& graph);
} // namespace graph