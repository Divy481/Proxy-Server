#pragma once

#include "httprequest.hpp"


class HttpParser{
    public:
         HttpRequest parse(const std::string& raw);
};

