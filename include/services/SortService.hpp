#pragma once

#include "services/ISortService.hpp"

class SortService final : public ISortService {
public:
    std::vector<int> Sort(std::vector<int> data) const override;
};

