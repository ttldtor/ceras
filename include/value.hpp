#ifndef VALUE_HPP_INCLUDED_DS9P8IU4LKJASDOIPUY498YAFKASHFAS9F8Y4OKHDAFSIUOHASDFFS
#define VALUE_HPP_INCLUDED_DS9P8IU4LKJASDOIPUY498YAFKASHFAS9F8Y4OKHDAFSIUOHASDFFS

#include "./includes.hpp"
#include "./tensor.hpp"
#include "./utils/id.hpp"
#include "./utils/better_assert.hpp"
#include "./utils/enable_shared.hpp"

namespace ceras
{

    template< typename T > requires std::floating_point<T>
    struct value : enable_id< value<T>, "Value" >
    {
        typedef T value_type;
        typedef tensor<value_type> tensor_type;
        value_type data_;

        value() = delete;
        value( value_type v ) noexcept : enable_id<value<T>, "Value">{}, data_{ v } {}
        value( value const& ) noexcept = default;
        value( value && ) noexcept = default;
        value& operator =( value const& ) noexcept = default;
        value& operator =( value && ) noexcept = default;

        void backward( auto ) noexcept { }

        template< Tensor Tsor >
        Tsor const forward( Tsor const& refer ) const
        {
            Tsor ans = ones_like( refer ); // cast it to a tensor
            ans *= data_;
            return ans;
        }

        std::vector<unsigned long> shape() const noexcept
        {
            return std::vector<unsigned long>{ {-1UL,} };
        }

    };//struct value

    template< typename T >
    struct is_value : std::false_type {};

    template< typename T >
    struct is_value< value< T > > : std::true_type {};

    template< class T >
    inline constexpr bool is_value_v = is_value<T>::value;

    template< typename T >
    concept Value = is_value_v<T>;


    // for tensor_type deduction in a binary operator
    template< typename L, typename R >
    struct tensor_deduction
    {
        using op_type = std::conditional<is_value_v<L>, R, L>::type;
        using tensor_type = std::remove_cv_t<decltype(std::declval<op_type>().forward())>;
    };


}//namespace ceras

#endif//VALUE_HPP_INCLUDED_DS9P8IU4LKJASDOIPUY498YAFKASHFAS9F8Y4OKHDAFSIUOHASDFFS

