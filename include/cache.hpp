#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include <mutex>
#include <cstddef>
#include <utility>


class LRUCache{
    public:
        explicit LRUCache(const size_t& maxSize);
        bool get(const std::string& key,std::string& value);
        void put(const std::string& key,const std::string& value);

    private:

        struct CacheEntry{
            std::string data;
            size_t size;
        };

        size_t maxSize;
        size_t currentSize;

        std::list<std::string> lrulist;
        std::unordered_map<std::string, std::pair<CacheEntry, std::list<std::string>::iterator>>cache;
        std::mutex cacheMutex;

        void evictIfNeed();

};
