// This playground demonstrates the ability of certain compiler in pair with a chosen language standard to perform copy elision

// Copy elision
// Omits copy [and move (since C++11)] constructors, resulting in zero-copy pass-by-value semantics.
// ISO C++ permits copies to be elided in a number of cases:
//  - when a temporary object is used to initialize another object (including the object returned by a function, or the exception object created by a throw-expression)
//  - when a variable that is about to go out of scope is returned or thrown
//  - when an exception is caught by value


#include <class-a.h>

using namespace cpp_core_sandbox;

A GenerateInstanceOfA_local( void )
{
    A local;
    return local;    
}

A GenerateInstanceOfA_temporary( void )
{
    return A{};
}

A GenerateInstanceOfA_via_function_call( void )
{
    return GenerateInstanceOfA_temporary();
}

A GenerateInstanceOfA_via_xvalue( void )
{
    A local;
    return std::move( local );
}

int main(void)
{
    // Copy elision is expected
    {
        std::cout << "Copy elision is expected.." << std::endl;
        auto var1 = GenerateInstanceOfA_local();

        std::cout << "Copy elision is expected.." << std::endl;
        auto var2 = GenerateInstanceOfA_temporary();

        std::cout << "Copy elision is expected.." << std::endl;
        auto var3 = GenerateInstanceOfA_via_function_call();
    }

    // ensure that std::move doesn't let to elide
    std::cout << "Copy elision is not expected.." << std::endl;
    auto var4 = GenerateInstanceOfA_via_xvalue();

    try {
        try {
            std::cout << "Now we're going to throw temporary object A{} and catch it by reference. Copy elision is observed" << std::endl;
            throw GenerateInstanceOfA_via_function_call();
        }
        catch( A& by_ref ) {
            std::cout << "addr of `by_ref`: " << reinterpret_cast< uint64_t >( &by_ref ) << std::endl;
            throw;
        }
    }
    catch( A by_val ) {
        std::cout << "addr of `by_val`: " << reinterpret_cast< uint64_t >( &by_val ) << std::endl;
    }

    try {
        std::cout << "Now we're going to throw temporary object A{} and catch it by value. No copy elision" << std::endl;
        throw GenerateInstanceOfA_via_function_call();
    }
    catch( A by_val ) {
        std::cout << "addr of `by_val`: " << reinterpret_cast< uint64_t >( &by_val ) << std::endl;
    }


    try {

        try {
            try {
                std::cout << "Throw local lvalue A{}. Compiler doesn't elide copy" << std::endl;
                auto local = GenerateInstanceOfA_temporary();
                throw local;
            }
            catch( A& by_ref ) {
                std::cout << "addr of `by_ref`: " << reinterpret_cast< uint64_t >( &by_ref ) << std::endl; 
                throw std::move( by_ref );
            }
        }
        catch( A& by_ref_moved ) {
            std::cout << "addr of `by_ref_moved`: " << reinterpret_cast< uint64_t >( &by_ref_moved ) << std::endl; 
            throw by_ref_moved;
        }

    } catch( A by_value ) {
        std::cout << "addr of `by_value`: " << reinterpret_cast< uint64_t >( &by_value ) << std::endl; 
    }

    return 0;
}
