#pragma once
#include <vector>

class IPrimeService {
public:
    virtual ~IPrimeService() = default;
    virtual std::vector<int> Calculate(int n) const = 0;
};

