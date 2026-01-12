#include "../include/Socket.hpp"
#include <arpa/inet.h>
#include <cstddef>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

Socket::Socket() {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    throw std::runtime_error("socket failed");
  }
}
Socket::Socket(int fd) : sockfd(fd) {};

Socket::~Socket() {
  if (sockfd >= 0)
    ::close(sockfd);
}

Socket::Socket(Socket &&other) noexcept {
  sockfd = other.sockfd;
  other.sockfd = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
    ::close(sockfd);
    sockfd = other.sockfd;
    other.sockfd = -1;
  }
  return *this;
}

void Socket::bind(int port) {
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (::bind(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0) {
    throw std::runtime_error("bind failed");
  }
}

void Socket::listen(int backlog) {
  if (::listen(sockfd, backlog) < 0) {
    throw std::runtime_error("listen failed");
  }
}

Socket Socket::accept() {
  int clientFd = ::accept(sockfd, nullptr, nullptr);
  if (clientFd < 0) {
    throw std::runtime_error("accept failed");
  }
  return Socket(clientFd);
}

void Socket::connect(std::string &host, int port) {
  hostent *server = gethostbyname(host.c_str());
  if (!server) {
    throw std::runtime_error("DNS Failed");
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  std::memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);
  addr.sin_port = htons(port);

  if (::connect(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0) {
    throw std::runtime_error("connect failed");
  }
}

size_t Socket::send(std::string data) {
  return ::send(sockfd, data.c_str(), data.size(), 0);
}

std::string Socket::recv() {
  char buffer[BUFFER_SIZE];
  ssize_t bytes = ::recv(sockfd, buffer, BUFFER_SIZE, 0);
  if (bytes <= 0)
    return "";
  return std::string(buffer, bytes);
}