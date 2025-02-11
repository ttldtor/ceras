#ifndef STRIDE_ITERATOR_HPP_INCLUDED_SDOJASDLKJASDFKL489I0UJASLFIJKASLFKJSAAAA
#define STRIDE_ITERATOR_HPP_INCLUDED_SDOJASDLKJASDFKL489I0UJASLFIJKASLFKJSAAAA

#include "../includes.hpp"

namespace ceras
{

    template < typename Iterator_Type >
    struct stride_iterator
    {
        typedef stride_iterator                                                     self_type;
        typedef typename std::iterator_traits< Iterator_Type >::value_type          value_type;
        typedef typename std::iterator_traits< Iterator_Type >::reference           reference;
        typedef typename std::iterator_traits< Iterator_Type >::difference_type     difference_type;
        typedef typename std::iterator_traits< Iterator_Type >::pointer             pointer;
        typedef typename std::iterator_traits< Iterator_Type >::iterator_category   iterator_category;

        Iterator_Type                                                               iterator_;
        difference_type                                                             step_;

        constexpr stride_iterator( const Iterator_Type& it, std::integral auto const dt ) noexcept : iterator_( it ) , step_( static_cast<difference_type>(dt) ) { }

        constexpr stride_iterator() noexcept : iterator_( 0 ) , step_( 1 ) { }
        constexpr stride_iterator( const self_type& ) noexcept = default;
        constexpr stride_iterator( self_type&& ) noexcept = default;
        constexpr self_type& operator=( const self_type& ) noexcept = default;
        constexpr self_type& operator=( self_type&& ) noexcept = default;

        self_type& operator++() noexcept
        {
            std::advance( iterator_, step_ );
            return *this;
        }
        const self_type operator++( int ) noexcept
        {
            self_type ans( *this );
            operator++();
            return ans;
        }
        self_type& operator+=( const difference_type dt ) noexcept
        {
            std::advance( iterator_, step_*dt );
            return *this;
        }
        friend const self_type operator+( const self_type& lhs, const difference_type rhs ) noexcept
        {
            self_type ans( lhs );
            ans += rhs;
            return ans;
        }
        friend const self_type operator+( const difference_type lhs, const self_type& rhs ) noexcept
        {
            return rhs + lhs;
        }
        self_type& operator--() noexcept
        {
            std::advance( iterator_, -step_ );
            return *this;
        }
        const self_type operator--( int ) noexcept
        {
            self_type ans( *this );
            operator--();
            return ans;
        }
        self_type& operator-=( const difference_type dt ) noexcept
        {
            std::advance( iterator_, -dt * step_ );
            return *this;
        }
        friend const self_type operator-( const self_type& lhs, const difference_type rhs ) noexcept
        {
            self_type ans( lhs );
            ans -= rhs;
            return ans;
        }
        reference operator[]( const difference_type dt ) noexcept
        {
            return iterator_[dt * step_];
        }
        const reference operator[]( const difference_type dt ) const noexcept
        {
            return iterator_[dt * step_];
        }
        reference operator*() noexcept
        {
            return *iterator_;
        }
        const reference operator*() const noexcept
        {
            return *iterator_;
        }

        auto operator<=>(const self_type&) const = default;

        friend difference_type operator-( const self_type& lhs, const self_type& rhs ) noexcept
        {
            better_assert( lhs.step_ == rhs.step_ && "stride iterators of different steps" );
            return ( lhs.iterator_ - rhs.iterator_ ) / lhs.step_;
        }
    }; //struct stride_iterator

}//namespace ceras

#endif

