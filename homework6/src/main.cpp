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