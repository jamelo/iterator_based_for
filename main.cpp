#include <algorithm>
#include <iostream>
#include <vector>

#include <IteratorBasedFor.hpp>

int main() {
    std::vector<int> v(10);
    std::iota(v.begin(), v.end(), 1);

    /*for (auto x : v.begin(), v.end()) {
        std::cout << x << std::endl;
    }*/

    std::vector<int>::iterator a;
    std::vector<int>::iterator b;

    swap(a, b);

    return 0;
}
