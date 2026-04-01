#pragma once

#include <span>
#include <vector>

namespace ranges_playground {

bool shouldKeepAfterModuloFilter(int value);

std::vector<int> filterThenReverse(std::span<const int> input);

std::vector<int> reverseThenFilter(std::span<const int> input);

} // namespace ranges_playground
