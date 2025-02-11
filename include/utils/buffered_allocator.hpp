#ifndef BUFFERED_ALLOCATOR_HPP_INCLUDED_DPSIOJASLKJ3489UASLIKJASOI8UJ3498UAFDSJA
#define BUFFERED_ALLOCATOR_HPP_INCLUDED_DPSIOJASLKJ3489UASLIKJASOI8UJ3498UAFDSJA

#include "../includes.hpp"
#include "../config.hpp"
#include "../backend/cuda.hpp"

namespace ceras
{

    //
    // Warning: this allocator is only designed for tensor
    //
    template< typename T, std::size_t BYTES > requires (not std::same_as<T, void>)
    struct buffered_allocator
    {
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        constexpr buffered_allocator() noexcept = default;
        constexpr buffered_allocator( const buffered_allocator<T, BYTES>& ) noexcept = default;
        constexpr buffered_allocator( buffered_allocator<T, BYTES>&& ) noexcept = default;

        template< class U >
        constexpr buffered_allocator( const buffered_allocator<U, BYTES>& ) noexcept {}

        constexpr buffered_allocator& operator = ( buffered_allocator<T, BYTES> const& ) noexcept = default;
        constexpr buffered_allocator& operator = ( buffered_allocator<T, BYTES>&& ) noexcept = default;

        constexpr ~buffered_allocator() {}

        [[nodiscard]] constexpr T* allocate( std::size_t const n )
        {
            const std::size_t bytes = sizeof(T) * n;
            if ( bytes <= BYTES ) // use stack in case of small memory
                return reinterpret_cast<T*>( cache_.data() );

            if constexpr( cuda_mode ) // cuda host allocation to accelerate the cudaMemcpy
            {
                return allocate_host<T>( n );
            }
            else // default heap allocation
            {
                std::allocator<T> a;
                return a.allocate( bytes );
            }
        }

        constexpr void deallocate( T* p, std::size_t const n )
        {
            const std::size_t bytes = sizeof(T) * n;

            if ( bytes <= BYTES )
                return;

            if constexpr( cuda_mode )
            {
                deallocate_host( p );
            }
            else
            {
                std::allocator<T> a;
                a.deallocate( p, n );
            }
        }

        std::array<std::byte, BYTES> cache_;

        //althought this has been removed in std::allocator from C++20 on, some STL's allocator_trait still relies on this embeded class
        template< class U > struct rebind { typedef buffered_allocator<U, BYTES> other; };
    };

    template< class T1, class T2, std::size_t N >
    constexpr bool operator==( const buffered_allocator<T1, N>& lhs, const buffered_allocator<T2, N>& rhs ) noexcept
    {
        return lhs.cache_ == rhs.cache_;
    }

}//ceras

#endif//BUFFERED_ALLOCATOR_HPP_INCLUDED_DPSIOJASLKJ3489UASLIKJASOI8UJ3498UAFDSJA

