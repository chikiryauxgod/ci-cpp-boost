#pragma once

#include "app/ServiceRegistry.hpp"
#include "factories/IHandlerFactory.hpp"

class DefaultHandlerFactory final : public IHandlerFactory {
public:
    std::vector<std::shared_ptr<IHandler>> CreateHandlers(
        const ServiceRegistry& services) const override;
};
