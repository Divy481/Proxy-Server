#pragma once

#include <cstddef>
#include <string>

class Socket{
    public:
        Socket();
        explicit Socket(int fd);
        ~Socket();

        Socket(const Socket&) = delete;
        Socket& operator =(const Socket&) = delete;

        Socket(Socket&& other)noexcept;
        Socket& operator =(Socket&& other)noexcept;

        void bind(int port);
        void listen(int backlog = 120);

        Socket accept();

        void connect(std::string& host,int port);

        std::size_t send(std::string data);
        std::string recv();

        int fd(){return sockfd;};

    private:
        int sockfd;
};