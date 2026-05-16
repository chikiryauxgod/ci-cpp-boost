#include "app/App.hpp"

#include "app/RouterBuilder.hpp"

#include <algorithm>
#include <csignal>

App::App(AppConfig config)
    : config_(std::move(config)),
      worker_count_(std::max(1U, std::thread::hardware_concurrency())),
      ioc_(static_cast<int>(worker_count_)),
      signals_(ioc_, SIGINT, SIGTERM),
      services_(service_factory_.CreateServices()),
      router_(BuildRouter(handler_factory_.CreateHandlers(services_))),
      server_(ioc_, config_.port, router_, config_.body_limit_bytes)
{}

int App::Run() {
    ConfigureSignals();
    StartServer();
    RunWorkers();
    JoinWorkers();

    return 0;
}

void App::ConfigureSignals() {
    signals_.async_wait([this](const boost::system::error_code&, int) {
        ioc_.stop();
    });
}

void App::StartServer() {
    server_.Run();
}

void App::RunWorkers() {
    threads_.reserve(worker_count_ > 0 ? worker_count_ - 1 : 0);

    for (unsigned int i = 1; i < worker_count_; ++i) {
        threads_.emplace_back([this] {
            ioc_.run();
        });
    }

    ioc_.run();
}

void App::JoinWorkers() {
    for (auto& thread : threads_) {
        thread.join();
    }
}
