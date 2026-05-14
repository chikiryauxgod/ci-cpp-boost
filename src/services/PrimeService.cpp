#include "services/PrimeService.hpp"

#include <cstddef>

std::vector<int> PrimeService::Calculate(int n) const {
    if (n < 2) {
        return {};
    }

    const auto limit = static_cast<std::size_t>(n);

    std::vector<bool> sieve(limit + 1, true);
    sieve[0] = sieve[1] = false;

    for (std::size_t i = 2; i * i <= limit; ++i) {
        if (sieve[i]) {
            for (std::size_t j = i * i; j <= limit; j += i) {
                sieve[j] = false;
            }
        }
    }

    std::vector<int> result;
    result.reserve(limit / 2);

    for (std::size_t i = 2; i <= limit; ++i) {
        if (sieve[i]) {
            result.push_back(static_cast<int>(i));
        }
    }

    return result;
}
