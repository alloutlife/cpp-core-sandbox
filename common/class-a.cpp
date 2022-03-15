#include "class-a.h"
#include <cstring>

namespace cpp_core_sandbox {

int A::unique_seq_no_{ 1 };
int A::throw_in_ctor_for_seq_no_{ -2 };         // note: `-1` is for moved instance with its undefined state

void A::_init( void )
{
    if( raw_string_ ) {
        delete[] raw_string_;
    }

    // I need this to be a low-level, non-modern approach
    const int array_size = 5;
    raw_string_ = new char[ array_size ];
    std::strncpy( raw_string_, "1234", array_size );

    pstr_ = std::make_unique< std::string >( "unique_pointer" );
    regular_string_ = "some value123";

    // This method is going to be used as an initializer in A::ctor

    // Note that every time the constructor throws, we have a memory leak in `raw_string_`
}

}
