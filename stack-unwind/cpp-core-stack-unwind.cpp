// A playground to investigate the unwinding of stack after an exception was thrown


#include <class-a.h>
#include <iomanip>

int main( void )
{
    using namespace cpp_core_sandbox;

    {
        // A small deviation from the topic.
        // `const reference` bound to a temporary xvalue (expiring rvalue) object.

        // Life time is limited by the scope of definition of the reference

        const double& ref_d = 3;    // Ex.1. Looks weird but legit. Compiler creates a temporary object converted from prvalue (integer 3)
                                    // and stores it in memory (temporary materialization conversion).
                                    // Since that point `&ref_d` addresses an 8-bytes area containing the exact value.
                                    // So it is bound to a temp xvalue
        std::cout << "ref_d = " << ref_d << "; addr = " << std::hex << &ref_d << std::endl;


        // Ex.2. Referencing the object in the same way, but this time it is a class object (i.e. string).
        // "123abc" (which in fact is lvalue) is converted to a temporary object of type std::string via implicit converting constructor
        // So finally we have xvalue referenced by ref_str and ref_str2
        const std::string& ref_str = "123abc";
        const std::string& ref_str2 = ref_str;

        std::cout << "addr1 = " << &ref_str << std::endl;
        std::cout << "addr2 = " << &ref_str2 << std::endl;

        // Note: the both addr1 and addr2 have the same address

        // Having a reference to an lvalue, we see the same thing: all three variables have the same address
        std::string local_str{ "zz4455" };
        const std::string& ref_local_str = local_str;
        const std::string& ref_local_str2 = local_str;

        std::cout << "local_str = " << &local_str << std::endl;
        std::cout << "local_addr1 = " << &ref_local_str << std::endl;
        std::cout << "local_addr2 = " << &ref_local_str2 << std::endl;


        // We have the same behavior in function arguments.
        // Consider a function taking const reference of T
        auto fn1 = [] ( const std::string& ref ) {
            std::cout << "ref = " << ref << std::endl;
        };

        fn1( local_str );       // Pass the const reference to an existing lvalue
        fn1( "zzxx112" );       // Create temporary xvalue object, pass the const reference to it. Lifetime is limited by
                                // the parentheses

        // Note: non-const references to a temporary is not legit
        // double& _ref_d = 3;                     // won't compile
        // std::string& _ref_str = "123abc";       // won't compile
        // auto _fn1 = [] ( std::string& ref ) {}; // <-- the lambda is valid, the call is not
        // _fn1( "???" );
    }

    {
        // Lets see how the stack unwinding works

        // Exception in constructor
        try {
            A::SetCtorThrowCondition( 5 );

            A* ten_instances = new A[ 10 ];                     // Unmanaged memory allocation
                                                                // This approach is now considered outdated

            // Please also remember that every element is a chunk of data representing an
            // instance of A. In my gcc x64 debug run the size of each element is 0x38 bytes

            // So technically what we see. The compiler allocates a block of memory
            // which length is `sizeof( A ) * 10`
            // Then it invokes the constructor in place of each element
            // Once we get to fifth element, the exception occurs

            // The fifth one is gonna throw, so we never run to this point.

            delete[] ten_instances;
        }
        catch( std::exception& ) {

            // Set a break point at this line and make sure A::raw_string_ of the fifth element wasn't freed
            // Also make sure that first four instances were released.
            // `ten_instances` is never assigned with a value, so it's not easy to observe
            // de-allocation of memory occupied by the array
            std::cout << "exception has been caught" << std::endl;
        }

    }

    return 0;
}
