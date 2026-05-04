#pragma once

#include <string>
#include <vector>

#include "graph.hpp"
#include "types.hpp"

namespace graph {

class Maze {
public:
    explicit Maze(const std::string& file_path);

    Graph to_graph() const;

    int rows() const;
    int cols() const;

    int start_id() const;
    int goal_id() const;

    Point id_to_point(int id) const;
    int point_to_id(Point p) const;

    bool is_walkable(Point p) const;
    int cell_cost(Point p) const;

private:
    int rows_{};
    int cols_{};
    int wall_value_{0};

    std::vector<std::vector<int>> grid_;

    Point start_{};
    Point goal_{};

    bool in_bounds(Point p) const;
    void validate() const;
};

} // namespace graph