#include "handlers/PrimeHandler.hpp"

#include <boost/json.hpp>

#include <cstdint>
#include <limits>

PrimeHandler::PrimeHandler(std::shared_ptr<IPrimeService> service)
    : service_(std::move(service)) {
    if (!service_) {
        throw std::invalid_argument("Prime service must not be null");
    }
}

bool PrimeHandler::CanHandle(
    const http::request<http::string_body>& req) const {
    return req.method() == http::verb::post &&
           req.target() == "/primes";
}

http::response<http::string_body>
PrimeHandler::Handle(const http::request<http::string_body>& req) const {
    boost::json::error_code ec;
    const auto body = boost::json::parse(req.body(), ec);

    if (ec || !body.is_object()) {
        return MakeError(req, http::status::bad_request, "Invalid JSON");
    }

    const auto& obj = body.as_object();
    const auto it = obj.find("n");

    if (it == obj.end() || !it->value().is_int64()) {
        return MakeError(req, http::status::bad_request,
                         "Missing field 'n'");
    }

    const std::int64_t raw = it->value().as_int64();
    if (raw < 0 ||
        raw > static_cast<std::int64_t>(std::numeric_limits<int>::max())) {
        return MakeError(req, http::status::bad_request,
                         "Field 'n' is out of range");
    }

    const auto primes = service_->Calculate(static_cast<int>(raw));

    boost::json::array values;
    values.reserve(primes.size());
    for (const int prime : primes) {
        values.emplace_back(prime);
    }

    boost::json::object response;
    response["primes"] = std::move(values);

    http::response<http::string_body> res{
        http::status::ok, req.version()};

    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(response);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());

    return res;
}

http::response<http::string_body>
PrimeHandler::MakeError(const http::request<http::string_body>& req,
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
