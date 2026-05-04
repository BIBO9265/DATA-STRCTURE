#pragma once

#include <string>
#include <vector>

namespace graph {

struct Point {
    int row{};
    int col{};

    bool operator==(const Point& other) const {
        return row == other.row && col == other.col;
    }
};

struct PathResult {
    std::string algorithm;
    bool found{};
    std::vector<int> path;

    int path_length{};
    int path_cost{};
    int turn_count{};
    int visited_nodes{};

    double elapsed_ms{};
};

} // namespace graph