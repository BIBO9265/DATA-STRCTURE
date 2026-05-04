#include "graph/graph.hpp"

namespace graph {

Graph::Graph(int vertex_count)
    : adjacency_list_(vertex_count),
      active_(vertex_count, false) {}

void Graph::add_edge(int from, int to, int weight) {
    adjacency_list_[from].push_back({to, weight});
}

void Graph::set_active(int vertex, bool active) {
    active_[vertex] = active;
}

bool Graph::is_active(int vertex) const {
    return active_[vertex];
}

int Graph::vertex_count() const {
    return static_cast<int>(adjacency_list_.size());
}

const std::vector<std::pair<int, int>>& Graph::neighbors(int vertex) const {
    return adjacency_list_[vertex];
}

} // namespace graph