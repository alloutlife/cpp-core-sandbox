#include "ranges_playground.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <random>
#include <ranges>

/* Terms:

- Range - a collection of objects which can be iterated.
    Examples: vector, string, list, array, C-arrays, views::iota()

- Range factory
    EXample: std::views::iota

- View - an object, a range that transforms another underlying range.
    Example:
        `auto v = vector<>{} | views::take(n)`
                                        ^--------- View Adapter
              ^------- View, type is std::ranges::take_view<
                                std::ranges::ref_view<std::vector<int>>>

- View Adapter - takes a range, returns a view object. May be chained with
other view adapters using the | operator. Adapters are in the
`std::ranges::views namespace`

*/

std::random_device s_rd; // recall it is external, slow, non-deterministic
std::seed_seq s_seed{s_rd(), s_rd(), s_rd(),
                     s_rd()}; // mt19937 is happy to take a long seed sequence

std::mt19937 s_rng{s_seed};
std::uniform_int_distribution<> s_dist{1, 255};

auto randomSequence(auto &rng, auto &dist)
{
    // Infinite generator of random numbers. Use the range factory
    // `std::views::iota`.
    //
    // NOTE: the counter produced by `iota` is ignored. The incrementing effort
    // is negligible compared to the `dist(rng)`. And overall, this is an
    // idiomatic way of generating infinite sequence.
    return std::views::iota(0) |
           std::views::transform([&]([[maybe_unused]] auto) {
               // comment
               return dist(rng);
           });
}

auto randomSequence_alt(auto &rng, auto &dist)
{
    // The same as above but using the explicit types, not the adapters.
    // As a consequence, they are not composable via | operator
    std::ranges::transform_view r2(std::ranges::iota_view(0),
                                   [&]([[maybe_unused]] auto) {
                                       // comment
                                       return dist(rng);
                                   });
    return r2;
}

void viewsAndViewAdapters()
{
    std::int8_t cArray[4] = {1, 2, 3, 4};

    // These are roughly the same, the type of v1 looks like a closure, although
    // it doesn't imply an additional run-time complexity.
    [[maybe_unused]] auto v1 = cArray | std::views::take(4);
    [[maybe_unused]] std::ranges::take_view v2(cArray, 4);
}

void printView(auto &&r)
{
    std::cout << "size: " << std::ranges::distance(r) << " | ";
    for (const auto &v : r) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

void printView(auto &&r, std::string_view caption)
{
    std::cout << caption;
    printView(r);
}

auto makeViewWithDanglingData()
// ☠️ return a view that refers to a dangling vector, compiler doesn't
{
    std::vector<int> v{1, 2, 3};
    return v | std::views::take(2);

    // NOTE: ✅ this would be safe instead:
    // return std::ranges::take_view(std::vector<int>{1, 2, 3}, 3);

    // NOTE: ✅ this would be safe either:
    // return std::ranges::take_view(std::move(v), 3);
}

void experimentation_1()
{
    // Attempt to pipe an infinite generator to a reverse view adaptor that
    // wants to start from .end
    [[maybe_unused]] auto rv = randomSequence(s_rng, s_dist) |
                               std::views::reverse | std::views::take(5);
    // Would hang forever:
    // printView(rv); // ☠️
}

template <std::ranges::input_range R>
void experimentation_2(R &&input)
// Use of the `input_range` concept
{
    const auto v = input | std::views::drop(3);
    printView(v);
}

int main()
{
    // A lazy generator, no generating happens so far
    auto rv = randomSequence_alt(s_rng, s_dist) | std::views::take(10);

    {
        // Actually we can do like this:
        std::ranges::take_view rv1{randomSequence(s_rng, s_dist), 10};
        printView(rv1);

        // .. but the view adapter version `| std::views::take` is more
        // preferred. View adapters take `|` operands
    }

    // This is where random numbers are generated
    std::vector<int> randomValues;
    std::ranges::copy(rv, std::back_inserter(randomValues));

    printView(randomValues);

    // Create a view on 5 items from `randomValues`.
    // Doesn't own data
    std::ranges::take_view randomValues_view1{randomValues, 5};
    printView(randomValues_view1);

    randomValues.clear();
    assert(randomValues_view1.empty()); // Is also empty now

    // Create an owning view -- a temporary array is passed to the view
    auto view2 = std::views::take(std::vector<int>{1, 2, 3, 4, 5}, 4);
    assert(view2.size() == 4); // Is also empty now
    printView(view2);

    // Compare those two:
    {
        auto isEvenNumber = [](int x) { return x % 2 == 0; };
        auto evenNumbers =
            std::views::iota(0) | std::views::filter(isEvenNumber);
        // evenNumbers.size(); <-- is not implemented!

        // Capped version of the view
        auto tenEvenNums = evenNumbers | std::views::take(10);
        // tenEvenNums.size(); <-- still not implemented, regardless that the
        // view is capped by 10.

        // NOTE: std::ranges::distance(evenNumbers) will hang

        // Use std::ranges::distance to calculate the number of elements
        // But maybe maybe O(n), if .size() is not available
        std::cout << "tenEvenNums size: " << std::ranges::distance(tenEvenNums)
                  << std::endl;

        printView(tenEvenNums);

        auto tenEvenNumsDescending = tenEvenNums | std::views::reverse;
        printView(tenEvenNumsDescending);
    }

    { // the vector-based `view` has size!
        std::vector<int> v{1, 2, 3, 4, 5};
        auto tv = v | std::views::take(3);

        std::cout << "tv size: " << tv.size() << std::endl; // O(1)
        std::cout << "tv distance: " << std::ranges::distance(tv)
                  << std::endl; // maybe O(n)
    }

    // ☠️
    // auto vv = make_view_with_dangling_data();
    // printView(vv);

    experimentation_1();

    std::array a1{1, 2, 3, 4, 5};
    printView(ranges_playground::filterThenReverse(a1), "filter+reverse:");
    printView(ranges_playground::reverseThenFilter(std::array{1, 2, 3, 4, 5}),
              "reverse+filter:"); // <-- preferred order of views

    // TODO: ???
    // Demonstrate an alternative:
    // template <std::ranges::input_range R>
    // void reverseThenFilter(R &&input)
    return 0;
}
