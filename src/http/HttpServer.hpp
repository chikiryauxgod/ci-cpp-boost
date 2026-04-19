#pragma once
#include <boost/asio.hpp>
#include "../core/Router.hpp"

class HttpServer {
public:
    HttpServer(boost::asio::io_context& ioc,
               unsigned short port,
               const Router& router,
               std::size_t body_limit_bytes);

    void Run();
    [[nodiscard]] unsigned short Port() const;

private:
    void DoAccept();

    boost::asio::ip::tcp::acceptor acceptor_;
    const Router& router_;
    std::size_t body_limit_bytes_;
};
