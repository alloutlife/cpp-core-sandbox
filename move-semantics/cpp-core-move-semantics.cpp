// Observing move semantics

#include <class-a.h>
#include <iomanip>
#include <type_traits>
#include <cassert>


// Remember the approximate scale of value types:
//
// (i) means has identity
// (m) means is movable
//
// glvalue - i
// lvalue  - i & !m
// xvalue  - i & m
// prvalue - !i & m
// rvalue  - m
//
// (note that !i & !m makes no sense, so we don't have such a value type)


int main( void )
{
    std::string inst1{ "123zxc" };

    // The 11th standard of c++ introduces a reference to rvalue type
    // Which is used to implement move semantics.

    // A reference to rvalue is witten as follows..
    // Although we define `ref_rval` as a reference to rvalue, the compiler will avoid calling && variant of overloaded functions
    // because this rvalue reference object has identifier.
    // Normally you see such a definition in function arguments, not on local stack

    std::string&& ref_rval = std::move( inst1 );                        // the entry looks weird
    const std::string&& ref_rval_const = std::move( inst1 );            // The code is valid, although doesn't make a sense
                                                                        // because of constness preventing moving action

    assert( not std::is_rvalue_reference< decltype( inst1 ) >::value );
    assert( std::is_rvalue_reference< decltype( ref_rval ) >::value );


    std::string inst2{ inst1 };         // calls string( const string& )
    std::string inst3{ ref_rval };      // still calls string( const string& )
                                        // From the perspective of compiler `ref_rval` still lives after call,
                                        // So it doesn't call && constructor

    struct move_or_copy {
        static void fn1( const std::string& ) {
            std::cout << "lvalue ref" << std::endl;
        }
        static void fn1( std::string&& ) {
            std::cout << "rvalue ref" << std::endl;
        }
    };

    move_or_copy::fn1( inst1 );
    move_or_copy::fn1( ref_rval );      // The same result. Doesn't call && variant.
                                        // In this context the variable has name, so it's treated as an lvalue

    // If we want to call the && variant, we need to cast a non-const lvalue
    // to an unnamed rvalue reference (i.e. xvalue).
    // In other words we need to tell the compiler that this is expiring rvalue.
    // `std::move` helps to convert non-const lvalue to xvalue
    move_or_copy::fn1( std::move( ref_rval ) );                     // calls &&-variant

    // We might have a prvalue object. It's already prepared to be moved
    move_or_copy::fn1( std::string{ "temporary object" } );         // calls &&-variant


    // Step aside. An isolated simple example
    {
        std::string s1{ "1" }, s2{ "2" };

        std::string s3 = s1;            // calls string( const string& )
        std::string s4 = s1 + s2;       // s1 + s2 results in creating a prvalue. We are not likely to observe a && construction here
                                        // most likely the compiler will elide copy.

        std::string s5{ std::move( s4 ) };      // std::move returns a reference to rvalue that isn't bound to a name

        std::cout << "s4: " << s4 << std::endl;
        std::cout << "s5: " << s5 << std::endl;
    }


    // TODO: how to pass an initialization value via function call: const ref, &&, pass by value? Pros, cons


    return 0;
}
