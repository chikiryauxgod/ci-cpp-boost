#include "services/SortService.hpp"
#include <algorithm>

std::vector<int> SortService::Sort(std::vector<int> data) const {
    std::sort(data.begin(), data.end());
    return data;
}
