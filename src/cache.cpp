#include "../include/cache.hpp"
#include <cstddef>
#include <mutex>

LRUCache::LRUCache(const size_t& maxSZ) : maxSize(maxSZ) , currentSize(0){};


bool LRUCache::get(const std::string& key,std::string& value){

    std::lock_guard<std::mutex> lock(cacheMutex);

    auto it = cache.find(key);
    if(it == cache.end())return false; 
    
    lrulist.splice(lrulist.begin(),lrulist,it->second.second);

    value = it->second.first.data;

    return true;
}

void LRUCache::put(const std::string& key,const std::string& value){
    std::lock_guard<std::mutex> lock(cacheMutex);

    auto it = cache.find(key);
    if(it != cache.end()){
        currentSize -= it->second.first.size;
        lrulist.erase(it->second.second);
        cache.erase(it);
    }

    lrulist.push_front(key);
    CacheEntry tempCache;
    tempCache.data = value;
    tempCache.size = value.size();

    cache[key] = {tempCache,lrulist.begin()};

    currentSize += tempCache.size;

    evictIfNeed();
}

void LRUCache::evictIfNeed(){
    while(currentSize>maxSize && !lrulist.empty()){

        auto key = lrulist.back();
        auto it = cache.find(key);

        currentSize -= it->second.first.size;

        cache.erase(key);
        lrulist.pop_back();
    }
}



