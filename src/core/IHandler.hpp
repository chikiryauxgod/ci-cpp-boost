#pragma once
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class IHandler {
public:
    virtual ~IHandler() = default;

    virtual bool CanHandle(
        const http::request<http::string_body>& req) const = 0;

    virtual http::response<http::string_body> Handle(
        const http::request<http::string_body>& req) const = 0;
};