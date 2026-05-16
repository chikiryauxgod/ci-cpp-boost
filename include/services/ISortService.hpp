#pragma once

#include <vector>

class ISortService {
public:
    virtual ~ISortService() = default;
    virtual std::vector<int> Sort(std::vector<int> data) const = 0;
};

