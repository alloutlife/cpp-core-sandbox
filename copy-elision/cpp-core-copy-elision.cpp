// This playground demonstrates the ability of certain compiler in pair with a chosen language standard to perform copy elision

// Copy elision
// Omits copy [and move (since C++11)] constructors, resulting in zero-copy pass-by-value semantics.
// ISO C++ permits copies to be elided in a number of cases:
//  - when a temporary object is used to initialize another object (including the object returned by a function, or the exception object created by a throw-expression)
//  - when a variable that is about to go out of scope is returned or thrown
//  - when an exception is caught by value (TODO: I wasn't lucky to observe this)
// See [url](https://en.cppreference.com/w/cpp/language/copy_elision) for details


#include <class-a.h>
#include <iomanip>

using namespace cpp_core_sandbox;

A GenerateInstanceOfA_local( void )
{
    // Non-mandatory elision of copy/move (since C++11) operations

    // In a return statement, when the operand is the name of a non-volatile object
    // with `automatic storage duration`, which isn't a function parameter or a catch clause parameter,
    // and which is of the same class type (ignoring cv-qualification) as the function return type.
    // This variant of copy elision is known as NRVO, "named return value optimization".

    // gcc elides copy, msvc 141 [cpp 17] maybe doesn't
    A local;
    return local;
}

A GenerateInstanceOfA_local_undefined( void )
{
    A local;
    A local2{ std::move( local ) };

    return local;
}

A GenerateInstanceOfA_temporary( void )
{
    // Mandatory elision of copy/move operations:
    // In a return statement, when the operand is a prvalue of the same
    // class type (ignoring cv-qualification) as the function return type
    return A{};
}

A GenerateInstanceOfA_via_function_call( void )
{
    return GenerateInstanceOfA_temporary();
}

A GenerateInstanceOfA_via_xvalue( void )
{
    A local;
    return std::move( local );              // The compiler might generate -Wpessimizing-move
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
    std::cout << "Copy elision is prevented by move operator.." << std::endl;
    auto var4 = GenerateInstanceOfA_via_xvalue();

    // As for copy elision in exceptions we have the following rules:

    // Non-mandatory elision of copy/move (since C++11) operations
    //
    // In a catch clause, when the argument is of the same type (ignoring cv-qualification) as the
    // exception object thrown, the copy of the exception object is omitted and the body of the catch
    // clause *accesses the exception object directly*, as if caught by reference (there cannot be
    // a move from the exception object because it is always an lvalue).
    //
    // This is disabled if such copy elision would change the observable behavior of the program
    // for any reason other than skipping the copy constructor and the destructor of the catch clause
    // argument (for example, if the catch clause argument is modified, and the exception object is rethrown with throw).

    try {
        try {
            try {
                std::cout << "Now we're going to throw temporary object A{} and catch it by reference. Copy elision is observed" << std::endl;
                throw GenerateInstanceOfA_via_function_call();
            }
            catch( A& by_ref ) {
                std::cout << "addr of `by_ref`: " << reinterpret_cast< uint64_t >( &by_ref ) << std::endl;
                std::cout << "rethrow; catch by reference .."  << std::endl;
                throw;
            }
        } catch( A& by_ref ) {
            std::cout << "addr of `by_ref`: " << reinterpret_cast< uint64_t >( &by_ref ) << std::endl;
            std::cout << "rethrow; catch by value .. (make a copy)"  << std::endl;
            throw;
        }
    }
    catch( A by_val ) {
        std::cout << "addr of `by_val`: " << reinterpret_cast< uint64_t >( &by_val ) << std::endl;
    }

    try {
        try {
            std::cout << "Now we're going to throw temporary object A{} (prvalue) and catch it by value. The compiler doesn't elide copy" << std::endl;
            throw GenerateInstanceOfA_via_function_call();
        }
        catch( A by_val ) {
            std::cout << "addr of `by_val`: " << reinterpret_cast< uint64_t >( &by_val ) << std::endl;

            // rethrow. the following block catches the original object by reference
            throw;
        }
    } catch( A& by_ref ) {
        std::cout << "addr of `by_ref`: " << reinterpret_cast< uint64_t >( &by_ref ) << std::endl;
    }



    try {
        try {
            std::cout << "Throw local lvalue A{}. Compiler doesn't elide copy. Instead we observe moving the lvalue" << std::endl;
            auto local = GenerateInstanceOfA_temporary();
            throw local;
        }
        catch( A& by_ref ) {
            std::cout << "addr of `by_ref`: " << reinterpret_cast< uint64_t >( &by_ref ) << std::endl;
            throw std::move( by_ref );
        }
    }
    catch( A& by_ref )
    {
        std::cout << "addr of `by_ref`: " << reinterpret_cast< uint64_t >( &by_ref ) << std::endl;
    }

    return 0;
}
