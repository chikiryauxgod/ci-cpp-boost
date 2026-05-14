#pragma once

#include <cstddef>
#include <cstdint>

struct AppConfig {
    unsigned short port = 8080;
    std::size_t body_limit_bytes = 16 * 1024;
};

[[nodiscard]] AppConfig LoadAppConfig();
