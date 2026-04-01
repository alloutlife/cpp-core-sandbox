#include "ranges_playground.h"

#include <ranges>

namespace ranges_playground {

bool shouldKeepAfterModuloFilter(int value)
{
    return value % 3 == 1;
}

std::vector<int> filterThenReverse(std::span<const int> input)
// `const` is crucial for letting temp objects to be passed
{
    auto view = input | std::views::filter([](int value) {
                    return shouldKeepAfterModuloFilter(value);
                }) |
                std::views::reverse;

    // 🚨 Prefer a lambda here --^: a function-pointer predicate may be harder
    // for the optimizer to inline than a concrete closure type.
    //
    // auto view = input | std::views::filter(&shouldKeepAfterModuloFilter) |
    // std::views::reverse;

    return {view.begin(), view.end()};
}

std::vector<int> reverseThenFilter(std::span<const int> input)
{
    auto view = input | std::views::reverse | std::views::filter([](int value) {
                    return shouldKeepAfterModuloFilter(value);
                });

    return {view.begin(), view.end()};
}

} // namespace ranges_playground
