#pragma once

#include "core/IHandler.hpp"
#include "services/ISortService.hpp"

#include <memory>
#include <string_view>

class SortHandler final : public IHandler {
public:
    explicit SortHandler(std::shared_ptr<ISortService> service);

    bool CanHandle(const http::request<http::string_body>& req) const override;

    http::response<http::string_body>
    Handle(const http::request<http::string_body>& req) const override;

private:
    http::response<http::string_body>
    MakeError(const http::request<http::string_body>& req,
              http::status status,
              std::string_view message) const;

    std::shared_ptr<ISortService> service_;
};
