#pragma once

#include "core/IHandler.hpp"
#include "services/IHashService.hpp"

#include <memory>
#include <stdexcept>
#include <string_view>

class HashHandler final : public IHandler {
public:
    explicit HashHandler(std::shared_ptr<IHashService> service);

    ~HashHandler() override = default;

    bool CanHandle(const http::request<http::string_body>& req) const override;

    http::response<http::string_body>
    Handle(const http::request<http::string_body>& req) const override;

private:
    http::response<http::string_body>
    MakeError(const http::request<http::string_body>& req,
              http::status status,
              std::string_view message) const;

    std::shared_ptr<IHashService> service_;
};
