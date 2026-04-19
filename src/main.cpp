#include "http/HttpServer.hpp"
#include "services/PrimeService.hpp"
#include "services/HashService.hpp"
#include "handlers/PrimeHandler.hpp"
#include "handlers/HashHandler.hpp"
#include <algorithm>
#include <csignal>
#include <thread>
#include <vector>

int main() {
    const unsigned int worker_count =
        std::max(1U, std::thread::hardware_concurrency());

    boost::asio::io_context ioc{static_cast<int>(worker_count)};
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const boost::system::error_code&, int) {
        ioc.stop();
    });

    auto hash_service = std::make_shared<HashService>();
    auto prime_service = std::make_shared<PrimeService>();

    Router router;
    router.AddHandler(
        std::make_shared<PrimeHandler>(prime_service));
    router.AddHandler(
        std::make_shared<HashHandler>(hash_service));
    
    HttpServer server(ioc, 8080, router);
    server.Run();

    std::vector<std::thread> threads;
    threads.reserve(worker_count > 0 ? worker_count - 1 : 0);

    for (unsigned int i = 1; i < worker_count; ++i) {
        threads.emplace_back([&ioc] {
            ioc.run();
        });
    }

    ioc.run();

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
