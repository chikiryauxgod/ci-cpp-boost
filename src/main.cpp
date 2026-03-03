#include "http/HttpServer.hpp"
#include "services/HashService.hpp"
#include "handlers/HashHandler.hpp"

int main() {
    boost::asio::io_context ioc{std::thread::hardware_concurrency()};

    auto hashService = std::make_shared<HashService>();

    Router router;
    router.AddHandler(
        std::make_shared<HashHandler>(hashService));

    HttpServer server(ioc, 8080, router);
    server.Run();

    std::vector<std::thread> threads;
    for (unsigned i = 0; i < std::thread::hardware_concurrency(); ++i)
        threads.emplace_back([&ioc]{ ioc.run(); });

    for (auto& t : threads)
        t.join();
}