#pragma once
#include <string>

class IHashService {
public:
    virtual ~IHashService() = default;
    virtual std::string Hash(const std::string& input) const = 0;
};
