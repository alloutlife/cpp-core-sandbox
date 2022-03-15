#pragma once

#include <iostream>
#include <cstdint>
#include <memory>

namespace cpp_core_sandbox {

// Copyable, movable, traces every step
// First of all we need to inspect the lifetime of instances, so copy-move logic is not fairly implemented.
class A final {
public:
    explicit A( void )
    {
        std::cout << __func__ << "() ctor [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;
        _init();
        if( throw_in_ctor_for_seq_no_ == seq_no_ ) {
            throw std::exception{};
        }
    }

    explicit A( int val )
    {
        std::cout << __func__ << "( " << val << " ) ctor [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;
        _init();
        if( throw_in_ctor_for_seq_no_ == val ) {
            throw std::exception{};
        }
    }

    A(const A& rh)
    {
        std::cout << __func__ << "( const A& ) ctor [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;
        // Don't copy seq_no

        // Just init the payload, it's not necessary to perform a deep copy until every instance has the same payload values
        _init();
    }

    A(A&& rh)
    {
        std::cout << __func__ << "( A&& ) ctor [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;

        /*seq_no_ = rh.seq_no_;
        rh.seq_no_ = -1;               // Isn't necessary, but let's invalidate the payload for the old instance
        */

        // Sequence number remains the same in order to observe the lifetime of class instances

        _init();
    }

    A& operator=(const A& rh)
    {
        std::cout << "operator=( const A& ) [ " << seq_no_ << " ]" << std::endl;

        // Sequence number remains the same in order to observe the lifetime of class instances

        return *this;
    }

    A& operator=(A&& rh)
    {
        std::cout << "operator=( A&& ) [ " << seq_no_ << " ]" << std::endl;

        // Sequence number remains the same in order to observe the lifetime of class instances

        return *this;
    }

    // Notes:
    // - by default destructors are noexcept
    // - I don't care about the destructor is not being virtual because the class is non-derivable
    ~A( void ) noexcept( false )
    {
        std::cout << __func__ << "() [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;

        // The only manual deinitialization step
        if( raw_string_ ) {
            delete[] raw_string_;
            raw_string_ = nullptr;              // Actually we don't need this assignment
        }
    }


    static void SetCtorThrowCondition( int seq_no = -2 ) noexcept
    {
        throw_in_ctor_for_seq_no_ = seq_no;
    }



private:

    // No matter how an object was constructed, `unique_seq_no_` always has unique sequentially incremented value
    static int unique_seq_no_;
    int seq_no_{ unique_seq_no_++ };

    static int throw_in_ctor_for_seq_no_;


private:

    // Some payload
    char* raw_string_{ nullptr };
    std::unique_ptr< std::string > pstr_;
    std::string regular_string_;

    // A function to initialize the payload
    void _init( void );
};

}
