#pragma once

#include "services/IPrimeService.hpp"

class PrimeService final : public IPrimeService {
public:
    std::vector<int> Calculate(int n) const override;
};
