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