#include "HttpServer.hpp"
#include "HttpSession.hpp"

HttpServer::HttpServer(boost::asio::io_context& ioc,
                       unsigned short port,
                       const Router& router,
                       std::size_t body_limit_bytes)
    : acceptor_(ioc, {boost::asio::ip::tcp::v4(), port}),
      router_(router),
      body_limit_bytes_(body_limit_bytes)
{}

void HttpServer::Run() {
    DoAccept();
}

unsigned short HttpServer::Port() const {
    return acceptor_.local_endpoint().port();
}

void HttpServer::DoAccept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec,
               boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                std::make_shared<HttpSession>(
                    std::move(socket),
                    router_,
                    body_limit_bytes_
                )->Run();
            }

            DoAccept();
        });
}
