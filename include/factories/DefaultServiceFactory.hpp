#pragma once

#include "factories/IServiceFactory.hpp"

class DefaultServiceFactory final : public IServiceFactory {
public:
    ServiceRegistry CreateServices() const override;
};
