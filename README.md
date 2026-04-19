# C++ Boost HTTP Server

Small HTTP API server written in C++20 with Boost.Asio and Boost.Beast.

## Features

- `POST /hash` returns the SHA-1 hash of the input string
- `POST /primes` returns all prime numbers up to `n`
- JSON request/response handling with Boost.JSON
- Configurable server port and request body limit via environment variables
- CMake build, CTest-based tests, Docker and Docker Compose support

## Endpoints

### `POST /hash`

Request:

```json
{"input":"abc"}
```

Response:

```json
{"hash":"a9993e364706816aba3e25717850c26c9cd0d89d"}
```

### `POST /primes`

Request:

```json
{"n":10}
```

Response:

```json
{"primes":[2,3,5,7]}
```

## Configuration

The server supports these environment variables:

- `APP_PORT` - server port, default: `8080`
- `APP_BODY_LIMIT_BYTES` - maximum request body size in bytes, default: `16384`

Example:

```bash
APP_PORT=8081 APP_BODY_LIMIT_BYTES=4096 ./build/server
```

## Build

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run Locally

```bash
./build/server
```

Run with custom port:

```bash
APP_PORT=8081 ./build/server
```

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

You can also run the test binary directly:

```bash
./build/server_tests
```

## Docker

Build the image:

```bash
docker build -t ci-cpp-boost-app .
```

Run the container:

```bash
docker run --rm -p 8080:8080 ci-cpp-boost-app
```

If port `8080` is already in use:

```bash
docker run --rm -p 8081:8080 ci-cpp-boost-app
```

## Docker Compose

```bash
docker compose up --build
```

Stop:

```bash
docker compose down
```

## Quick API Check

```bash
curl -X POST http://localhost:8080/hash \
  -H "Content-Type: application/json" \
  -d '{"input":"abc"}'
```

```bash
curl -X POST http://localhost:8080/primes \
  -H "Content-Type: application/json" \
  -d '{"n":10}'
```
