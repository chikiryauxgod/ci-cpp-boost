#pragma once

#include "core/IHandler.hpp"
#include "services/IPrimeService.hpp"

#include <memory>
#include <stdexcept>
#include <string_view>

class PrimeHandler final : public IHandler {
public:
    explicit PrimeHandler(std::shared_ptr<IPrimeService> service);

    ~PrimeHandler() override = default;

    bool CanHandle(const http::request<http::string_body>& req) const override;

    http::response<http::string_body>
    Handle(const http::request<http::string_body>& req) const override;

private:
    http::response<http::string_body>
    MakeError(const http::request<http::string_body>& req,
              http::status status,
              std::string_view message) const;

    std::shared_ptr<IPrimeService> service_;
};
