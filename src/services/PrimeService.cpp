#include "PrimeService.hpp"

std::vector<int> PrimeService::Calculate(int n) const {
    if (n < 2) {
        return {};
    }

    std::vector<bool> sieve(n + 1, true);
    sieve[0] = sieve[1] = false;

    for (int i = 2; i * i <= n; ++i) {
        if (sieve[i]) {
            for (int j = i * i; j <= n; j += i) {
                sieve[j] = false;
            }
        }
    }

    std::vector<int> result;
    for (int i = 2; i <= n; ++i) {
        if (sieve[i]) {
            result.push_back(i);
        }
    }

    return result;
}