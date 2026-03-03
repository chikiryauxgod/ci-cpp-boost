#pragma once
#include <boost/asio.hpp>
#include "../core/Router.hpp"

class HttpServer {
public:
    HttpServer(boost::asio::io_context& ioc,
               unsigned short port,
               const Router& router);

    void Run();

private:
    void DoAccept();

    boost::asio::ip::tcp::acceptor acceptor_;
    const Router& router_;
};