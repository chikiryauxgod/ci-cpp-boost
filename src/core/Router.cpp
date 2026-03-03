#include "Router.hpp"
#include <boost/json.hpp>

void Router::AddHandler(std::shared_ptr<IHandler> handler) {
    handlers_.push_back(std::move(handler));
}

http::response<http::string_body> Router::Route(const http::request<http::string_body>& req) const {
    for (const auto& handler : handlers_) {
        if (handler->CanHandle(req)) {
            return handler->Handle(req);
        }
    }

    return MakeNotFound(req);
}

http::response<http::string_body>
Router::MakeNotFound(
    const http::request<http::string_body>& req) const {
    boost::json::object obj;
    obj["error"] = "Not Found";

    http::response<http::string_body> res{
        http::status::not_found,
        req.version()
    };

    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(obj);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());

    return res;
}