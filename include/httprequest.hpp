#pragma once

#include <string>
#include <unordered_map>
struct HttpRequest{
    std::string url;
    std::string method;
    std::string path;
    std::string version;

    std::string host;
    int port = 80;

    std::unordered_map<std::string, std::string>headers;
};

