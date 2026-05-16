#include "app/RouterBuilder.hpp"

Router BuildRouter(const std::vector<std::shared_ptr<IHandler>>& handlers) {
    Router router;

    for (const auto& handler : handlers) {
        router.AddHandler(handler);
    }

    return router;
}
