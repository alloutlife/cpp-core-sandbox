#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

struct A {
    int a{0};
};

class FactoryI {
  public:
    virtual ~FactoryI() = default;

    [[nodiscard]] virtual auto getInstance() -> std::shared_ptr<A> = 0;
};

class FactoryLock : public FactoryI {
    // DATA
    std::shared_ptr<A> d_instance;
    std::shared_mutex d_mutex;

  public:
    [[nodiscard]] auto getInstance() -> std::shared_ptr<A> override {
        {
            std::shared_lock readLock{d_mutex};
            if (d_instance) {
                return d_instance;
            }
        }

        std::lock_guard writeLock{d_mutex};
        if (!d_instance) {
            d_instance = std::make_shared<A>();
        }
        return d_instance;
    }
};

class FactoryAtomic : public FactoryI {
    // DATA
    std::shared_ptr<A> d_instance;
    std::mutex d_mutex;

  public:
    [[nodiscard]] auto getInstance() -> std::shared_ptr<A> override {

        if (auto ptr = std::atomic_load_explicit(&d_instance,
                                                 std::memory_order_acquire);
            ptr) {
            return ptr;
        }

        std::lock_guard guard{d_mutex};
        if (auto ptr = std::atomic_load_explicit(&d_instance,
                                                 std::memory_order_acquire);
            ptr) {
            return ptr;
        }

        auto ptr = std::make_shared<A>();
        std::atomic_store_explicit(&d_instance, ptr, std::memory_order_release);
        return ptr;
    }
};

class FactoryOnce : public FactoryI {
    // DATA
    std::shared_ptr<A> d_instance;
    std::once_flag d_once;

  public:
    [[nodiscard]] auto getInstance() -> std::shared_ptr<A> override {
        std::call_once(d_once,
                       [this]() { d_instance = std::make_shared<A>(); });
        return d_instance;
    }
};

struct BenchParams {
    int threadCount;
    int callsPerThread;
};
auto runBenchmark(const char *name, FactoryI &factory, BenchParams params)
    -> void {
    using clock = std::chrono::steady_clock;
    auto sink = std::atomic<std::uintptr_t>{0};

    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(params.threadCount));

    auto start = clock::now();
    for (int i = 0; i < params.threadCount; ++i) {
        threads.emplace_back([&factory, params, &sink]() {
            uintptr_t local{0};
            for (int j = 0; j < params.callsPerThread; ++j) {
                auto ptr = factory.getInstance();
                local += reinterpret_cast<std::uintptr_t>(ptr.get());
            }
            sink.fetch_add(local);
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        clock::now() - start);

    std::cout << name << ": " << elapsed.count() << " us"
              << ", checksum=" << sink.load(std::memory_order_relaxed) << '\n';
}

int main() {
    constexpr BenchParams benchParams{
        .threadCount = 8,
        .callsPerThread = 25'000'000,
    };

    FactoryLock lockFactory;
    FactoryAtomic atomicFactory;
    FactoryOnce onceFactory;

    runBenchmark("FactoryLock", lockFactory, benchParams);
    runBenchmark("FactoryAtomic", atomicFactory, benchParams);
    runBenchmark("FactoryOnce", onceFactory, benchParams);

    // Results on macOS:
    //
    // FactoryLock: 156002 us, checksum=8765456752000000
    // FactoryAtomic: 53019 us, checksum=8765453680000000
    // FactoryOnce: 195208 us, checksum=104656278128000000

    return 0;
}
