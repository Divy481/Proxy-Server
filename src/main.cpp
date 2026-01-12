#include "../include/proxyServer.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  int port = 8080;
  int threads = 10;
  size_t cacheSize = 1024 * 1024 * 10; // 10 MB

  if (argc > 1)
    port = std::stoi(argv[1]);
  if (argc > 2)
    threads = std::stoi(argv[2]);
  if (argc > 3)
    cacheSize = std::stoul(argv[3]);

  std::cout << "Starting Proxy Server on port " << port << " with " << threads
            << " threads and " << cacheSize << " bytes cache." << std::endl;

  try {
    ProxyServer server(port, threads, cacheSize);
    server.start();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
