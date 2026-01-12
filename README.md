# Multithreaded Proxy Server

A high-performance multithreaded HTTP proxy server written in C++ with LRU caching capabilities. This server handles multiple client connections concurrently and caches responses to improve latency and reduce bandwidth usage.

## Features

- **Multithreading**: Uses a thread pool to handle multiple client requests concurrently.
- **LRU Caching**: Implements a Least Recently Used (LRU) cache to store and retrieve frequently accessed responses.
- **HTTP/HTTPS Support**: Handles standard HTTP requests and supports HTTPS tunneling via the HTTP CONNECT method.
- **Configurable**: Easy configuration of port, thread pool size, and cache size.

## File Structure

```
.
├── include/            # Header files
│   ├── cache.hpp       # LRU Cache declaration
│   ├── httpparser.hpp  # HTTP Request parser
│   ├── httprequest.hpp # HTTP Request structure
│   ├── proxyServer.hpp # Main server class
│   ├── Socket.hpp      # Socket wrapper
│   └── threadpool.hpp  # Thread pool implementation
├── src/                # Source files
│   ├── cache.cpp       # LRU Cache definition
│   ├── httpparser.cpp  # Parsing logic
│   ├── main.cpp        # Entry point
│   ├── proxyServer.cpp # Server logic
│   ├── Socket.cpp      # Socket implementation
│   └── threadpool.cpp  # Thread pool logic
├── tests/              # Test scripts
│   └── test_proxy.py   # Python functional test
├── Makefile            # Build configuration
└── README.md           # Project documentation
```

## Prerequisites

- **C++ Compiler**: A compiler supporting C++17 or later (e.g., `g++`).
- **Make**: Build automation tool.
- **Python 3** (Optional): For running the test script.
- **Python `requests` library** (Optional): Required for the test script (`pip install requests`).

## Build Instructions

Use the provided `Makefile` to compile the project.

```bash
make
```

To clean build artifacts:
```bash
make clean
```

## Usage

Run the proxy server with default settings (Port: 8080, Threads: 10, Cache: 10MB):
```bash
./proxy_server
```

### Custom Configuration

You can pass command-line arguments to configure the server:

```bash
./proxy_server <PORT> <THREAD_COUNT> <CACHE_SIZE_BYTES>
```

**Example:**
Run on port 8081 with 20 threads and 20MB cache:
```bash
./proxy_server 8081 20 20971520
```

## Testing

A Python script is provided to verify the proxy server's functionality, including GET requests, caching, and HTTPS tunneling.

1. Start the proxy server in one terminal:
   ```bash
   ./proxy_server 8081
   ```

2. Run the test script in another terminal (ensure port matches the script, default is 8081):
   ```bash
   python3 tests/test_proxy.py
   ```

   **Note:** You might need to adjust the `PROXY_PORT` variable in `tests/test_proxy.py` if you run the server on a different port.

## License

This project is open-source.