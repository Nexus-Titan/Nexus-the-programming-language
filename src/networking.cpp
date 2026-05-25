#include "networking.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#ifndef _WIN32
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

ParsedUrl parse_url(string url) {
  ParsedUrl res;
  res.is_ssl = false;
  res.port = 80;
  res.path = "/";
  
  url = trim(url);
  if (url.compare(0, 8, "https://") == 0) {
    res.is_ssl = true;
    res.port = 443;
    url = url.substr(8);
  } else if (url.compare(0, 7, "http://") == 0) {
    url = url.substr(7);
  }
  
  size_t slash = url.find('/');
  if (slash != string::npos) {
    res.path = url.substr(slash);
    res.host = url.substr(0, slash);
  } else {
    res.host = url;
  }
  
  size_t colon = res.host.find(':');
  if (colon != string::npos) {
    res.port = stoi(res.host.substr(colon + 1));
    res.host = res.host.substr(0, colon);
  }
  
  return res;
}

string native_http_request(const string &method, const string &url, const string &post_data) {
  ParsedUrl p = parse_url(url);

#ifdef _WIN32
  HINTERNET hSession = InternetOpenA("Nexus/4.5", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
  if (!hSession) return "";
  
  DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE;
  if (p.is_ssl) {
    flags |= INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
  }
  
  HINTERNET hUrl = InternetOpenUrlA(hSession, url.c_str(), NULL, 0, flags, 0);
  if (!hUrl) {
    InternetCloseHandle(hSession);
    return "";
  }
  
  string response = "";
  char buf[4096];
  DWORD readBytes = 0;
  while (InternetReadFile(hUrl, buf, sizeof(buf), &readBytes) && readBytes > 0) {
    response.append(buf, readBytes);
  }
  
  InternetCloseHandle(hUrl);
  InternetCloseHandle(hSession);
  return response;
#else
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return "";
  
  struct hostent* he = gethostbyname(p.host.c_str());
  if (!he) {
    close(sock);
    return "";
  }
  
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(p.port);
  memcpy(&addr.sin_addr.s_addr, he->h_addr_list[0], he->h_length);
  
  if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    close(sock);
    return "";
  }
  
  SSL* ssl = NULL;
  SSL_CTX* ctx = NULL;
  
  if (p.is_ssl) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD* m_method = TLS_client_method();
    ctx = SSL_CTX_new(m_method);
    if (!ctx) {
      close(sock);
      return "";
    }
    ssl = SSL_new(ctx);
    if (!ssl) {
      SSL_CTX_free(ctx);
      close(sock);
      return "";
    }
    SSL_set_fd(ssl, sock);
    SSL_set_tlsext_host_name(ssl, p.host.c_str());
    if (SSL_connect(ssl) <= 0) {
      SSL_free(ssl);
      SSL_CTX_free(ctx);
      close(sock);
      return "";
    }
  }
  
  string req = method + " " + p.path + " HTTP/1.1\r\n";
  req += "Host: " + p.host + "\r\n";
  req += "User-Agent: Nexus/4.5\r\n";
  req += "Connection: close\r\n";
  if (method == "POST") {
    req += "Content-Type: application/x-www-form-urlencoded\r\n";
    req += "Content-Length: " + to_string(post_data.length()) + "\r\n";
    req += "\r\n" + post_data;
  } else {
    req += "\r\n";
  }
  
  int sent = 0;
  if (p.is_ssl) {
    sent = SSL_write(ssl, req.c_str(), req.length());
  } else {
    sent = send(sock, req.c_str(), req.length(), 0);
  }
  
  if (sent <= 0) {
    if (p.is_ssl) {
      SSL_free(ssl);
      SSL_CTX_free(ctx);
    }
    close(sock);
    return "";
  }
  
  string response = "";
  char buf[4096];
  int n = 0;
  while (true) {
    if (p.is_ssl) {
      n = SSL_read(ssl, buf, sizeof(buf) - 1);
    } else {
      n = recv(sock, buf, sizeof(buf) - 1, 0);
    }
    if (n <= 0) break;
    response.append(buf, n);
  }
  
  if (p.is_ssl) {
    SSL_free(ssl);
    SSL_CTX_free(ctx);
  }
  close(sock);
  
  size_t header_end = response.find("\r\n\r\n");
  if (header_end == string::npos) return response;
  
  string headers = response.substr(0, header_end);
  string body = response.substr(header_end + 4);
  
  size_t chunked = headers.find("Transfer-Encoding: chunked");
  if (chunked != string::npos || headers.find("transfer-encoding: chunked") != string::npos) {
    string decoded = "";
    size_t pos = 0;
    while (pos < body.length()) {
      size_t line_end = body.find("\r\n", pos);
      if (line_end == string::npos) break;
      string size_str = body.substr(pos, line_end - pos);
      unsigned int chunk_size = 0;
      sscanf(size_str.c_str(), "%x", &chunk_size);
      if (chunk_size == 0) break;
      pos = line_end + 2;
      if (pos + chunk_size > body.length()) break;
      decoded.append(body.substr(pos, chunk_size));
      pos += chunk_size + 2;
    }
    return decoded;
  }
  
  return body;
#endif
}
