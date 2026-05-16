#pragma once

#include "app/ServiceRegistry.hpp"

class IServiceFactory {
public:
    virtual ~IServiceFactory() = default;

    virtual ServiceRegistry CreateServices() const = 0;
};
