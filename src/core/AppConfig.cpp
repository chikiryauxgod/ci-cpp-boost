#include "AppConfig.hpp"

#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

std::uint64_t ParseUnsigned(std::string_view name, const char* value) {
    try {
        const std::string text(value);
        std::size_t pos = 0;
        const auto parsed = std::stoull(text, &pos, 10);
        if (pos != text.size()) {
            throw std::invalid_argument("trailing characters");
        }
        return parsed;
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid value for " + std::string(name));
    }
}

}  // namespace

AppConfig LoadAppConfig() {
    AppConfig config;

    if (const char* port = std::getenv("APP_PORT")) {
        const auto parsed = ParseUnsigned("APP_PORT", port);
        if (parsed == 0 ||
            parsed > std::numeric_limits<unsigned short>::max()) {
            throw std::runtime_error("APP_PORT is out of range");
        }
        config.port = static_cast<unsigned short>(parsed);
    }

    if (const char* limit = std::getenv("APP_BODY_LIMIT_BYTES")) {
        const auto parsed = ParseUnsigned("APP_BODY_LIMIT_BYTES", limit);
        if (parsed == 0 ||
            parsed > std::numeric_limits<std::size_t>::max()) {
            throw std::runtime_error("APP_BODY_LIMIT_BYTES is out of range");
        }
        config.body_limit_bytes = static_cast<std::size_t>(parsed);
    }

    return config;
}
