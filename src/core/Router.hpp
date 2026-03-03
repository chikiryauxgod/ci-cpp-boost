#pragma once
#include "IHandler.hpp"
#include <memory>
#include <vector>

class Router {
public:
    void AddHandler(std::shared_ptr<IHandler> handler);
    http::response<http::string_body>
    Route(const http::request<http::string_body>& req) const;

private:
    std::vector<std::shared_ptr<IHandler>> handlers_;
    http::response<http::string_body> MakeNotFound(const http::request<http::string_body>& req) const;
};