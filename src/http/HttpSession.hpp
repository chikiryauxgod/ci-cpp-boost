#pragma once
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "../core/Router.hpp"

#include <optional>

class HttpSession
    : public std::enable_shared_from_this<HttpSession> {

public:
    HttpSession(boost::asio::ip::tcp::socket socket,
                const Router& router,
                std::size_t body_limit_bytes);

    void Run();

private:
    void DoRead();
    void DoClose();
    void OnRead(boost::beast::error_code ec,
                std::size_t bytes);
    void OnWrite(boost::beast::error_code ec,
                std::size_t bytes);
    
    http::response<http::string_body> res_;
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    std::optional<http::request_parser<http::string_body>> parser_;
    const Router& router_;
    std::size_t body_limit_bytes_;

    http::response<http::string_body> MakeInternalError(
        const http::request<http::string_body>& req) const;
    http::response<http::string_body> MakeReadError(
        boost::beast::error_code ec) const;
};
