#include "common.h"

vector<InteractiveHtmlElement> interactive_html_elements;
string current_active_html = "";
string active_web_input_id = "";
string hovered_web_element_action = "";
bool hovered_web_element = false;

#ifndef _WIN32
unsigned long get_x11_color_by_name(Display* dpy, int screen, const string &name) {
  Colormap colormap = DefaultColormap(dpy, screen);
  XColor color;
  if (name.front() == '#') {
    if (XAllocNamedColor(dpy, colormap, name.c_str(), &color, &color)) {
      return color.pixel;
    }
  } else {
    if (XAllocNamedColor(dpy, colormap, name.c_str(), &color, &color)) {
      return color.pixel;
    }
  }
  return BlackPixel(dpy, screen);
}
#endif

string trim(string s) {
  size_t first = s.find_first_not_of(" \t\r\n");
  if (first == string::npos) return "";
  size_t last = s.find_last_not_of(" \t\r\n");
  return s.substr(first, (last - first + 1));
}

string get_os() {
#ifdef _WIN32
  return "Windows";
#else
  return "Linux";
#endif
}

string get_user() {
#ifdef _WIN32
  char buf[256];
  DWORD len = sizeof(buf);
  if (GetUserNameA(buf, &len))
    return string(buf);
  return "unknown";
#else
  char *u = getenv("USER");
  return u ? string(u) : "unknown";
#endif
}

string base64_encode(const string &in) {
  static const char *lookup =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  string out;
  int val = 0, valb = -6;
  for (char c : in) {
    val = (val << 8) + (unsigned char)c;
    valb += 8;
    while (valb >= 0) {
      out.push_back(lookup[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6)
    out.push_back(lookup[((val << 8) >> (valb + 8)) & 0x3F]);
  while (out.size() % 4)
    out.push_back('=');
  return out;
}

string base64_decode(const string &in) {
  static const string lookup =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  vector<int> T(256, -1);
  for (int i = 0; i < 64; i++)
    T[lookup[i]] = i;
  string out;
  int val = 0, valb = -8;
  for (char c : in) {
    if (T[c] == -1)
      continue;
    val = (val << 6) + T[c];
    valb += 6;
    while (valb >= 0) {
      out.push_back((char)((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}

string escape_json_string(const string &s) {
  string res = "";
  for (char c : s) {
    if (c == '"') res += "\\\"";
    else if (c == '\\') res += "\\\\";
    else if (c == '\n') res += "\\n";
    else if (c == '\r') res += "\\r";
    else if (c == '\t') res += "\\t";
    else res += c;
  }
  return res;
}
