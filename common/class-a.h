#pragma once

#include <iostream>
#include <cstdint>

namespace cpp_core_sandbox {

// Copyable, movable, traces every step
class A final {
public:
    explicit A( void )
    {
        std::cout << __func__ << "() ctor [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;
        if( throw_in_ctor_for_seq_no_ == seq_no_ ) {
            throw std::exception{};
        }
    }

    explicit A( int payload )
    {
        std::cout << __func__ << "() ctor [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;
        if( throw_in_ctor_for_seq_no_ == seq_no_ ) {
            throw std::exception{};
        }
    }

    A(const A& rh)
    {
        std::cout << __func__ << "( const A& ) ctor [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;
        seq_no_ = rh.seq_no_;
    }

    A(A&& rh)
    {
        std::cout << __func__ << "( A&& ) ctor [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;
        seq_no_ = rh.seq_no_;
        rh.seq_no_ = -1;               // Isn't necessary, but let's invalidate the payload for the old instance
    }

    A& operator=(const A& rh)
    {
        std::cout << "operator=( const A& ) [ " << seq_no_ << " ]" << std::endl;
        seq_no_ = rh.seq_no_;
        return *this;
    }

    A& operator=(A&& rh)
    {
        std::cout << "operator=( A&& ) [ " << seq_no_ << " ]" << std::endl;
        seq_no_ = rh.seq_no_;
        rh.seq_no_ = -1;               // Isn't necessary, but let's invalidate the payload for the old instance
        return *this;
    }

    // Notes:
    // - by default destructors are noexcept
    // - I don't care about the destructor being virtual because the class is non-derivable
    ~A( void ) noexcept( false )
    {
        std::cout << __func__ << "() [ " << seq_no_ << " ]" << "; this = " << reinterpret_cast< uint64_t >( this ) << std::endl;
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
};

}
