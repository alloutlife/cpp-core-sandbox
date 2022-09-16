#include <cassert>
#include <iomanip>
#include <iostream>
#include <optional>

// The example below demonstrates the wrapping a non-copyable movable object by
// std::optional

class Movable
{
    int n{1};

  public:
    Movable(void) = default;
    Movable(const Movable&) = delete;
    Movable(Movable&& rh) noexcept
    {
        n = rh.n + 1;
        rh.n = -1;

        std::cout << "Move constructed [" << std::hex << &rh << "] -> [" << this
                  << std::resetiosflags(std::ios_base::basefield)
                  << "] n = " << n << std::endl;
    }
    Movable& operator=(const Movable&) = delete;
    Movable& operator=(Movable&& rh) noexcept
    {
        if (this != &rh) {
            n = rh.n + 1;
            rh.n = -1;

            std::cout << "Move assigned [" << std::hex << &rh << "] -> ["
                      << this << std::resetiosflags(std::ios_base::basefield)
                      << "] n = " << n << std::endl;
        }
        return *this;
    }
    ~Movable()
    {
        if (n != -1) {
            std::cout << "~Movable(); n = " << n << std::endl;
        }
    }

    void print(void) const noexcept { std::cout << n << std::endl; }
};

void foo(std::optional<Movable> local_copy)
{
    if (local_copy.has_value()) {
        local_copy->print();
    } else {
        std::cerr << "has no value" << std::endl;
    }
}

int main(void)
{
    std::optional<Movable> a;
    a = Movable(); // Move construction

    // Stealing value from another optional when constructing std::optional
    // (move construction)
    std::optional<Movable> b{std::optional<Movable>{std::move(a.value())}};

    // Move std::optional -> std::optional (still move construction)
    std::optional<Movable> c;
    assert(c == std::nullopt); // initial nullopt
    c = std::move(b);
    assert(b != std::nullopt); // `b` now contains invalid value but is not
                               // equal to `std::nullopt` , so this is not the
                               // same as if we'd call std::optional::reset();

    {
        assert(b.has_value());
        b->print(); // prints undefined state (-1)
        // If you want it to become `nullopt`, do it explicitly
        b.reset();
        assert(b == std::nullopt);
    }

    std::optional<Movable> d;
    d = std::move(c); // still `move construction`

    Movable e; // Steal value from optional (assignment)
    e = std::move(d.value());

    foo(std::move(e));

    return 0;
}
