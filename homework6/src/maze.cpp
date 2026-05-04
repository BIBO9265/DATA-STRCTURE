#include "graph/maze.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

namespace graph {

namespace {

std::string read_all_text(const std::string& file_path) {
    std::ifstream file(file_path);

    if (!file) {
        throw std::runtime_error("Cannot open JSON map file: " + file_path);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int extract_int_field(const std::string& text, const std::string& key) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+)");
    std::smatch match;

    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Missing integer field: " + key);
    }

    return std::stoi(match[1].str());
}

Point extract_point_field(const std::string& text, const std::string& key) {
    std::regex pattern(
        "\"" + key + "\"\\s*:\\s*\\{[^\\}]*\"row\"\\s*:\\s*(-?\\d+)\\s*,\\s*\"col\"\\s*:\\s*(-?\\d+)"
    );

    std::smatch match;

    if (!std::regex_search(text, match, pattern)) {
        throw std::runtime_error("Missing point field: " + key);
    }

    return {std::stoi(match[1].str()), std::stoi(match[2].str())};
}

std::string extract_grid_block(const std::string& text) {
    const std::string key = "\"grid\"";
    std::size_t key_pos = text.find(key);

    if (key_pos == std::string::npos) {
        throw std::runtime_error("Missing field: grid");
    }

    std::size_t start = text.find('[', key_pos);

    if (start == std::string::npos) {
        throw std::runtime_error("Invalid grid format");
    }

    int depth = 0;

    for (std::size_t i = start; i < text.size(); ++i) {
        if (text[i] == '[') {
            ++depth;
        } else if (text[i] == ']') {
            --depth;

            if (depth == 0) {
                return text.substr(start, i - start + 1);
            }
        }
    }

    throw std::runtime_error("Invalid grid bracket structure");
}

std::vector<int> extract_numbers(const std::string& text) {
    std::vector<int> values;
    std::regex pattern("-?\\d+");

    auto begin = std::sregex_iterator(text.begin(), text.end(), pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        values.push_back(std::stoi((*it).str()));
    }

    return values;
}

} // namespace

Maze::Maze(const std::string& file_path) {
    const std::string text = read_all_text(file_path);

    rows_ = extract_int_field(text, "rows");
    cols_ = extract_int_field(text, "cols");
    start_ = extract_point_field(text, "start");
    goal_ = extract_point_field(text, "goal");

    const std::string grid_block = extract_grid_block(text);
    const std::vector<int> values = extract_numbers(grid_block);

    if (static_cast<int>(values.size()) != rows_ * cols_) {
        throw std::runtime_error("Grid size does not match rows * cols");
    }

    grid_.assign(rows_, std::vector<int>(cols_, 0));

    int index = 0;

    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            grid_[r][c] = values[index++];
        }
    }

    validate();
}

Graph Maze::to_graph() const {
    Graph graph(rows() * cols());

    const std::vector<Point> directions = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    for (int r = 0; r < rows(); ++r) {
        for (int c = 0; c < cols(); ++c) {
            Point current{r, c};

            if (!is_walkable(current)) {
                continue;
            }

            int from = point_to_id(current);
            graph.set_active(from, true);

            for (const auto& d : directions) {
                Point next{r + d.row, c + d.col};

                if (is_walkable(next)) {
                    int to = point_to_id(next);

                    // Cost rule:
                    // cost(u -> v) = cost of entering cell v.
                    graph.add_edge(from, to, cell_cost(next));
                }
            }
        }
    }

    return graph;
}

int Maze::rows() const {
    return rows_;
}

int Maze::cols() const {
    return cols_;
}

int Maze::start_id() const {
    return point_to_id(start_);
}

int Maze::goal_id() const {
    return point_to_id(goal_);
}

Point Maze::id_to_point(int id) const {
    return {id / cols(), id % cols()};
}

int Maze::point_to_id(Point p) const {
    return p.row * cols() + p.col;
}

bool Maze::is_walkable(Point p) const {
    return in_bounds(p) && grid_[p.row][p.col] != wall_value_;
}

int Maze::cell_cost(Point p) const {
    if (!is_walkable(p)) {
        throw std::runtime_error("Cannot get cost of wall cell");
    }

    return grid_[p.row][p.col];
}

bool Maze::in_bounds(Point p) const {
    return p.row >= 0 && p.row < rows_ && p.col >= 0 && p.col < cols_;
}

void Maze::validate() const {
    if (rows_ <= 0 || cols_ <= 0) {
        throw std::runtime_error("Rows and cols must be positive");
    }

    if (!in_bounds(start_)) {
        throw std::runtime_error("Start position is out of bounds");
    }

    if (!in_bounds(goal_)) {
        throw std::runtime_error("Goal position is out of bounds");
    }

    if (!is_walkable(start_)) {
        throw std::runtime_error("Start position cannot be a wall");
    }

    if (!is_walkable(goal_)) {
        throw std::runtime_error("Goal position cannot be a wall");
    }

    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (grid_[r][c] < 0) {
                throw std::runtime_error("Grid cost cannot be negative");
            }
        }
    }
}

} // namespace graph