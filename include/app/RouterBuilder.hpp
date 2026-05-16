#pragma once

#include "core/IHandler.hpp"
#include "core/Router.hpp"

#include <memory>
#include <vector>

[[nodiscard]] Router BuildRouter(
    const std::vector<std::shared_ptr<IHandler>>& handlers);
