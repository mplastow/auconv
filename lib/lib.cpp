// TODO(MATT): This library isn't used in auconv and so this file should be removed!

// lib.cpp
// A demo library containing only factorial()

#include <lib.hpp>

namespace lib {

int factorial(int n)
{
    if (n == 0) {
        return 1;
    }

    return n * factorial(n - 1);
}

} // namespace lib