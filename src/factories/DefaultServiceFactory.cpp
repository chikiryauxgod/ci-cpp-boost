#include "factories/DefaultServiceFactory.hpp"

#include "services/HashService.hpp"
#include "services/PrimeService.hpp"
#include "services/SortService.hpp"

#include <memory>

ServiceRegistry DefaultServiceFactory::CreateServices() const {
    ServiceRegistry services;
    services.hash_service = std::make_shared<HashService>();
    services.prime_service = std::make_shared<PrimeService>();
    services.sort_service = std::make_shared<SortService>();

    return services;
}
