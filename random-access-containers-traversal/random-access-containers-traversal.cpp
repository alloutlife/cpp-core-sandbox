#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <algorithm>

int main( void )
{
    // We have two random access containers in STL: `std::vector` and `std::deque`.
    // The following code demonstrates that it doesn't matter which way you traverse a `std::array`.
    // But on the contrary it is preferred to use iterators or `range-based for` in case of `std::deque` traversal
    //
    // I also found that the code compiled with GCC 11.2 performs slightly better than compiled with Clang 13.0
    constexpr size_t array_size{ 1'000'000'000 };

    // Uncomment one of:
    using container_type = std::vector< int >;
    // using container_type = std::deque< int >;
    // using container_type = std::string;
    // using container_type = std::wstring;

    container_type s1;
    int generator{ 0 };
    std::generate_n( std::back_inserter( s1 ), array_size, [&generator]{ return (++generator) % 256; } );

    size_t count{ 0 };
    auto fn_perform_traverse_var1 = [&s1, &count] {

        // Iterator-based array traversal
        const auto it_end = s1.cend();
        for( auto it = s1.cbegin(); it != it_end; ++it ) {

            // Payload. Count elements of the array equal to some number
            if( *it == 126 ) {
                ++count;
            }
        }
    };

    auto fn_perform_traverse_var2 = [&s1, &count] {

        // Index-based array traversal
        const auto size = s1.size();
        for( decltype( s1 )::size_type k = 0; k < size; ++k ) {
            if( s1.at( k ) == 126 ) {
                ++count;
            }
        }
    };

    auto fn_perform_traverse_var3 = [&s1, &count] {

        // Range-based for. Copy of the value
        for( auto val : s1 ) {
            if( val == 126 ) {
                ++count;
            }
        }
    };

    auto fn_perform_traverse_var4 = [&s1, &count] {

        // Range-based for. Reference to the value
        // Might perform slightly better on deque
        for( const auto& val : s1 ) {
            if( val == 126 ) {
                ++count;
            }
        }
    };


    auto tp_start1 = std::chrono::high_resolution_clock::now();
    fn_perform_traverse_var1();
    auto tp_end1 = std::chrono::high_resolution_clock::now();

    auto tp_start2 = std::chrono::high_resolution_clock::now();
    fn_perform_traverse_var2();
    auto tp_end2 = std::chrono::high_resolution_clock::now();

    auto tp_start3 = std::chrono::high_resolution_clock::now();
    fn_perform_traverse_var3();
    auto tp_end3 = std::chrono::high_resolution_clock::now();

    auto tp_start4 = std::chrono::high_resolution_clock::now();
    fn_perform_traverse_var4();
    auto tp_end4 = std::chrono::high_resolution_clock::now();



    std::cout << count << ";" << std::endl
              << "time consumed #1: " << std::chrono::duration_cast< std::chrono::milliseconds >( tp_end1 - tp_start1 ).count() << std::endl
              << "time consumed #2: " << std::chrono::duration_cast< std::chrono::milliseconds >( tp_end2 - tp_start2 ).count() << std::endl
              << "time consumed #3: " << std::chrono::duration_cast< std::chrono::milliseconds >( tp_end3 - tp_start3 ).count() << std::endl
              << "time consumed #4: " << std::chrono::duration_cast< std::chrono::milliseconds >( tp_end4 - tp_start4 ).count() << std::endl
    ;

    return 0;
}
