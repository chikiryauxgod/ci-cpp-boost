#pragma once
#include "IPrimeService.hpp"

class PrimeService final : public IPrimeService {
public:
    std::vector<int> Calculate(int n) const override;
};