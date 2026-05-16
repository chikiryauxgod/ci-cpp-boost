#pragma once

#include "app/ServiceRegistry.hpp"
#include "core/AppConfig.hpp"
#include "core/Router.hpp"
#include "factories/DefaultHandlerFactory.hpp"
#include "factories/DefaultServiceFactory.hpp"
#include "http/HttpServer.hpp"

#include <boost/asio.hpp>

#include <thread>
#include <vector>

class App {
public:
    explicit App(AppConfig config = LoadAppConfig());

    int Run();

private:
    void ConfigureSignals();
    void StartServer();
    void RunWorkers();
    void JoinWorkers();

    AppConfig config_;
    unsigned int worker_count_;
    boost::asio::io_context ioc_;
    boost::asio::signal_set signals_;
    DefaultServiceFactory service_factory_;
    DefaultHandlerFactory handler_factory_;
    ServiceRegistry services_;
    Router router_;
    HttpServer server_;
    std::vector<std::thread> threads_;
};
