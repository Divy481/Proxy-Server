#include "../include/httpparser.hpp"
#include <sstream>
#include <string>

static inline std::string trim(const std::string &s) {
  auto start = s.find_first_not_of(" \t");
  auto end = s.find_last_not_of(" \t");

  return (start == std::string::npos) ? " " : s.substr(start, end - start + 1);
}
HttpRequest HttpParser::parse(const std::string &raw) {
  HttpRequest req;
  std::istringstream stream(raw);
  std::string line;

  /* ---- Request Line ---- */
  std::getline(stream, line);
  if (!line.empty() && line.back() == '\r')
    line.pop_back();

  std::istringstream rl(line);
  rl >> req.method >> req.url >> req.version;

  /* ---- Headers ---- */
  while (std::getline(stream, line)) {
    if (line == "\r" || line == "")
      break;
    if (!line.empty() && line.back() == '\r')
      line.pop_back();

    auto colon = line.find(':');
    if (colon == std::string::npos)
      continue;

    std::string key = trim(line.substr(0, colon));
    std::string value = trim(line.substr(colon + 1));
    req.headers[key] = value;
  }

  /* ---- Host & Port Extraction ---- */
  if (req.method == "CONNECT") {
    // CONNECT host:port HTTP/1.1
    auto pos = req.url.find(':');
    req.host = req.url.substr(0, pos);
    req.port = std::stoi(req.url.substr(pos + 1));
    req.path = "";
  } else {
    // Absolute URL: http://host:port/path
    std::string url = req.url;

    const std::string http = "http://";
    if (url.compare(0, http.size(), http) == 0) {
      url = url.substr(http.size());
    }

    auto slash = url.find('/');
    if (slash == std::string::npos) {
      req.host = url;
      req.port = 80;
      req.path = "/";
    } else {
      std::string hostport = url.substr(0, slash);
      req.path = url.substr(slash);

      auto colon = hostport.find(':');
      if (colon != std::string::npos) {
        req.host = hostport.substr(0, colon);
        req.port = std::stoi(hostport.substr(colon + 1));
      } else {
        req.host = hostport;
        req.port = 80;
      }
    }
  }

  return req;
}