#pragma once

#include "../core/IHandler.hpp"

#include <boost/json.hpp>

class HealthHandler final : public IHandler {
public:
    bool CanHandle(const http::request<http::string_body>& req) const override {
        return req.method() == http::verb::get &&
               req.target() == "/health";
    }

    http::response<http::string_body>
    Handle(const http::request<http::string_body>& req) const override {
        boost::json::object response;
        response["status"] = "ok";

        http::response<http::string_body> res{
            http::status::ok, req.version()};

        res.set(http::field::content_type, "application/json");
        res.body() = boost::json::serialize(response);
        res.prepare_payload();
        res.keep_alive(req.keep_alive());

        return res;
    }
};
