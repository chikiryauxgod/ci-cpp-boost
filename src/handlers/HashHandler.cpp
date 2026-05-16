#include "handlers/HashHandler.hpp"

#include <boost/json.hpp>

HashHandler::HashHandler(std::shared_ptr<IHashService> service)
    : service_(std::move(service)) {
    if (!service_) {
        throw std::invalid_argument("Hash service must not be null");
    }
}

bool HashHandler::CanHandle(
    const http::request<http::string_body>& req) const {
    return req.method() == http::verb::post &&
           req.target() == "/hash";
}

http::response<http::string_body>
HashHandler::Handle(const http::request<http::string_body>& req) const {
    boost::json::error_code ec;
    const auto body = boost::json::parse(req.body(), ec);

    if (ec || !body.is_object()) {
        return MakeError(req, http::status::bad_request, "Invalid JSON");
    }

    const auto& obj = body.as_object();
    const auto it = obj.find("input");

    if (it == obj.end() || !it->value().is_string()) {
        return MakeError(req, http::status::bad_request,
                         "Missing field 'input'");
    }

    const std::string input =
        boost::json::value_to<std::string>(it->value());

    const auto result = service_->Hash(input);

    boost::json::object response;
    response["hash"] = result;

    http::response<http::string_body> res{
        http::status::ok, req.version()};

    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(response);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());

    return res;
}

http::response<http::string_body>
HashHandler::MakeError(const http::request<http::string_body>& req,
                       http::status status,
                       std::string_view message) const {
    boost::json::object obj;
    obj["error"] = message;

    http::response<http::string_body> res{status, req.version()};
    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(obj);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());

    return res;
}
