#include "HttpSession.hpp"

#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;

HttpSession::HttpSession(asio::ip::tcp::socket socket,
                         const Router& router)
    : stream_(std::move(socket)),
      router_(router)
{}

void HttpSession::Run() {
    DoRead();
}

void HttpSession::DoRead() {
    parser_.body_limit(16 * 1024); // 16 KB лимит

    http::async_read(
        stream_,
        buffer_,
        parser_,
        beast::bind_front_handler(
            &HttpSession::OnRead,
            shared_from_this()));
}

void HttpSession::OnRead(beast::error_code ec,
                         std::size_t) {
    if (ec == http::error::end_of_stream) {
        beast::error_code ignored;
        stream_.socket().shutdown(
            asio::ip::tcp::socket::shutdown_send,
            ignored);
        return;
    }

    auto req = parser_.release();

    if (ec) {
        return;
    }
    try {
        res_ = router_.Route(req);
    }
    catch(const std::exception& e) {
        res_ = MakeInternalError(req, e.what());
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
    const http::request<http::string_body>& req,
    std::string_view message) {
    boost::json::object obj;
    obj["error"] = message;

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