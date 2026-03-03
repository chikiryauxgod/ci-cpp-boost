#pragma once
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "../core/Router.hpp"

class HttpSession
    : public std::enable_shared_from_this<HttpSession> {

public:
    HttpSession(boost::asio::ip::tcp::socket socket,
                const Router& router);

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
    boost::beast::http::request<boost::beast::http::string_body> req_;
    const Router& router_;

    http::response<http::string_body> MakeInternalError(const http::request<http::string_body>& req,
                  std::string_view message);
};