#include "handlers/SortHandler.hpp"
#include "services/SortService.hpp"

#include <boost/json.hpp>

SortHandler::SortHandler(std::shared_ptr<ISortService> service)
    : service_(std::move(service)) {
    if (!service_) {
        throw std::invalid_argument("Sort service must not be null");
    }
}

bool SortHandler::CanHandle(
    const http::request<http::string_body>& req) const {
    return req.method() == http::verb::post &&
           req.target() == "/sort";
}

http::response<http::string_body>
SortHandler::Handle(const http::request<http::string_body>& req) const {
    boost::json::error_code ec;

    const boost::json::value body =
        boost::json::parse(req.body(), ec);

    if (ec || !body.is_object()) {
        return MakeError(req, http::status::bad_request, "Invalid JSON");
    }

    const auto& obj = body.as_object();
    const auto it = obj.find("values");

    if (it == obj.end() || !it->value().is_array()) {
        return MakeError(req, http::status::bad_request, "Missing field 'values'");
    }

    const auto& arr = it->value().as_array();
    std::vector<int> values;
    values.reserve(arr.size());

    for (const auto& item : arr) {
        if (!item.is_int64()) {
            return MakeError(req, http::status::bad_request,
                            "All elements in 'values' must be integers");
        }

        const auto raw = item.as_int64();
        if (raw < std::numeric_limits<int>::min() ||
            raw > std::numeric_limits<int>::max()) {
            return MakeError(req, http::status::bad_request,
                            "Integer value is out of range");
        }

        values.push_back(static_cast<int>(raw));
    }

    const auto sorted = service_->Sort(values);

    boost::json::array result;
    result.reserve(sorted.size());

    for (const int value : sorted) {
        result.emplace_back(value);
    }
    
    boost::json::object response;
    response["sorted"] = std::move(result);

    http::response<http::string_body> res{
        http::status::ok, req.version()};

    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(response);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());

    return res;

}

http::response<http::string_body>
SortHandler::MakeError(const http::request<http::string_body>& req,
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
