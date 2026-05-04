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