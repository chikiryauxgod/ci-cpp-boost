#pragma once

#include "services/IHashService.hpp"
#include "services/IPrimeService.hpp"
#include "services/ISortService.hpp"

#include <memory>

struct ServiceRegistry {
    std::shared_ptr<IHashService> hash_service;
    std::shared_ptr<IPrimeService> prime_service;
    std::shared_ptr<ISortService> sort_service;
};
