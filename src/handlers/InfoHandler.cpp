#include "handlers/InfoHandler.hpp"

#include <boost/json.hpp>

bool InfoHandler::CanHandle(
    const http::request<http::string_body>& req) const {
    return req.method() == http::verb::get &&
           req.target() == "/info";
}

http::response<http::string_body>
InfoHandler::Handle(const http::request<http::string_body>& req) const {
    boost::json::object response;
    boost::json::array endpoints;

    response["service"] = "boost-http-server";
    response["version"] = "1.0.0";

    endpoints.emplace_back("/health");
    endpoints.emplace_back("/hash");
    endpoints.emplace_back("/primes");
    endpoints.emplace_back("/info");

    response["endpoints"] = std::move(endpoints);


    http::response<http::string_body> res{
        http::status::ok, req.version()};

    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(response);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());

    return res;
}
