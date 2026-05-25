#pragma once
#include "common.h"

struct ParsedUrl {
  bool is_ssl;
  string host;
  int port;
  string path;
};

ParsedUrl parse_url(string url);
string native_http_request(const string &method, const string &url, const string &post_data = "");
