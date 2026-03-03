#pragma once
#include "IHashService.hpp"

class HashService final : public IHashService {
public:
    std::string Hash(const std::string& input) const override;
};

