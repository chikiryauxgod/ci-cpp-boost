#include "HttpServer.hpp"
#include "HttpSession.hpp"

HttpServer::HttpServer(boost::asio::io_context& ioc,
                       unsigned short port,
                       const Router& router)
    : acceptor_(ioc, {boost::asio::ip::tcp::v4(), port}),
      router_(router)
{}

void HttpServer::Run() {
    DoAccept();
}

void HttpServer::DoAccept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec,
               boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                std::make_shared<HttpSession>(
                    std::move(socket),
                    router_
                )->Run();
            }

            DoAccept();
        });
}