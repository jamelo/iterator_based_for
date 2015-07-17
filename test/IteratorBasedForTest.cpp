#include <gtest/gtest.h>
#include <IteratorBasedFor.hpp>

#include <algorithm>
#include <vector>

using namespace IteratorBasedForImpl;

TEST(IteratorBasedForTest, SwapCallMatchesStdSwap)
{
    EXPECT_TRUE((swap_call_matches_std_swap<int, int>::value));
    EXPECT_TRUE((swap_call_matches_std_swap<std::vector<int>::iterator, std::vector<int>::iterator>::value));
    EXPECT_TRUE((swap_call_matches_std_swap<std::vector<int>::iterator&, std::vector<int>::iterator&>::value));


}

TEST(IteratorBasedForTest, StdSwapCallIsValid)
{
    EXPECT_TRUE((std_swap_call_is_valid<int, int>::value));
    EXPECT_TRUE((std_swap_call_is_valid<std::vector<int>::iterator, std::vector<int>::iterator>::value));
    EXPECT_TRUE((std_swap_call_is_valid<std::vector<int>::iterator&, std::vector<int>::iterator&>::value));
}

class A { };
int swap(A, A);

TEST(IteratorBasedForTest, SwapCallIsValid)
{
    EXPECT_TRUE((swap_call_is_valid<A, A>::value));
    EXPECT_TRUE((swap_call_is_valid<int, int>::value));
    EXPECT_TRUE((swap_call_is_valid<int&, int&>::value));
    EXPECT_TRUE((swap_call_is_valid<std::vector<int>::iterator, std::vector<int>::iterator>::value));
    EXPECT_TRUE((swap_call_is_valid<std::vector<int>::iterator&, std::vector<int>::iterator&>::value));
}

TEST(IteratorBasedForTest, IsSwappable)
{
    EXPECT_TRUE((is_swappable<int, int>::value));
    EXPECT_TRUE((is_swappable<std::vector<int>::iterator, std::vector<int>::iterator>::value));
    EXPECT_TRUE((is_swappable<std::vector<int>::iterator&, std::vector<int>::iterator&>::value));
}

TEST(IteratorBasedForTest, HasIteratorPreIncrement)
{
    EXPECT_TRUE(has_iterator_pre_increment<std::vector<int>::iterator>::value);
    EXPECT_TRUE(has_iterator_pre_increment<std::vector<int>::const_iterator>::value);

    EXPECT_FALSE(has_iterator_pre_increment<int>::value);
    EXPECT_FALSE(has_iterator_pre_increment<std::vector<int>>::value);
    EXPECT_FALSE(has_iterator_pre_increment<std::string>::value);
}

TEST(IteratorBasedForTest, HasIteratorIndirection)
{
    EXPECT_TRUE(has_iterator_indirection<std::vector<int>::iterator>::value);
    EXPECT_TRUE(has_iterator_indirection<std::vector<int>::const_iterator>::value);

    EXPECT_FALSE(has_iterator_indirection<int>::value);
    EXPECT_FALSE(has_iterator_indirection<std::string>::value);
    EXPECT_FALSE(has_iterator_indirection<std::vector<int>>::value);
}

TEST(IteratorBasedForTest, IsIterator)
{
    EXPECT_TRUE(is_iterator<std::vector<int>::iterator>::value);
    EXPECT_TRUE(is_iterator<std::vector<int>::const_iterator>::value);
    EXPECT_TRUE(is_iterator<std::vector<int>::reverse_iterator>::value);
    EXPECT_TRUE(is_iterator<std::vector<int>::const_reverse_iterator>::value);

    EXPECT_FALSE(is_iterator<std::vector<int>::iterator&>::value);
    EXPECT_FALSE(is_iterator<std::vector<int>::iterator&&>::value);
    EXPECT_FALSE(is_iterator<std::vector<int>::const_iterator&>::value);
    EXPECT_FALSE(is_iterator<std::vector<int>::const_iterator&&>::value);
    EXPECT_FALSE(is_iterator<std::vector<int>::reverse_iterator&>::value);
    EXPECT_FALSE(is_iterator<std::vector<int>::reverse_iterator&&>::value);
    EXPECT_FALSE(is_iterator<std::vector<int>::const_reverse_iterator&>::value);
    EXPECT_FALSE(is_iterator<std::vector<int>::const_reverse_iterator&&>::value);

    EXPECT_FALSE(is_iterator<int>::value);
    EXPECT_FALSE(is_iterator<int*>::value);
    EXPECT_FALSE(is_iterator<int&>::value);
    EXPECT_FALSE(is_iterator<int&&>::value);
    EXPECT_FALSE(is_iterator<std::vector<int>::iterator*>::value);
}

/*TEST(IteratorBasedForTest, CommaIteratorTypes)
{
    std::string s("Test");
    std::vector<int> v({1, 2, 3, 4});

    EXPECT_EQ((s.begin(), s.end()), make_iterator_range(s.begin(), s.end()));
    EXPECT_EQ((v.begin(), v.end()), make_iterator_range(v.begin(), v.end()));
}

TEST(IteratorBasedForTest, CommaNonIteratorTypes)
{
    std::string a("Test1");
    std::string b("Test2");

    EXPECT_EQ((1,2), 2);
    EXPECT_EQ((a, b), b);
    EXPECT_EQ((&a[0], &a[4]), &a[4]);
}

TEST(IteratorBasedForTest, ForVectorIterator)
{
    std::vector<int> v;
    std::iota(v.begin(), v.end(), 0);

    int n = 0;

    for (int x : v.begin(), v.end()) {
        EXPECT_EQ(x, n);
        n++;
    }

    EXPECT_EQ(n, v.size());
}*/
