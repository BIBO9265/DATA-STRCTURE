#include "graph/algorithms.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <stack>
#include <string>
#include <vector>

namespace graph {

namespace {

std::vector<int> rebuild_path(const std::vector<int>& parent, int start, int goal) {
    std::vector<int> path;

    if (goal < 0 || goal >= static_cast<int>(parent.size())) {
        return path;
    }

    if (start != goal && parent[goal] == -1) {
        return path;
    }

    for (int current = goal; current != -1; current = parent[current]) {
        path.push_back(current);

        if (current == start) {
            break;
        }
    }

    std::reverse(path.begin(), path.end());

    if (path.empty() || path.front() != start) {
        return {};
    }

    return path;
}

int edge_weight_between(const Graph& graph, int from, int to) {
    for (const auto& [next, weight] : graph.neighbors(from)) {
        if (next == to) {
            return weight;
        }
    }

    return 0;
}

int calculate_path_cost(const Graph& graph, const std::vector<int>& path) {
    if (path.size() < 2) {
        return 0;
    }

    int cost = 0;

    for (std::size_t i = 1; i < path.size(); ++i) {
        cost += edge_weight_between(graph, path[i - 1], path[i]);
    }

    return cost;
}

int count_turns(const std::vector<int>& path, int cols) {
    if (path.size() < 3) {
        return 0;
    }

    int turns = 0;

    for (std::size_t i = 2; i < path.size(); ++i) {
        int a = path[i - 2];
        int b = path[i - 1];
        int c = path[i];

        int ar = a / cols;
        int ac = a % cols;
        int br = b / cols;
        int bc = b % cols;
        int cr = c / cols;
        int cc = c % cols;

        int dr1 = br - ar;
        int dc1 = bc - ac;
        int dr2 = cr - br;
        int dc2 = cc - bc;

        if (dr1 != dr2 || dc1 != dc2) {
            ++turns;
        }
    }

    return turns;
}

void finalize_result(PathResult& result, const Graph& graph, int cols) {
    result.found = !result.path.empty();
    result.path_length = result.found ? static_cast<int>(result.path.size()) - 1 : 0;
    result.path_cost = result.found ? calculate_path_cost(graph, result.path) : 0;
    result.turn_count = result.found ? count_turns(result.path, cols) : 0;
}

template <typename Func>
PathResult measure(const std::string& name, Func&& func) {
    auto begin = std::chrono::high_resolution_clock::now();
    PathResult result = func();
    auto end = std::chrono::high_resolution_clock::now();

    result.algorithm = name;
    result.elapsed_ms = std::chrono::duration<double, std::milli>(end - begin).count();

    return result;
}

} // namespace

PathResult dfs_path(const Graph& graph, int start, int goal, int cols) {
    return measure("DFS", [&]() {
        PathResult result;
        std::vector<bool> visited(graph.vertex_count(), false);
        std::vector<int> parent(graph.vertex_count(), -1);
        std::stack<int> stack;

        stack.push(start);
        visited[start] = true;

        while (!stack.empty()) {
            int current = stack.top();
            stack.pop();
            ++result.visited_nodes;

            if (current == goal) {
                break;
            }

            for (const auto& [next, weight] : graph.neighbors(current)) {
                if (!visited[next]) {
                    visited[next] = true;
                    parent[next] = current;
                    stack.push(next);
                }
            }
        }

        result.path = rebuild_path(parent, start, goal);
        finalize_result(result, graph, cols);

        return result;
    });
}

PathResult bfs_path(const Graph& graph, int start, int goal, int cols) {
    return measure("BFS", [&]() {
        PathResult result;
        std::vector<bool> visited(graph.vertex_count(), false);
        std::vector<int> parent(graph.vertex_count(), -1);
        std::queue<int> queue;

        queue.push(start);
        visited[start] = true;

        while (!queue.empty()) {
            int current = queue.front();
            queue.pop();
            ++result.visited_nodes;

            if (current == goal) {
                break;
            }

            for (const auto& [next, weight] : graph.neighbors(current)) {
                if (!visited[next]) {
                    visited[next] = true;
                    parent[next] = current;
                    queue.push(next);
                }
            }
        }

        result.path = rebuild_path(parent, start, goal);
        finalize_result(result, graph, cols);

        return result;
    });
}

PathResult dijkstra_path(const Graph& graph, int start, int goal, int cols) {
    return measure("Dijkstra", [&]() {
        PathResult result;

        constexpr int INF = std::numeric_limits<int>::max();
        std::vector<int> dist(graph.vertex_count(), INF);
        std::vector<int> parent(graph.vertex_count(), -1);

        using Node = std::pair<int, int>;
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            auto [cost, current] = pq.top();
            pq.pop();

            if (cost != dist[current]) {
                continue;
            }

            ++result.visited_nodes;

            if (current == goal) {
                break;
            }

            for (const auto& [next, weight] : graph.neighbors(current)) {
                int new_cost = cost + weight;

                if (new_cost < dist[next]) {
                    dist[next] = new_cost;
                    parent[next] = current;
                    pq.push({new_cost, next});
                }
            }
        }

        result.path = rebuild_path(parent, start, goal);
        finalize_result(result, graph, cols);

        return result;
    });
}

PathResult astar_path(const Graph& graph, int start, int goal, int cols) {
    return measure("A*", [&]() {
        PathResult result;

        auto heuristic = [cols](int a, int b) {
            int ar = a / cols;
            int ac = a % cols;
            int br = b / cols;
            int bc = b % cols;

            return std::abs(ar - br) + std::abs(ac - bc);
        };

        constexpr int INF = std::numeric_limits<int>::max();
        std::vector<int> g_score(graph.vertex_count(), INF);
        std::vector<int> parent(graph.vertex_count(), -1);

        using Node = std::pair<int, int>;
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

        g_score[start] = 0;
        pq.push({heuristic(start, goal), start});

        while (!pq.empty()) {
            auto [priority, current] = pq.top();
            pq.pop();

            if (priority > g_score[current] + heuristic(current, goal)) {
                continue;
            }

            ++result.visited_nodes;

            if (current == goal) {
                break;
            }

            for (const auto& [next, weight] : graph.neighbors(current)) {
                int tentative = g_score[current] + weight;

                if (tentative < g_score[next]) {
                    g_score[next] = tentative;
                    parent[next] = current;

                    int f_score = tentative + heuristic(next, goal);
                    pq.push({f_score, next});
                }
            }
        }

        result.path = rebuild_path(parent, start, goal);
        finalize_result(result, graph, cols);

        return result;
    });
}

int connected_components(const Graph& graph) {
    std::vector<bool> visited(graph.vertex_count(), false);
    int count = 0;

    for (int start = 0; start < graph.vertex_count(); ++start) {
        if (visited[start] || !graph.is_active(start)) {
            continue;
        }

        ++count;

        std::queue<int> queue;
        queue.push(start);
        visited[start] = true;

        while (!queue.empty()) {
            int current = queue.front();
            queue.pop();

            for (const auto& [next, weight] : graph.neighbors(current)) {
                if (!visited[next] && graph.is_active(next)) {
                    visited[next] = true;
                    queue.push(next);
                }
            }
        }
    }

    return count;
}

} // namespace graph