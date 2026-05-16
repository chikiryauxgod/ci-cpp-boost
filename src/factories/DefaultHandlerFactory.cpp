#include "factories/DefaultHandlerFactory.hpp"

#include "handlers/HashHandler.hpp"
#include "handlers/HealthHandler.hpp"
#include "handlers/InfoHandler.hpp"
#include "handlers/PrimeHandler.hpp"
#include "handlers/SortHandler.hpp"

#include <memory>
#include <vector>

std::vector<std::shared_ptr<IHandler>>
DefaultHandlerFactory::CreateHandlers(const ServiceRegistry& services) const {
    std::vector<std::shared_ptr<IHandler>> handlers;
    handlers.reserve(5);

    handlers.push_back(std::make_shared<HealthHandler>());
    handlers.push_back(std::make_shared<InfoHandler>());
    handlers.push_back(std::make_shared<PrimeHandler>(services.prime_service));
    handlers.push_back(std::make_shared<HashHandler>(services.hash_service));
    handlers.push_back(std::make_shared<SortHandler>(services.sort_service));

    return handlers;
}
