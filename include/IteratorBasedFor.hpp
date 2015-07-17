#ifndef IteratorBasedFor_H
#define IteratorBasedFor_H

#include <type_traits>

namespace IteratorBasedForImpl
{

    struct tag {};

    template<typename T>
    tag swap(T&, T&);

    template <typename T, typename U>
    struct swap_call_matches_std_swap_impl {
        template <typename V, typename W>
        static constexpr auto check(int) -> typename std::is_same<
            decltype(swap(std::declval<V&>(), std::declval<W&>())),
            tag>::type;

        template <typename, typename>
        static constexpr std::false_type check(...);

        using type = decltype(check<T, U>(0));
    };

    template <typename T, typename U>
    struct swap_call_matches_std_swap : swap_call_matches_std_swap_impl<T, U>::type { };

    template <typename T, typename U>
    struct std_swap_call_is_valid : std::integral_constant<bool,
            swap_call_matches_std_swap<T, U>::value
            && std::is_move_assignable<T>::value
            && std::is_move_assignable<U>::value
            && std::is_move_constructible<T>::value
            && std::is_move_constructible<U>::value> {
    };

    template <typename T, typename U>
    struct swap_call_is_valid_impl {
        //Uses comma operator to return std::true_type() if template substitution succeeds.
        //Comma operator will always return std::true_type(), but template substitution may fail if left operand is
        //malformed.
        template <typename V, typename W>
        static constexpr auto check(int) -> decltype(swap(std::declval<V&>(), std::declval<W&>()), std::true_type());

        template <typename, typename>
        static constexpr std::false_type check(...);

        using type = decltype(check<T, U>(0));
    };

    template <typename T, typename U>
    struct swap_call_is_valid : swap_call_is_valid_impl<T, U>::type { };

    template <typename T, typename U>
    struct is_swappable : std::integral_constant<bool,
            swap_call_matches_std_swap<T, U>::value
            && std_swap_call_is_valid<T, U>::value
            || swap_call_is_valid<T, U>::value> {
    };

    /**
     * @brief Checks whether T defines a pre-increment operator that returns a reference to T. Provides the member
     *        constant 'value' which is equal to true if T passes the check. Otherwise 'value' is equal to false.
     */
    template <typename T>
    struct has_iterator_pre_increment_impl {
        template <typename U>
        static constexpr auto check(int) -> typename std::is_same<decltype(++std::declval<U>()), U&>::type;

        template <typename>
        static constexpr std::false_type check(...);

        typedef decltype(check<T>(0)) type;
    };

    template <typename T>
    struct has_iterator_pre_increment : has_iterator_pre_increment_impl<T>::type { };

    /**
     * @brief Checks whether T defines an indirection operator. Provides the member constant 'value' which is equal
     *        to true if T passes the check. Otherwise 'value' is equal to false.
     */
    template <typename T>
    struct has_iterator_indirection_impl {
        template <typename U>
        static constexpr auto check(int) -> typename std::is_reference<decltype(*std::declval<U>())>::type;

        template <typename>
        static constexpr std::false_type check(...);

        typedef decltype(check<T>(0)) type;
    };

    template <typename T>
    struct has_iterator_indirection : has_iterator_indirection_impl<T>::type {};

    template <typename T>
    struct is_iterator : std::integral_constant<bool,
            !std::is_reference<T>::value
            && std::is_copy_constructible<T>::value
            && std::is_copy_assignable<T>::value
            && std::is_destructible<T>::value
            && is_swappable<T, T>::value
            && has_iterator_pre_increment<T>::value
            && has_iterator_indirection<T>::value> {
    };

    /**
     * @brief Represents a pseudo-range involving a begin iterator and end iterator. Provides the methods begin() and
     *        end() returning the respective iterators.
     */
    template <typename Iterator>
    class iterator_range {
    private:
        Iterator m_begin;
        Iterator m_end;

    public:
        iterator_range(Iterator&& _begin, Iterator&& _end) :
                m_begin(_begin),
                m_end(_end) {
        }

        Iterator begin() const { return m_begin; }
        Iterator end()   const { return m_end;   }

        bool operator==(const iterator_range<Iterator>& rhs) const {
            return m_begin == rhs.m_begin && m_end == rhs.m_end;
        }
    };

    /**
     * @brief Helper function for constructing an iterator_range object.
     */
    template <typename Iterator>
    iterator_range<Iterator> make_iterator_range(Iterator&& _begin, Iterator&& _end) {
        return iterator_range<typename std::remove_reference<Iterator>::type>(_begin, _end);
    }

} // namespace IteratorBasedForImpl


/**
 * @brief Templated comma operator overload for all types that appear to be an iterator. A type appears to be an
 *        iterator if it defines a pre-increment operator and an indirection operator.
 *
 * @return Returns an iterator_range object that facilitates iterating over elements in the range represented by the
 *         _begin and _end iterators with a range-based-for construct.
 */
template <typename Iterator,
typename std::enable_if<IteratorBasedForImpl::is_iterator<typename std::remove_reference<Iterator>::type>::value, int>::type = 0>
IteratorBasedForImpl::iterator_range<Iterator> operator,(Iterator&& _begin, Iterator&& _end)
{
    return IteratorBasedForImpl::make_iterator_range(_begin, _end);
}

#endif // IteratorBasedFor_H
