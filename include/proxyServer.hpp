#pragma once

#include "Socket.hpp"
#include "threadpool.hpp"
#include "cache.hpp"
#include <cstddef>

class ProxyServer{
    public:
        ProxyServer(int port,int threads,const size_t& maxSz);
        void start();

    private:
        void handleClient(Socket client);

        int port;
        ThreadPool pool;
        LRUCache cache;
};
