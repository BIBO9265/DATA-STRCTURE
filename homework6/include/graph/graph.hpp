#pragma once

#include <utility>
#include <vector>

namespace graph {

class Graph {
public:
    explicit Graph(int vertex_count = 0);

    void add_edge(int from, int to, int weight = 1);

    void set_active(int vertex, bool active = true);
    bool is_active(int vertex) const;

    int vertex_count() const;

    const std::vector<std::pair<int, int>>& neighbors(int vertex) const;

private:
    std::vector<std::vector<std::pair<int, int>>> adjacency_list_;
    std::vector<bool> active_;
};

} // namespace graph