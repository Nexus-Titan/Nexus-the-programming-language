#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstring>
#include <cstdlib>

#ifndef _WIN32
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#else
#include <windows.h>
#include <wininet.h>
#endif

using namespace std;

struct GuiWidget {
  string type;
  string id;
  string text;
  string action;
};

struct DrawCommand {
  string type;
  double x1, y1, x2, y2, r;
  string color;
  string text;
  string color2;
  double x3, y3;
  int extra_int;
};

struct InteractiveHtmlElement {
  int x1, y1, x2, y2;
  string type;
  string action;
};

extern vector<InteractiveHtmlElement> interactive_html_elements;
extern string current_active_html;
extern string active_web_input_id;
extern string hovered_web_element_action;
extern bool hovered_web_element;

#ifndef _WIN32
extern unsigned long get_x11_color_by_name(Display* dpy, int screen, const string &name);
#endif

string trim(string s);
string get_os();
string get_user();
string base64_encode(const string &in);
string base64_decode(const string &in);
string escape_json_string(const string &s);
