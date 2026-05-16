#pragma once

#include "app/ServiceRegistry.hpp"
#include "core/IHandler.hpp"

#include <memory>
#include <vector>

class IHandlerFactory {
public:
    virtual ~IHandlerFactory() = default;

    virtual std::vector<std::shared_ptr<IHandler>> CreateHandlers(
        const ServiceRegistry& services) const = 0;
};
