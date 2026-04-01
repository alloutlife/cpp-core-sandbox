#include "ranges_playground.h"

#include <cstddef>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

namespace {

std::random_device s_rd;
std::seed_seq s_seed{s_rd()};

std::mt19937 s_rng{s_seed};
std::uniform_int_distribution<> s_dist{1, 255};

std::vector<int> makeInput(std::size_t size)
{
    std::vector<int> values(size);
    for (auto &value : values) {
        value = s_dist(s_rng);
    }

    return values;
}

void benchmarkFilterThenReverse(benchmark::State &state)
{
    const auto input = makeInput(static_cast<std::size_t>(state.range(0)));

    for ([[maybe_unused]] auto _ : state) {
        auto output = ranges_playground::filterThenReverse(input);
        benchmark::DoNotOptimize(output);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

void benchmarkReverseThenFilter(benchmark::State &state)
{
    const auto input = makeInput(static_cast<std::size_t>(state.range(0)));

    for ([[maybe_unused]] auto _ : state) {
        auto output = ranges_playground::reverseThenFilter(input);
        benchmark::DoNotOptimize(output);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

} // namespace

BENCHMARK(benchmarkFilterThenReverse)
    ->RangeMultiplier(8)
    ->Range(1 << 10, 1 << 20);
BENCHMARK(benchmarkReverseThenFilter)
    ->RangeMultiplier(8)
    ->Range(1 << 10, 1 << 20);

BENCHMARK_MAIN();
