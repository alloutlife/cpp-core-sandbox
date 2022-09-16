#include <algorithm>
#include <assert.h>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

//    E x a m p l e   # 1
//
// Compile time check to determine if the first class is open derived from the
// second class (note: private derivation doesn't work here and breaks the code)

// Check that two pointers are convertible from Dptr to Bptr
template <class Bptr, class Dptr> struct _is_convertible_to_base_ptr {
    // Technical types `yes` and `no`. They have different size
    typedef char yes;
    typedef struct {
        char _hide_[2];
    } no;

    static auto check(Bptr) -> yes; // Note that no one method is implemented,
                                    // all we need is just their signatures
    static auto check(...) -> no;   // Ellipsis has the least priority

    static auto get_derived()
        -> Dptr; // Note. Instead of defining this meta-function, we can use
                 // std::declval<Dptr>()

    enum {
        value =
            sizeof(yes) ==
                sizeof(check(
                    get_derived())) // Check that check( Bptr ) is instantiated
            && !std::is_same<Bptr, void*>::value
    };

    // The same could be done via constexpr
    static constexpr bool value_alt =
        sizeof(yes) ==
            sizeof(check(
                get_derived())) // Check that check( Bptr ) is instantiated
        && !std::is_same<Bptr, void*>::value;
};

// Wrapper struct
template <class Base, class Derived> struct is_convertible_to_base {
    enum { value = _is_convertible_to_base_ptr<Base*, Derived*>::value_alt };
};

struct Base1 {
    virtual void _do() { return; }
};
struct Derived1 : public Base1 {
    void _do() override
    {
        std::cout << "Hi, mom!" << std::endl;
        return;
    }
};
struct C {
};
struct Derived2 : Derived1 {
};

void _do_sfinae_example1()
{
    // Some positive asserts
    static_assert(is_convertible_to_base<Base1, Derived1>::value == true,
                  "test #1 failed");
    static_assert(is_convertible_to_base<Base1, Derived2>::value == true,
                  "test #2 failed");
    static_assert(is_convertible_to_base<Derived1, Derived2>::value == true,
                  "test #3 failed");
    static_assert(
        is_convertible_to_base<std::basic_string<char>, std::string>::value ==
            true,
        "test #4 failed");

    // And negative asserts
    static_assert(is_convertible_to_base<Base1, C>::value == false,
                  "test #5 failed");
    static_assert(is_convertible_to_base<Derived1, Base1>::value == false,
                  "test #6 failed");
    static_assert(is_convertible_to_base<Base1, std::string>::value == false,
                  "test #7 failed");
    static_assert(is_convertible_to_base<std::string, void>::value == false,
                  "test #8 failed");

    static_assert(std::is_base_of<Base1, Derived1>::value == true,
                  "test #8 failed");

    static_assert(std::true_type::value == true, "test #8 failed");
}

//    E x a m p l e   # 2
//
// Check if the class has method with certain signature

// Primary template with a static assertion
// for a meaningful error message
// if it ever gets instantiated.
// We could leave it undefined if we didn't care.

template <typename, typename T, typename... Args> struct has_serialize {
    static_assert(std::integral_constant<T, false>::value,
                  "Second template parameter needs to be of function type.");
};

// Specialization that does the checking
template <typename C, typename Ret, typename... Args>
struct has_serialize<C, Ret(Args...)> {
  private:
    template <typename T>
    static constexpr auto check(T*) -> typename std::is_same<
        decltype(std::declval<T>().serialize(std::declval<Args>()...)),
        Ret      // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        >::type; // attempt to call it and see if the return type is correct

    template <typename> static constexpr std::false_type check(...);

    typedef decltype(check<C>(nullptr)) type;

  public:
    static constexpr bool value = type::value;
};

void _do_sfinae_example2()
{
    struct X {
        int serialize(const std::string&) { return 42; }
    };
    struct Y : X {
    };

    static_assert(has_serialize<Y, int(const std::string&)>::value,
                  "test #1 failed");
    // static_assert( has_serialize< char, bool >::value, "test #1 failed" ); //
    // won't compile
}

void do_sfinae_main(void)
{
    _do_sfinae_example1();
    _do_sfinae_example2();
}
