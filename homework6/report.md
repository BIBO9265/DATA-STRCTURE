# 41343115

作業二

## 解題說明

本次作業根據 Graph 圖形資料結構章節，實作圖形的基本操作與常見路徑搜尋演算法。Graph 是由頂點（Vertex）與邊（Edge）組成的資料結構，本專案將迷宮路徑搜尋問題抽象為圖形問題，並比較不同圖形演算法在路徑長度、路徑成本、搜尋節點數與執行時間上的差異。

本專案使用 **Adjacency List（鄰接串列）** 儲存圖形。迷宮中的每一個可通行格子視為一個頂點，兩個相鄰且可通行的格子之間建立一條邊。若格子為牆壁，則不建立對應的有效邊。

本作業實作以下演算法：

1. DFS（Depth First Search，深度優先搜尋）
2. BFS（Breadth First Search，廣度優先搜尋）
3. Connected Components（連通元件）
4. Dijkstra 最短路徑演算法
5. A* 搜尋演算法

本次程式以 **無向加權圖** 作為主要模型。DFS、BFS 與 Connected Components 主要用於圖形走訪與可達性分析；Dijkstra 與 A* 則使用邊權重計算最低成本路徑。

### 圖形模型

本專案將迷宮抽象為 Graph：

| 迷宮元素 | Graph 對應 |
|----------|------------|
| 可通行格子 | Vertex |
| 相鄰且可通行的兩格 | Edge |
| 牆壁 | 不建立有效 Edge |
| 移動成本 | Edge Weight |
| 起點 | Source Vertex |
| 終點 | Target Vertex |

本專案地圖使用 JSON 格式儲存，格子數值定義如下：

| 數值 | 意義 |
|------|------|
| 0 | 牆壁，不可通行 |
| 1 | 一般可通行格，成本 1 |
| 2 | 中等成本格，成本 2 |
| 3 | 高成本格，成本 3 |

本程式採用的邊成本規則為：

```text
cost(u -> v) = cost(v)
```

也就是從目前格子 `u` 移動到下一格 `v` 時，移動成本等於進入格子 `v` 的成本。

### 圖形類型與演算法適用範圍

| 演算法 | 可用於有向圖 | 可用於無向圖 | 可用於有權圖 | 可用於無權圖 | 是否保證最佳路徑 | 說明 |
|--------|--------------|--------------|--------------|--------------|------------------|------|
| DFS | 可以 | 可以 | 可以 | 可以 | 否 | 適合走訪與探索，但不保證最短路徑或最低成本。 |
| BFS | 可以 | 可以 | 不適合直接處理權重 | 可以 | 是，限無權圖 | 在無權圖中可找最少邊數路徑。 |
| Connected Components | 不適合直接用於有向圖 | 可以 | 可以 | 可以 | 不處理路徑最佳化 | 用於判斷無向圖被分成幾個連通區塊。 |
| Dijkstra | 可以 | 可以 | 需要非負權重 | 可以 | 是 | 適合非負權重圖的最低成本路徑。 |
| A* | 可以 | 可以 | 需要非負權重 | 可以 | 是，若 heuristic 不高估 | 利用啟發式函數減少搜尋範圍。 |

### 解題策略

1. 使用 JSON 檔案讀取迷宮地圖、起點、終點與格子成本。
2. 將每個可通行格子轉換為 Graph 中的 vertex。
3. 若兩個格子上下左右相鄰且皆可通行，則建立 edge。
4. 使用 Adjacency List 儲存 graph，以節省空間。
5. 使用 DFS 與 BFS 作為基本 graph traversal。
6. 使用 Connected Components 判斷圖中有幾個連通區塊。
7. 使用 Dijkstra 計算加權圖中的最低成本路徑。
8. 使用 A* 搭配 Manhattan Distance 作為 heuristic，加速路徑搜尋。
9. 統計各演算法的路徑長度、路徑成本、轉彎次數、搜尋節點數與執行時間。
10. 將測試結果輸出到終端機，並另外寫入 `output/benchmark_results.csv`。

A* 使用的估計函數為 Manhattan Distance：

```text
h(n) = |row(n) - row(goal)| + |col(n) - col(goal)|
```

A* 的核心評估函數為：

```text
f(n) = g(n) + h(n)
```

其中：

- `g(n)`：從起點到目前節點的實際成本
- `h(n)`：目前節點到終點的估計成本
- `f(n)`：節點的優先級

## 程式實作

### 專案架構

本專案採用模組化架構，主程式只負責流程控制與輸出，演算法本身寫在 library-style 的檔案中。

```text
Graph/
├── README.md
├── report.md
├── graph.exe
├── data/
│   └── maps/
│       ├── maze_5x5_basic.json
│       ├── maze_weighted_7x7.json
│       └── maze_unreachable_6x6.json
├── include/
│   └── graph/
│       ├── algorithms.hpp
│       ├── benchmark.hpp
│       ├── graph.hpp
│       ├── maze.hpp
│       └── types.hpp
├── output/
│   └── benchmark_results.csv
└── src/
    ├── algorithms.cpp
    ├── benchmark.cpp
    ├── graph.cpp
    ├── main.cpp
    └── maze.cpp
```

### 完整程式碼

#### `include/graph/types.hpp`

```cpp
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
```

#### `include/graph/graph.hpp`

```cpp
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
```

#### `include/graph/maze.hpp`

```cpp
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
```

#### `include/graph/algorithms.hpp`

```cpp
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
```

#### `include/graph/benchmark.hpp`

```cpp
#pragma once

#include <string>
#include <vector>

#include "types.hpp"

namespace graph {

void write_benchmark_csv(
    const std::string& file_path,
    const std::string& map_file,
    int rows,
    int cols,
    int connected_components,
    const std::vector<PathResult>& results
);

} // namespace graph
```

#### `src/graph.cpp`

```cpp
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
```

#### `src/maze.cpp`

```cpp
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
```

#### `src/algorithms.cpp`

```cpp
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
                (void)weight;
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
                (void)weight;
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
                (void)weight;
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
```

#### `src/benchmark.cpp`

```cpp
#include "graph/benchmark.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace graph {

void write_benchmark_csv(
    const std::string& file_path,
    const std::string& map_file,
    int rows,
    int cols,
    int connected_components,
    const std::vector<PathResult>& results
) {
    std::filesystem::path output_path(file_path);

    if (output_path.has_parent_path()) {
        std::filesystem::create_directories(output_path.parent_path());
    }

    std::ofstream file(file_path);

    if (!file) {
        throw std::runtime_error("Cannot open benchmark output file: " + file_path);
    }

    file << "map_file,rows,cols,connected_components,"
         << "algorithm,found,path_length,path_cost,turn_count,visited_nodes,elapsed_ms\n";

    for (const auto& result : results) {
        file
            << map_file << ","
            << rows << ","
            << cols << ","
            << connected_components << ","
            << result.algorithm << ","
            << (result.found ? "Yes" : "No") << ","
            << result.path_length << ","
            << result.path_cost << ","
            << result.turn_count << ","
            << result.visited_nodes << ","
            << result.elapsed_ms
            << "\n";
    }
}

} // namespace graph
```

#### `src/main.cpp`

```cpp
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "graph/algorithms.hpp"
#include "graph/benchmark.hpp"
#include "graph/maze.hpp"

using graph::PathResult;

void print_result(const PathResult& result) {
    std::cout
        << std::left << std::setw(14) << result.algorithm
        << std::setw(10) << (result.found ? "Yes" : "No")
        << std::setw(12) << result.path_length
        << std::setw(12) << result.path_cost
        << std::setw(12) << result.turn_count
        << std::setw(14) << result.visited_nodes
        << std::fixed << std::setprecision(4)
        << result.elapsed_ms
        << '\n';
}

int main(int argc, char* argv[]) {
    std::string maze_file = "data/maps/maze_5x5_basic.json";

    if (argc >= 2) {
        maze_file = argv[1];
    }

    try {
        graph::Maze maze(maze_file);
        graph::Graph maze_graph = maze.to_graph();

        const int start = maze.start_id();
        const int goal = maze.goal_id();
        const int component_count = graph::connected_components(maze_graph);

        std::cout << "Homework 2 Graph - Maze Pathfinding Benchmark\n";
        std::cout << "Map file: " << maze_file << "\n";
        std::cout << "Rows: " << maze.rows() << ", Cols: " << maze.cols() << "\n";
        std::cout << "Connected Components: " << component_count << "\n\n";

        std::vector<PathResult> results;
        results.push_back(graph::dfs_path(maze_graph, start, goal, maze.cols()));
        results.push_back(graph::bfs_path(maze_graph, start, goal, maze.cols()));
        results.push_back(graph::dijkstra_path(maze_graph, start, goal, maze.cols()));
        results.push_back(graph::astar_path(maze_graph, start, goal, maze.cols()));

        std::cout
            << std::left << std::setw(14) << "Algorithm"
            << std::setw(10) << "Found"
            << std::setw(12) << "Length"
            << std::setw(12) << "Cost"
            << std::setw(12) << "Turns"
            << std::setw(14) << "Visited"
            << "Time(ms)"
            << '\n';

        std::cout << std::string(86, '-') << '\n';

        for (const auto& result : results) {
            print_result(result);
        }

        const std::string benchmark_file = "output/benchmark_results.csv";

        graph::write_benchmark_csv(
            benchmark_file,
            maze_file,
            maze.rows(),
            maze.cols(),
            component_count,
            results
        );

        std::cout << "\nBenchmark CSV written to: " << benchmark_file << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
```

### 測試地圖資料

#### `data/maps/maze_5x5_basic.json`

```json
{
  "schema_version": "1.1",
  "name": "maze_5x5_basic",
  "description": "Basic reachable maze for correctness verification.",
  "rows": 5,
  "cols": 5,
  "start": {
    "row": 0,
    "col": 0
  },
  "goal": {
    "row": 4,
    "col": 4
  },
  "cost_rule": {
    "wall_value": 0,
    "movement_cost": "entering_cell_cost",
    "allow_diagonal": false
  },
  "grid": [
    [1, 1, 1, 0, 1],
    [0, 0, 1, 0, 1],
    [1, 1, 1, 1, 1],
    [1, 0, 0, 0, 1],
    [1, 1, 1, 1, 1]
  ]
}
```

#### `data/maps/maze_weighted_7x7.json`

```json
{
  "schema_version": "1.1",
  "name": "maze_weighted_7x7",
  "description": "Weighted maze designed to compare BFS, Dijkstra, and A*. The shortest step path has higher cost, while a longer path has lower cost.",
  "rows": 7,
  "cols": 7,
  "start": {
    "row": 0,
    "col": 0
  },
  "goal": {
    "row": 0,
    "col": 6
  },
  "cost_rule": {
    "wall_value": 0,
    "movement_cost": "entering_cell_cost",
    "allow_diagonal": false
  },
  "grid": [
    [1, 3, 3, 3, 3, 3, 1],
    [1, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 0, 1],
    [0, 0, 0, 0, 1, 0, 1],
    [1, 1, 1, 0, 1, 1, 1],
    [1, 0, 1, 0, 0, 0, 1],
    [1, 0, 1, 1, 1, 1, 1]
  ]
}
```

#### `data/maps/maze_unreachable_6x6.json`

```json
{
  "schema_version": "1.1",
  "name": "maze_unreachable_6x6",
  "description": "Unreachable maze for testing connected components and failure handling.",
  "rows": 6,
  "cols": 6,
  "start": {
    "row": 0,
    "col": 0
  },
  "goal": {
    "row": 5,
    "col": 5
  },
  "cost_rule": {
    "wall_value": 0,
    "movement_cost": "entering_cell_cost",
    "allow_diagonal": false
  },
  "grid": [
    [1, 1, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0],
    [0, 0, 0, 1, 1, 1],
    [0, 0, 0, 1, 0, 1],
    [0, 0, 0, 1, 1, 1]
  ]
}
```

### 編譯與執行指令

本專案可使用 g++ 編譯：

```shell
g++ -std=c++17 -g -Wall -Wextra -I include src/main.cpp src/graph.cpp src/maze.cpp src/algorithms.cpp src/benchmark.cpp -o graph.exe
```

Windows PowerShell 環境可使用：

```shell
.\graph.exe data\maps\maze_5x5_basic.json
.\graph.exe data\maps\maze_weighted_7x7.json
.\graph.exe data\maps\maze_unreachable_6x6.json
```

## 效能分析

令圖中有：

- `V`：頂點數量
- `E`：邊數量

由於本專案使用 adjacency list，圖形儲存空間為：

```text
O(V + E)
```

各演算法複雜度如下：

| 演算法 | 時間複雜度 | 空間複雜度 | 說明 |
|--------|------------|------------|------|
| DFS | O(V + E) | O(V) | 每個頂點與每條邊最多被走訪一次。 |
| BFS | O(V + E) | O(V) | 使用 queue 逐層走訪。 |
| Connected Components | O(V + E) | O(V) | 透過多次 DFS 或 BFS 找出所有連通區塊。 |
| Dijkstra | O((V + E) log V) | O(V + E) | 使用 priority queue 取得目前最低成本節點。 |
| A* | 最壞 O((V + E) log V) | O(V + E) | 使用 heuristic 減少實際展開的節點數。 |

### 各演算法特性比較

| 演算法 | 路徑品質 | 是否考慮權重 | 搜尋速度 | 適合用途 |
|--------|----------|--------------|----------|----------|
| DFS | 不穩定 | 否 | 可能很快，也可能很差 | 基本走訪、探索 |
| BFS | 無權圖最佳 | 否 | 中等 | 無權圖最少步數路徑 |
| Connected Components | 不產生路徑 | 否 | 快 | 可達性與連通性分析 |
| Dijkstra | 加權圖最佳 | 是 | 中等偏慢 | 非負權重圖最低成本路徑 |
| A* | 加權圖最佳，若 heuristic 合法 | 是 | 通常比 Dijkstra 快 | 具有空間座標的路徑搜尋 |

## 測試與驗證

### 測試地圖

本專案使用三組 JSON 地圖作為測試資料：

| 測試地圖 | 用途 |
|----------|------|
| `maze_5x5_basic.json` | 基本可達地圖，用於驗證所有演算法能正確找到路徑 |
| `maze_weighted_7x7.json` | 加權地圖，用於比較 BFS、Dijkstra 與 A* 在成本模型下的差異 |
| `maze_unreachable_6x6.json` | 不可達地圖，用於測試 Connected Components 與找不到路徑的處理 |

### 測試案例

| 測試案例 | 測試內容 | 預期結果 | 實際結果 |
|----------|----------|----------|----------|
| 測試一 | 5x5 基本可達地圖 | 所有演算法皆可找到路徑，Connected Components = 1 | 符合預期 |
| 測試二 | 7x7 加權地圖 | BFS 找最少步數，Dijkstra 與 A* 找最低成本 | 符合預期 |
| 測試三 | 6x6 不可達地圖 | Connected Components = 2，所有演算法 Found = No | 符合預期 |
| 測試四 | A* 與 Dijkstra 比較 | A* 與 Dijkstra 找到相同最低成本，A* 通常展開較少節點 | 符合預期 |
| 測試五 | CSV 輸出 | 程式產生 `output/benchmark_results.csv` | 符合預期 |

### 基本地圖執行結果

執行指令：

```shell
.\graph.exe data\maps\maze_5x5_basic.json
```

執行結果：

```text
Homework 2 Graph - Maze Pathfinding Benchmark
Map file: data\maps\maze_5x5_basic.json
Rows: 5, Cols: 5
Connected Components: 1

Algorithm     Found     Length      Cost        Turns       Visited       Time(ms)
--------------------------------------------------------------------------------------
DFS           Yes       8           8           3           9             0.0388
BFS           Yes       8           8           3           15            0.0235
Dijkstra      Yes       8           8           3           15            0.0299
A*            Yes       8           8           3           9             0.0228

Benchmark CSV written to: output/benchmark_results.csv
```

結果說明：

1. Connected Components 為 1，代表起點與終點位於同一個連通區塊。
2. 所有演算法皆成功找到路徑。
3. 由於基本地圖中可通行格子的成本皆為 1，因此最少步數路徑也等於最低成本路徑。
4. BFS、Dijkstra 與 A* 的路徑長度與成本相同。
5. A* 展開的節點數低於 BFS 與 Dijkstra，顯示 heuristic 有助於縮小搜尋範圍。

### 不可達地圖執行結果

執行指令：

```shell
.\graph.exe data\maps\maze_unreachable_6x6.json
```

執行結果：

```text
Homework 2 Graph - Maze Pathfinding Benchmark
Map file: data\maps\maze_unreachable_6x6.json
Rows: 6, Cols: 6
Connected Components: 2

Algorithm     Found     Length      Cost        Turns       Visited       Time(ms)
--------------------------------------------------------------------------------------
DFS           No        0           0           0           4             0.0143
BFS           No        0           0           0           4             0.0050
Dijkstra      No        0           0           0           4             0.0060
A*            No        0           0           0           4             0.0042

Benchmark CSV written to: output/benchmark_results.csv
```

結果說明：

1. Connected Components 為 2，代表可通行區域被分成兩個不連通區塊。
2. 起點與終點不在同一個連通區塊，因此所有演算法皆無法找到路徑。
3. DFS、BFS、Dijkstra 與 A* 均正確輸出 `Found = No`。
4. 此測試確認程式能正確處理不可達地圖，不會輸出錯誤路徑。

### 加權地圖執行結果

執行指令：

```shell
.\graph.exe data\maps\maze_weighted_7x7.json
```

執行結果：

```text
Homework 2 Graph - Maze Pathfinding Benchmark
Map file: data\maps\maze_weighted_7x7.json
Rows: 7, Cols: 7
Connected Components: 1

Algorithm     Found     Length      Cost        Turns       Visited       Time(ms)
--------------------------------------------------------------------------------------
DFS           Yes       6           16          0           7             0.0551
BFS           Yes       6           16          0           13            0.0318
Dijkstra      Yes       14          14          4           22            0.0945
A*            Yes       14          14          4           19            0.0542

Benchmark CSV written to: output/benchmark_results.csv
```

結果說明：

1. BFS 找到的路徑長度為 6，但總成本為 16。
2. Dijkstra 與 A* 找到的路徑長度為 14，但總成本下降為 14。
3. 此結果顯示，在加權圖中，最少步數路徑不一定等於最低成本路徑。
4. BFS 適合無權圖或等成本圖，但不適合用來最佳化 weighted path cost。
5. Dijkstra 與 A* 能正確處理非負權重圖中的最低成本路徑。
6. A* 與 Dijkstra 找到相同最低成本路徑，但 A* 的 visited nodes 較少，表示 heuristic 可以減少搜尋範圍。

## 申論及開發報告

### 選擇 Adjacency List 的原因

本專案使用 Adjacency List 作為 Graph 的儲存方式，主要原因是迷宮圖屬於 sparse graph。若迷宮大小為 `R × C`，則最多有 `V = R × C` 個頂點，但每個頂點最多只會與上、下、左、右四個方向相連，因此邊數 `E` 大約與 `V` 呈線性關係。

若使用 Adjacency Matrix，需要 `O(V^2)` 空間。當地圖變大時，矩陣會包含大量不存在的邊，造成空間浪費。因此本專案使用 Adjacency List，使空間複雜度降為 `O(V + E)`，更適合迷宮路徑搜尋問題。

### 選擇 DFS 的原因

DFS 是圖形走訪的基本演算法之一，適合用來展示圖中節點的可達性。DFS 採用「先往深處搜尋，走到底再回溯」的策略。

在本專案中，DFS 被用作 baseline。DFS 可以找到一條從起點到終點的路徑，但它不保證該路徑是最短或最低成本。因此 DFS 的結果主要用來與 BFS、Dijkstra 和 A* 進行比較。

### 選擇 BFS 的原因

BFS 是圖形搜尋的重要演算法，採用一層一層向外擴展的方式搜尋。若每條邊的成本相同，BFS 可以保證找到最少邊數的路徑。

在本專案中，BFS 適合用於 unweighted map 或所有通行成本皆相同的地圖。但在 weighted map 中，BFS 只會找到步數最短的路徑，不會考慮不同格子的成本，因此可能得到總成本較高的路徑。

### 選擇 Connected Components 的原因

Connected Components 可用於分析無向圖被分成幾個彼此不連通的區塊。在本專案中，它被定位為路徑搜尋前的可達性檢查。

若起點與終點位於不同 connected component，則代表兩者之間不存在任何有效路徑。此時即使執行 DFS、BFS、Dijkstra 或 A*，也不可能找到解。因此 Connected Components 雖然不直接最佳化路徑成本，但能有效判斷路徑搜尋問題是否具備可行性。

### 選擇 Dijkstra 的原因

Dijkstra 是經典的最短路徑演算法，適用於邊權重非負的 weighted graph。由於本專案中的地圖格子具有不同通行成本，因此 Dijkstra 可用來計算從起點到終點的最低成本路徑。

在 weighted map 測試中，Dijkstra 找到的路徑雖然比 BFS 更長，但總成本較低，說明 Dijkstra 能處理 BFS 無法處理的加權成本問題。

### 選擇 A* 的原因

A* 是在 Dijkstra 基礎上加入 heuristic 的路徑搜尋演算法。本專案使用 Manhattan Distance 作為 heuristic，因為地圖僅允許上下左右移動，不允許對角線移動。

A* 的優點是可以在維持最佳性的前提下，減少不必要的節點展開。在加權地圖測試中，A* 與 Dijkstra 找到相同最低成本，但 A* 的 visited nodes 較少，顯示 heuristic 對搜尋效率具有幫助。

### 開發過程遇到的問題與解決方式

1. **如何設計地圖格式**

   一開始可以使用文字地圖表示起點、終點、牆壁與道路，但該方式擴充性較差。因此本專案改用 JSON 格式，將地圖大小、起點、終點、成本規則與 grid 分開描述，使資料結構更清楚。

2. **如何表示牆壁與可通行格子**

   本專案使用 `0` 表示牆壁，使用正整數表示可通行格子的成本。這樣可以同時支援一般地圖與加權地圖。

3. **如何讓 BFS 與 Dijkstra 顯示不同結果**

   若所有格子成本皆為 1，則 BFS 與 Dijkstra 會找到相同成本的路徑。因此本專案設計了 weighted map，讓最短步數路徑與最低成本路徑不同，以展示加權圖中 Dijkstra 與 A* 的必要性。

4. **如何處理不可達地圖**

   本專案加入 `maze_unreachable_6x6.json` 作為測試案例，用來確認當起點與終點不連通時，所有演算法都能正確輸出 `Found = No`。

5. **如何維持主程式乾淨**

   本專案將 Graph、Maze、演算法與 benchmark 邏輯拆成不同檔案。`main.cpp` 只負責讀取地圖、呼叫函式與輸出結果，避免主程式混入演算法細節。

6. **如何輸出 benchmark 結果**

   後續加入 `benchmark.cpp` 與 `benchmark.hpp`，將每次演算法測試結果寫入 `output/benchmark_results.csv`。如此可以方便後續製作表格或進一步分析。

## 分工說明

本次作業由組員共同完成，分工如下：

| 學號 | 姓名 | 負責內容 |
|------|------|----------|
| 41343115 | 周子新 | Graph 專案架構設計、Adjacency List 實作、DFS 與 BFS 實作、Connected Components 實作、Dijkstra 與 A* 實作、JSON 地圖設計、測試案例設計、benchmark CSV 輸出、效能分析與報告整理 |

### 結論

本次作業完成了以 Adjacency List 為基礎的 Graph 路徑搜尋專案，並實作 DFS、BFS、Connected Components、Dijkstra 與 A*。透過基本地圖、加權地圖與不可達地圖三種測試資料，可以觀察到不同演算法的特性。

在無權圖中，BFS 能找到最少步數路徑；在加權圖中，Dijkstra 與 A* 能找到最低成本路徑；A* 在使用合適 heuristic 時，通常能以較少 visited nodes 找到與 Dijkstra 相同成本的最佳路徑；Connected Components 則可用於路徑搜尋前的可達性分析。

因此，本專案不僅完成 Graph 基本操作，也展示了不同圖形演算法在實際路徑搜尋問題中的適用場景與效能差異。
