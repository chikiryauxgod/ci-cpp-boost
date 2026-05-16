#pragma once

#include "core/IHandler.hpp"

class InfoHandler final : public IHandler {
public:
    bool CanHandle(const http::request<http::string_body>& req) const override;

    http::response<http::string_body>
    Handle(const http::request<http::string_body>& req) const override;
};
