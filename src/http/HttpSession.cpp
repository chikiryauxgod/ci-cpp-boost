#include "http/HttpSession.hpp"

#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;

HttpSession::HttpSession(asio::ip::tcp::socket socket,
                         const Router& router,
                         std::size_t body_limit_bytes)
    : stream_(std::move(socket)),
      router_(router),
      body_limit_bytes_(body_limit_bytes)
{}

void HttpSession::Run() {
    DoRead();
}

void HttpSession::DoRead() {
    parser_.emplace();
    parser_->body_limit(body_limit_bytes_);

    http::async_read(
        stream_,
        buffer_,
        *parser_,
        beast::bind_front_handler(
            &HttpSession::OnRead,
            shared_from_this()));
}

void HttpSession::OnRead(beast::error_code ec,
                         std::size_t) {
    if (ec == http::error::end_of_stream) {
        DoClose();
        return;
    }

    if (ec) {
        res_ = MakeReadError(ec);

        http::async_write(
            stream_,
            res_,
            beast::bind_front_handler(
                &HttpSession::OnWrite,
                shared_from_this()));
        return;
    }

    auto req = parser_->release();

    try {
        res_ = router_.Route(req);
    }
    catch (const std::exception&) {
        res_ = MakeInternalError(req);
    }

    http::async_write(
        stream_,
        res_,
        beast::bind_front_handler(
            &HttpSession::OnWrite,
            shared_from_this()));
}

void HttpSession::DoClose() {
    beast::error_code ec;
    stream_.socket().shutdown(
        asio::ip::tcp::socket::shutdown_send,
        ec);
}

void HttpSession::OnWrite(
    boost::system::error_code ec,
    std::size_t) {
    if (ec)
        return;

    if (res_.need_eof()) {
        DoClose();
        return;
    }

    DoRead();
}

http::response<http::string_body>
HttpSession::MakeInternalError(
    const http::request<http::string_body>& req) const {
    boost::json::object obj;
    obj["error"] = "Internal Server Error";

    http::response<http::string_body> res{
        http::status::internal_server_error,
        req.version()
    };

    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(obj);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());

    return res;
}

http::response<http::string_body>
HttpSession::MakeReadError(beast::error_code ec) const {
    http::status status = http::status::bad_request;
    std::string_view message = "Invalid HTTP request";

    if (ec == http::error::body_limit) {
        status = http::status::payload_too_large;
        message = "Request body is too large";
    } else if (ec == http::error::partial_message) {
        message = "Incomplete HTTP request";
    } else if (ec == beast::error::timeout) {
        status = http::status::request_timeout;
        message = "Request timed out";
    }

    boost::json::object obj;
    obj["error"] = message;

    http::response<http::string_body> res{status, 11};
    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(obj);
    res.keep_alive(false);
    res.prepare_payload();

    return res;
}
