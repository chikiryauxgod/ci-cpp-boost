#include "core/Router.hpp"
#include "handlers/HashHandler.hpp"
#include "handlers/PrimeHandler.hpp"
#include "http/HttpServer.hpp"
#include "services/HashService.hpp"
#include "services/PrimeService.hpp"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = asio::ip::tcp;

namespace {

class TestFailure : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Expect(bool condition, std::string_view message) {
    if (!condition) {
        throw TestFailure(std::string(message));
    }
}

void ExpectStringEqual(
    std::string_view actual,
    std::string_view expected,
    std::string_view message) {
    if (actual != expected) {
        throw TestFailure(
            std::string(message) + ": expected '" + std::string(expected) +
            "', got '" + std::string(actual) + "'");
    }
}

http::request<http::string_body> MakeHashRequest(
    std::string body,
    bool keep_alive = true) {
    http::request<http::string_body> req{http::verb::post, "/hash", 11};
    req.set(http::field::content_type, "application/json");
    req.keep_alive(keep_alive);
    req.body() = std::move(body);
    req.prepare_payload();
    return req;
}

http::request<http::string_body> MakePrimeRequest(
    std::string body,
    bool keep_alive = true) {
    http::request<http::string_body> req{http::verb::post, "/primes", 11};
    req.set(http::field::content_type, "application/json");
    req.keep_alive(keep_alive);
    req.body() = std::move(body);
    req.prepare_payload();
    return req;
}

class TestServer {
public:
    TestServer()
        : workGuard_(asio::make_work_guard(ioc_)),
          hashService_(std::make_shared<HashService>()),
          primeService_(std::make_shared<PrimeService>()),
          server_(ioc_, 0, router_) {
        router_.AddHandler(std::make_shared<PrimeHandler>(primeService_));
        router_.AddHandler(std::make_shared<HashHandler>(hashService_));
        server_.Run();
        thread_ = std::thread([this] {
            ioc_.run();
        });
    }

    ~TestServer() {
        workGuard_.reset();
        ioc_.stop();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    unsigned short Port() const {
        return server_.Port();
    }

private:
    Router router_;
    asio::io_context ioc_{1};
    asio::executor_work_guard<asio::io_context::executor_type> workGuard_;
    std::shared_ptr<HashService> hashService_;
    std::shared_ptr<PrimeService> primeService_;
    HttpServer server_;
    std::thread thread_;
};

http::response<http::string_body> RoundTrip(
    tcp::socket& socket,
    beast::flat_buffer& buffer,
    http::request<http::string_body>&& req) {
    http::write(socket, req);

    http::response<http::string_body> res;
    http::read(socket, buffer, res);
    return res;
}

void TestHashServiceProducesCanonicalSha1() {
    HashService service;
    ExpectStringEqual(
        service.Hash("abc"),
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "HashService should return canonical SHA-1");
}

void TestPrimeServiceCalculatesPrimes() {
    PrimeService service;

    const std::vector<int> expected{2, 3, 5, 7};
    Expect(service.Calculate(10) == expected, "PrimeService should return primes up to 10");
    Expect(service.Calculate(1).empty(), "PrimeService should return empty result below 2");
}

void TestRouterReturns404ForUnknownRoute() {
    Router router;
    http::request<http::string_body> req{http::verb::get, "/missing", 11};
    req.keep_alive(true);

    const auto res = router.Route(req);
    Expect(res.result() == http::status::not_found, "Router should return 404 for unknown route");

    const auto body = boost::json::parse(res.body()).as_object();
    ExpectStringEqual(
        boost::json::value_to<std::string>(body.at("error")),
        "Not Found",
        "Router should serialize not found response");
}

void TestHashHandlerValidatesInput() {
    auto service = std::make_shared<HashService>();
    HashHandler handler(service);

    const auto invalidJsonResponse = handler.Handle(MakeHashRequest("{not-json}"));
    Expect(
        invalidJsonResponse.result() == http::status::bad_request,
        "HashHandler should reject invalid JSON");

    const auto missingFieldResponse = handler.Handle(MakeHashRequest(R"({"value":"abc"})"));
    Expect(
        missingFieldResponse.result() == http::status::bad_request,
        "HashHandler should reject missing input field");
}

void TestHashHandlerRejectsNullService() {
    bool thrown = false;

    try {
        HashHandler handler(nullptr);
    } catch (const std::invalid_argument&) {
        thrown = true;
    }

    Expect(thrown, "HashHandler should reject null service");
}

void TestPrimeHandlerValidatesInput() {
    auto service = std::make_shared<PrimeService>();
    PrimeHandler handler(service);

    const auto invalidJsonResponse = handler.Handle(MakePrimeRequest("{not-json}"));
    Expect(
        invalidJsonResponse.result() == http::status::bad_request,
        "PrimeHandler should reject invalid JSON");

    const auto missingFieldResponse = handler.Handle(MakePrimeRequest(R"({"value":10})"));
    Expect(
        missingFieldResponse.result() == http::status::bad_request,
        "PrimeHandler should reject missing n field");

    const auto negativeResponse = handler.Handle(MakePrimeRequest(R"({"n":-1})"));
    Expect(
        negativeResponse.result() == http::status::bad_request,
        "PrimeHandler should reject negative n");
}

void TestPrimeHandlerRejectsNullService() {
    bool thrown = false;

    try {
        PrimeHandler handler(nullptr);
    } catch (const std::invalid_argument&) {
        thrown = true;
    }

    Expect(thrown, "PrimeHandler should reject null service");
}

void TestHttpServerHandlesKeepAliveRequests() {
    TestServer server;

    asio::io_context clientIoc;
    tcp::resolver resolver(clientIoc);
    beast::tcp_stream stream(clientIoc);

    const auto endpoints =
        resolver.resolve("127.0.0.1", std::to_string(server.Port()));
    stream.connect(endpoints);

    beast::flat_buffer buffer;

    const auto first = RoundTrip(
        stream.socket(),
        buffer,
        MakeHashRequest(R"({"input":"abc"})"));
    Expect(first.result() == http::status::ok, "First keep-alive request should succeed");

    const auto firstBody = boost::json::parse(first.body()).as_object();
    ExpectStringEqual(
        boost::json::value_to<std::string>(firstBody.at("hash")),
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "First response should contain the expected hash");

    const auto second = RoundTrip(
        stream.socket(),
        buffer,
        MakeHashRequest(R"({"input":"xyz"})", false));
    Expect(second.result() == http::status::ok, "Second keep-alive request should succeed");

    const auto secondBody = boost::json::parse(second.body()).as_object();
    ExpectStringEqual(
        boost::json::value_to<std::string>(secondBody.at("hash")),
        "66b27417d37e024c46526c2f6d358a754fc552f3",
        "Second response should contain the expected hash");
}

void TestHttpServerRejectsOversizedPayload() {
    TestServer server;

    asio::io_context clientIoc;
    tcp::resolver resolver(clientIoc);
    beast::tcp_stream stream(clientIoc);

    const auto endpoints =
        resolver.resolve("127.0.0.1", std::to_string(server.Port()));
    stream.connect(endpoints);

    const std::string largeInput(17 * 1024, 'a');
    beast::flat_buffer buffer;

    const auto res = RoundTrip(
        stream.socket(),
        buffer,
        MakeHashRequest(std::string(R"({"input":")") + largeInput + "\"}", false));

    Expect(
        res.result() == http::status::payload_too_large,
        "Oversized request should return 413");
}

void TestHttpServerReturnsPrimes() {
    TestServer server;

    asio::io_context clientIoc;
    tcp::resolver resolver(clientIoc);
    beast::tcp_stream stream(clientIoc);

    const auto endpoints =
        resolver.resolve("127.0.0.1", std::to_string(server.Port()));
    stream.connect(endpoints);

    beast::flat_buffer buffer;

    const auto res = RoundTrip(
        stream.socket(),
        buffer,
        MakePrimeRequest(R"({"n":10})", false));

    Expect(res.result() == http::status::ok, "Prime endpoint should return 200");

    const auto body = boost::json::parse(res.body()).as_object();
    const auto& primes = body.at("primes").as_array();

    Expect(primes.size() == 4, "Prime endpoint should return four primes up to 10");
    Expect(primes[0].as_int64() == 2, "First prime should be 2");
    Expect(primes[1].as_int64() == 3, "Second prime should be 3");
    Expect(primes[2].as_int64() == 5, "Third prime should be 5");
    Expect(primes[3].as_int64() == 7, "Fourth prime should be 7");
}

}  // namespace

int main() {
    const std::vector<std::pair<std::string, std::function<void()>>> tests{
        {"HashService canonical SHA-1", TestHashServiceProducesCanonicalSha1},
        {"PrimeService primes", TestPrimeServiceCalculatesPrimes},
        {"Router 404", TestRouterReturns404ForUnknownRoute},
        {"HashHandler validation", TestHashHandlerValidatesInput},
        {"HashHandler null service", TestHashHandlerRejectsNullService},
        {"PrimeHandler validation", TestPrimeHandlerValidatesInput},
        {"PrimeHandler null service", TestPrimeHandlerRejectsNullService},
        {"HttpServer keep-alive", TestHttpServerHandlesKeepAliveRequests},
        {"HttpServer oversized payload", TestHttpServerRejectsOversizedPayload},
        {"HttpServer primes endpoint", TestHttpServerReturnsPrimes},
    };

    for (const auto& [name, test] : tests) {
        try {
            test();
            std::cout << "[PASS] " << name << '\n';
        } catch (const std::exception& ex) {
            std::cerr << "[FAIL] " << name << ": " << ex.what() << '\n';
            return 1;
        }
    }

    return 0;
}
