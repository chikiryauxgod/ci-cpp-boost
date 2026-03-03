#include "Router.hpp"

void Router::AddHandler(std::shared_ptr<IHandler> handler) {
    handlers_.push_back(std::move(handler));
}

http::response<http::string_body>
Router::Route(const http::request<http::string_body>& req) const {

    for (const auto& h : handlers_) {
        if (h->CanHandle(req)) {
            return h->Handle(req);
        }
    }

    http::response<http::string_body> res{http::status::not_found, req.version()};
    res.set(http::field::content_type, "application/json");
    res.body() = R"({"error":"not found"})";
    res.prepare_payload();
    return res;
}