#include <cassert>
#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

void future_promise_playground(void)
{
    // 1. Promise is destructed while we are blocked on f.wait()
    {
        auto promise1 = std::make_unique<std::promise<std::string>>();
        auto f = promise1->get_future();

        // Setup a background cleanup routine
        std::thread t([p1 = std::move(promise1)] {
            std::this_thread::sleep_for(100ms);
            std::cout << "cleanup p1" << std::endl;
        });

        f.wait(); // infinite wait; we will immediately leave from here once the
                  // promise is destroyed
        std::cout << "exited from f.wait()" << std::endl;

        try {
            std::cout << f.get() << std::endl;
        } catch (std::future_error& ex) {
            // Here what we have!
            assert(ex.code() == std::future_errc::broken_promise);
        } catch (std::exception& ex) {
            assert(false);
            std::cout << ex.what() << std::endl;
        }

        t.join(); // Don't forget to join
    }

    // 2. Normal processing instead of result
    {
        auto promise1 = std::make_unique<std::promise<std::string>>();
        auto f = promise1->get_future();

        // Setup a background cleanup routine
        std::thread t([p1 = std::move(promise1)] {
            std::this_thread::sleep_for(100ms);
            std::cout << "setting p1" << std::endl;
            p1->set_value(std::string{"result?"});
        });

        assert(f.valid()); // Valid

        auto result =
            f.get(); // Wait and get (risky; what if the promise won't set?)
        std::cout << result << std::endl;

        assert(!f.valid()); // No more valid

        // We are not allowed to get it twice
        try {
            f.get();
        } catch (std::future_error& ex) {
            // Here what we have!
            assert(ex.code() == std::future_errc::no_state);
        }

        t.join(); // Don't forget to join
    }

    // 3. Exception instead of result
    {
        auto promise1 = std::make_unique<std::promise<std::string>>();
        auto f = promise1->get_future();

        // Setup a background cleanup routine
        std::thread t([p1 = std::move(promise1)] {
            std::this_thread::sleep_for(100ms);
            try {
                // Do something bad
                std::vector<int> empty_vector;
                empty_vector.at(2);
                assert(false);

                // Or we can just explicitly create and set an exception object:
                // p1->set_exception(
                // std::make_exception_ptr(std::runtime_error{""}));

            } catch (...) {
                p1->set_exception(std::current_exception());
            }
        });

        assert(f.valid()); // Valid

        try {
            auto result = f.get();
            assert(false);
        } catch (std::exception& ex) {
            // Here what we have!
            assert(dynamic_cast<std::out_of_range*>(&ex) != nullptr);
        }

        t.join(); // Don't forget to join
    }

    // 4. Acquiring future, don't wait
    {
        std::thread t;
        auto promise1 = std::make_unique<std::promise<std::string>>();
        {
            auto f = promise1->get_future();

            // Setup a background cleanup routine
            t = std::thread{[p1 = std::move(promise1)] {
                std::this_thread::sleep_for(100ms);
                p1->set_value(std::string{"result?"});
                std::cout << "p1 is set" << std::endl;
            }};

            assert(f.valid()); // Valid

            // `f` is destructed prior to promise was set
        }

        std::cout << "f is destructed" << std::endl;

        t.join(); // Don't forget to join

        return;
    }
}

int main(void)
{
    future_promise_playground();
    return 0;
}
