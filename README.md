# Iterator-Based For

**Warning**: As this library overloads the comma operator for pre-existing types, it may break existing code in strange
ways. As such, you probably shouldn't use this library. Please see below for details. This library was written
for academic purposes only.

C++11 introduced the useful range-based `for` statement for iterating over elements in a container, array or
initializer list. However, it does not allow for iterating over a range as defined by two bounding iterators. One must
create a helper class to represent such a range that provides the `begin()` and `end()` member functions in order to
use the range-based `for` in this case. However, this leads to clunky syntax. This whimsical library attempts to
provide a more natural syntax by providing a templated comma operator overload for iterator types.

## Usage

To use the iterator-based for, just include the file `IteratorBasedFor.hpp` and use the comma operator to specify the
begin and end iterators in a range-based `for` statement like in the following code snippet:

```c++
#include <iostream>
#include <vector>
#include <IteratorBasedFor.hpp>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto start = v.begin() + 3;
    auto end = v.end() - 2;

    for (int x : start, end) {
        std::cout << x << std::endl;
    }

    return 0;
}
```

The above code snippet produces the following output:

```
4
5
6
7
8
```

## How it Works

### Overview

In order to provide natural syntax for specifying ranges with a pair of iterators, the first thing that comes to mind
is using a templated comma operator overload. The comma operator could take its operands and package them into a helper
object that defines `begin()` and `end()` member functions returning the original operands. This object could then be
used as the range expression in a range-based `for` statement. Simple! Here's the code:

```c++
template <typename Iterator>
class iterator_range {
    Iterator m_begin, m_end;

public:
    iterator_range(Iterator _begin, Iterator _end) : m_begin(_begin), m_end(_end) { }

    Iterator begin() const { return m_begin; }
    Iterator end()   const { return m_end;   }
};

template <typename Iterator>
iterator_range<Iterator> operator,(Iterator _begin, Iterator _end) {
    return iterator_range<Iterator>(_begin, _end);
}
```

But what about every other use of the comma operator? Yikes, suddenly any use of the comma operator involving user
defined types behaves differently. We should selectively overload the comma operator for iterator types only to
restrict the damage.

### What Exactly is an Iterator?

An iterator behaves like a pointer. It has an indirection operator `*` for accessing the object that it points to.
But that's not really enough to make something an iterator. To help us out, the C++ standard defines requirements for
an iterator.

N4296 § 24.2.2/2 states:

> A type `X` satisfies the `Iterator` requirements if:
> * `X` satisfies the `CopyConstructible`, `CopyAssignable` and `Destructible` requirements (17.6.3.1) and lvalues of
>   type `X` are swappable (17.6.3.2), and
> * the expressions in Table 106 are valid and have the indicated semantics.
>
> Table 106: Iterator requirements
> |Expression|Return type|Operational semantics|    pre-/post-condition       |
> |----------|-----------|---------------------|------------------------------|
> |`*r`      |`reference`|                     |pre: `r` is dereferenceable.  |
> |`++r`     |`X&`       |                     |                              |

We could therefore ensure that the templated comma operator overload works only for types `T` that are copy
constructible, copy assignable, destructible, swappable, and provide the indirection operator returning a reference and
the pre-increment operator returning a reference to the same type `T`.

We additionally have one more requirement. The standard defines the range-based `for` statement

```
for (for-range-declaration : braced-init-list) statement
```
as being equivalent to (N4296 § 6.5.4/1):
```c++
{
    auto && __range = range-init;
    for ( auto __begin = begin-expr, __end = end-expr; __begin != end; ++begin ) {
        for-range-declaration = *__begin;
        statement
    }
}
```

As you can see, in addition to the indirection operator `*` and the pre-increment operator `++`, the range-based `for`
also requires the iterators (in this example, the ones defined by `begin-expr` and `end-expr`) to define the
inequality operator `!=`. We could also ensure that our template works only for types that define this operator, but
we will avoid doing so as it would be beneficial to have compilation fail if the iterators we are trying to use with
the range-based `for` do not have an inequality operator `!=`.

### How to Identify an Iterator Using Templates

Conveniently, C++ offers the `<iterator>` library which contains `std::iterator_traits`, a type trait class that allows
us to determine such things as the category of an iterator and SFINAE would allow us to differentiate between iterators
and non-iterators. *Not* conveniently, every piece of information offered by `std::iterator_traits` is taken directly
from typedefs defined within the iterator's class. This is not so useful because it is not a requirement for iterator
classes to have such typedefs. We'll need to develop our own mechanism for determining if a type is an iterator type.
C++ does, however, offer the `<type_traits>` library containing the `std::is_copy_constructible`,
`std::is_copy_assignable`, and `std::is_destructible` type traits, which do a lot of the heavy lifting. Remaining to
be seen is a way to test if a type is swappable and whether it defines the operators we desire.

#### Testing if a Type is Swappable

The standard defines the swappable concept as follows (N4296 § 17.6.3.2):

> <small>(1)</small> This subclause provides definitions for swappable types and expressions. In these definitions, let
> `t` denote an expression of type `T`, and let `u` denote an expression of type `U`.
>
> <small>(2)</small> An object `t` is *swappable with* an object `u` if and only if:
> * <small>(2.1)</small> the expressions `swap(t, u)` and `swap(u, t)` are valid when evaluated in the context
>   described below, and
> * <small>(2.2)</small> these expressions have the following effects:
>   * <small>(2.2.1)</small> the object referred to by `t` has the value originally held by `u` and
>   * <small>(2.2.2)</small> the object referred to by `u` has the value originally held by `t`.
>
> <small>(3)</small> The context in which `swap(t, u)` and `swap(u, t)` are evaluated shall ensure that a binary
> non-member function named "swap" is selected via overload resolution (13.3) on a candidate set that includes:
> * <small>(3.1</small>) the two `swap` function templates defined in `<utility>` (20.2) and
> * <small>(3.2)</small> the lookup set produced by argument-dependent lookup (3.4.2).

(TODO: rewrite, particularly the part about needing to add using std::swap and (3))

2.1 can be tested for. We must ensure calls to `swap(t, u)` and `swap(u, t)` are valid. (3) specifies that
`std::swap` must be included in the candidate set for overload resolution of such calls, so we must simply add
`using std::swap;` to the context in which the test for 2.1 is performed. 2.2, on the other hand, cannot be
tested for statically so we will ignore this requirement. For such function calls to be valid, it must either be valid
to call `std::swap` with the same arguments, or it must be valid to call another `swap` function in the same context in
which the function call takes place (via argument-dependent lookup). We can test for both cases individually.
For a call to `std::swap` to be valid, a call to `std::swap` with the arguments under test must not cause a template
parameter substitution error, that is, there must exist an overload of `std::swap` that accepts the arguments under
test. Furthermore, `std::swap(T& a, T& b)` requires objects of type `T` to be move constructible and move assignable
(N4296 § 20.2.2/2).

SFINAE techniques can help us determine if a valid overload of `std::swap` exists. We can create a dummy function with
a similar signature to `std::swap` but with a unique type that we can examine and determine if the `decltype` of the
call to `swap` matches the type of the dummy function. This can let us know if `std::swap` would have been the selected
function to call had the dummy function not been defined. Finally, we can test if the call to `std::swap` would be
valid by using the `std::is_move_constructible` and `std::is_move_assignable` type traits to verify that the types
under test pass the constraints of `std::swap`. Using this, we can create a new type trait for determining whether
calling `std::swap` on a pair of types is valid:

```c++
template <typename T, typename U>
struct swap_call_matches_std_swap {
    //TODO: implement
};

template <typename T, typename U>
struct std_swap_call_is_valid {
    //TODO: implement
};
```

For an unqualified call to `swap` to be valid in general, we can use SFINAE again to determine if such a function
exists in the candidate set determined by argument-dependent lookup:

```c++
template <typename T, typename U>
struct swap_call_is_valid {
    //TODO: implement
};
```

Putting it all together, we construct a type trait for determining if two types are swappable:

```c++
template <typename T, typename U>
struct is_swappable {
    //TODO: implement
};
```

http://stackoverflow.com/questions/26744589/what-is-a-proper-way-to-implement-is-swappable-to-test-for-the-swappable-concept

#### Testing If an Operator is Defined for a Particular Type

As mentioned above, iterators provide at least two operations: the indirection operator `*` and the pre-increment
operator `++`. Most importantly, for an object to behave as an iterator, the indirection operator must return a
reference and the pre-increment operator should return an l-value reference to another iterator with the same type.

The existence of the operators can be easily tested using SFINAE. In addition, we can check these operators to ensure
they have the return type we expect of an iterator by using type traits: `std::is_reference` can check if the return
type of the indirection operator `*` is a reference and `std::is_same` can check if the return type of the
pre-increment `++` operator applied to an iterator of type `U` is `U&`. Conveniently, we can test for both the
existence of the operator and its return type at the same type: if template substitution fails during our tests on
the operators' return types, we know the operator doesn't exist.

Using this knowledge, we can easily create the following type traits:

```c++
template <typename T>
struct has_iterator_pre_increment {
    //TODO: implement
};

template <typename T>
struct has_iterator_indirection {
    //TODO: implement
};
```

#### The `is_iterator` Type Trait

The final step is to combine everything we developed above into one type trait, `is_iterator`. Looking again at
N4296 § 24.2.2/2, we must ensure that an iterator is `CopyConstructible`, `CopyAssignable`, and `Destructible`, that
l-values of the iterator type are swappable, and that the indirection `*` and pre-increment `++` operators are defined
for the iterator type. We now have all the iterator traits required to develop an `is_iterator` trait. All we have to
do is combine them!

```c++
template <typename T>
struct is_iterator {
    //TODO: implement
};
```

## More About the Comma Operator: Why Overloading the Comma Operator Sucks

Overloading the comma operator is often avoided. It can be useful in some cases, such as in
[`boost::assign`](http://www.boost.org/doc/libs/1_57_0/libs/assign/doc/index.html), which uses the comma operator to
enable convenient syntax for storing sets of data in containers. However, in these cases, the comma operator is usually
only overloaded for types belonging to the library that created the overload. The library can document how the comma
operator is used for these types, and all other uses of the comma operator for these types can be avoided. When the
comma operator is overloaded for existing types, on the other hand, existing code may already be depending on the
default behaviour of the comma operator and such code can fail, sometimes silently so! There are two cases that can
be problematic:

### 1. Code Depending on the Return Value of the Comma Operator May Fail To Compile, or Worse

This is usually easy to figure out. The comma operator returns the value of its second operand. If this return value is
used in existing code, such existing code will break if your overloaded comma operator doesn't have the same semantics.
If the overload returns a different type than the type expected, the code may fail to compile. Even scarier is if the
overloaded comma operator returns the same type as the type expected by the existing code (or a type convertible to
the expected type), but a different value! In this library, a new type defined by the library is returned by the
comma operator overload, so at worst, it will cause pre-existing code to fail to compile in this situation.

But to be fair to the comma operator, you can encounter this problem when you overload any operator for pre-existing
types and change its semantics. It's just that usually people don't do this with any other operator. The only use
for overloading the comma operator is if you do want to change its semantics. Why else would you? The default comma
operator already works for all types!

### 2. Code Depending on the Order of Evaluation of Operands of the Comma Operator May Cause Undefined Behaviour

This is the scarier part. The default comma operator guarantees that the operands of the comma operator are
evaluated in order, left to right. The standard states that "every value computation and side effect associated with
the left expression is sequenced before every value computation and side effect associated with the right expression"
(N4296 § 5.19/1). When the comma operator is overloaded, this guarantee goes out the window: "an invocation of an
overloaded comma operator is an ordinary function call; hence, the evaluations of its argument expressions are
unsequenced relative to one another" (N4296 § 5.19/1 footnote 87). This is because overloaded operators don't maintain
their special treatment and become regular function calls and the computations of arguments of a function call are
unsequenced with respect to each other. Finally, if side effects operating on the same scalar objects are unsequenced
with respect to each other, the behaviour is undefined (N4296 § 1.9/15).

So if existing code depends on the order of evaluation guaranteed by the default comma operator, the operands may no
longer be evaluated in the correct order after overloading the comma operator and undefined behaviour can ensue if the
operands contain side effects:

```c++
x += 5, i = x
```

Does `x` get incremented by 5 before or after it is assigned to `i`? If `x` and `i` are a built-in type like `int`,
we can say for sure that, yes, `x` does get incremented before it is assigned to `i` because operators cannot be
overloaded for built-in types. On the other hand, if either `x` or `i` is some other type, like—oh, I don't know—*an
iterator*, we can no longer be so sure. We may even find that we've come down with a case of the nasal demons
before `x` or `i` ever get assigned to.
