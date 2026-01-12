#include "../include/proxyServer.hpp"
#include "../include/httpparser.hpp"

#include <algorithm>
#include <memory>

#include <asm-generic/socket.h>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

ProxyServer::ProxyServer(int port, int threads, const size_t &maxSz)
    : port(port), pool(threads), cache(maxSz) {}

void ProxyServer::start() {
  Socket server;
  int opt = 1;
  setsockopt(server.fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  server.bind(port);
  server.listen();

  std::cout << "[Proxy] :: Listening on Port: " << port << std::endl;

  while (true) {
    try {
      Socket client = server.accept();
      auto clientPtr = std::make_shared<Socket>(std::move(client));
      pool.submit([this, clientPtr]() mutable {
        if (clientPtr)
          handleClient(std::move(*clientPtr));
      });
    } catch (const std::exception &e) {
      std::cerr << "[Error] Accept failed: " << e.what() << std::endl;
    }
  }
}

void ProxyServer::handleClient(Socket client) {
  try {
    std::string rawRequest;
    // Simple heuristic: read until header end.
    // In a real server, we should handle partial reads more robustly.
    while (true) {
      std::string chunk = client.recv();
      if (chunk.empty())
        return; // Connection closed

      rawRequest += chunk;
      if (rawRequest.find("\r\n\r\n") != std::string::npos) {
        break;
      }
      // Add a sanity check for max header size to prevent DoS
      if (rawRequest.size() > 16384)
        return;
    }

    HttpParser parse;
    HttpRequest req = parse.parse(rawRequest);

    if (req.host.empty()) {
      // Fallback or error
      return;
    }

    std::cout << "[Proxy] " << req.method << " " << req.url << std::endl;

    if (req.method == "CONNECT") {
      // HTTPS Tunneling
      Socket server;
      try {
        server.connect(req.host, req.port);
        client.send("HTTP/1.1 200 Connection Established\r\n\r\n");

        int clientFd = client.fd();
        int serverFd = server.fd();
        int maxFd = std::max(clientFd, serverFd);
        fd_set readfds;

        while (true) {
          FD_ZERO(&readfds);
          FD_SET(clientFd, &readfds);
          FD_SET(serverFd, &readfds);

          // Timeout of 10 seconds to clean up idle connections
          struct timeval timeout;
          timeout.tv_sec = 10;
          timeout.tv_usec = 0;

          int activity =
              select(maxFd + 1, &readfds, nullptr, nullptr, &timeout);

          if (activity < 0 && errno != EINTR)
            break;
          if (activity == 0)
            break; // Timeout

          if (FD_ISSET(clientFd, &readfds)) {
            std::string data = client.recv();
            if (data.empty())
              break;
            server.send(data);
          }

          if (FD_ISSET(serverFd, &readfds)) {
            std::string data = server.recv();
            if (data.empty())
              break;
            client.send(data);
          }
        }
      } catch (const std::exception &e) {
        std::cerr << "[Connect Error] " << e.what() << std::endl;
        client.send("HTTP/1.1 502 Bad Gateway\r\n\r\n");
      }

    } else {
      // HTTP Request with Caching
      std::string cacheKey = req.method + " " + req.url;
      std::string cachedResponse;

      if (cache.get(cacheKey, cachedResponse)) {
        std::cout << "[Cache] HIT: " << req.url << std::endl;
        client.send(cachedResponse);
      } else {
        std::cout << "[Cache] MISS: " << req.url << std::endl;
        Socket server;
        try {
          server.connect(req.host, req.port);

          // Reconstruct Request
          // Note: In a full proxy, we might need to handle headers like
          // Proxy-Connection, etc.
          std::string newReq =
              req.method + " " + req.path + " " + req.version + "\r\n";
          for (auto &pair : req.headers) {
            if (pair.first == "Proxy-Connection") {
              newReq += "Connection: close\r\n";
            } else if (pair.first != "Connection") {
              newReq += pair.first + ": " + pair.second + "\r\n";
            }
          }
          if (req.headers.find("Connection") == req.headers.end()) {
            newReq += "Connection: close\r\n"; // Enforce close for simple body
                                               // reading
          } else {
            newReq += "Connection: close\r\n";
          }

          newReq += "\r\n";

          // If there was a body in rawRequest (e.g. POST), it should be
          // forwarded. For now, we only extracted headers. If body exists, we
          // should have read it. Logic simplification: Just forward rawRequest
          // if it's GET/simple, but we need to strip absolute URL from request
          // line. Hence reconstruction is better. We ignore body for GET
          // usually.

          server.send(newReq);

          std::string fullResponse;
          while (true) {
            std::string chunk = server.recv();
            if (chunk.empty())
              break;
            fullResponse += chunk;
            client.send(chunk); // Stream to client
          }

          // Only cache if successful response ?
          // For simplicity, cache everything.
          if (!fullResponse.empty()) {
            cache.put(cacheKey, fullResponse);
          }

        } catch (const std::exception &e) {
          std::cerr << "[HTTP Error] " << e.what() << std::endl;
          client.send("HTTP/1.1 502 Bad Gateway\r\n\r\n");
        }
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "[Error] Client handling failed: " << e.what() << std::endl;
  }
}
