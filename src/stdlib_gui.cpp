#include "stdlib.h"
#include "networking.h"
#include "gui.h"

string execute_stdlib_gui(const string &mod, const string &func, const vector<string> &args, NexusUltraV4* engine) {
  if (mod == "gui") {
    if (func == "window" && !args.empty()) {
      engine->vars["GUI_TITLE"] = args[0];
      if (args.size() > 1) engine->vars["GUI_GEOM"] = args[1];
      return "1";
    }
    if (func == "geom" && !args.empty()) {
      engine->vars["GUI_GEOM"] = args[0];
      return "1";
    }
    if (func == "color" && !args.empty()) {
      engine->vars["GUI_COLOR"] = args[0];
      return "1";
    }
    if (func == "bg_color" && !args.empty()) {
      engine->vars["GUI_BG_COLOR"] = args[0];
      return "1";
    }
    if (func == "canvas_color" && !args.empty()) {
      engine->vars["GUI_CANVAS_COLOR"] = args[0];
      return "1";
    }
    if (func == "font" && args.size() >= 2) {
      engine->vars["GUI_FONT_NAME"] = args[0];
      engine->vars["GUI_FONT_SIZE"] = args[1];
      return "1";
    }
    if (func == "size" && args.size() >= 2) {
      engine->vars["GUI_GEOM"] = args[0] + "x" + args[1];
      return "1";
    }
    if (func == "position" && args.size() >= 2) {
      engine->vars["GUI_POS_X"] = args[0];
      engine->vars["GUI_POS_Y"] = args[1];
      return "1";
    }
    if (func == "resizable" && !args.empty()) {
      engine->vars["GUI_RESIZABLE"] = args[0];
      return "1";
    }
    if (func == "label" && !args.empty()) {
      engine->gui_widgets.push_back({"label", "lbl_" + to_string(engine->gui_widgets.size()), args[0], ""});
      return "1";
    }
    if (func == "button" && args.size() >= 2) {
      engine->gui_widgets.push_back({"button", "btn_" + to_string(engine->gui_widgets.size()), args[0], args[1]});
      engine->gui_buttons.push_back({args[0], args[1]});
      return "1";
    }
    if (func == "entry" && args.size() >= 2) {
      engine->gui_widgets.push_back({"entry", args[0], args[1], ""});
      engine->gui_entries[args[0]] = args[1];
      return "1";
    }
    if (func == "get_entry" && !args.empty()) {
      return engine->gui_entries.count(args[0]) ? engine->gui_entries[args[0]] : "";
    }
    if (func == "draw_rect" && args.size() >= 5) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      double w = stod(args[2]);
      double h = stod(args[3]);
      string color = args[4];
      engine->draw_commands.push_back({"rect", x, y, x + w, y + h, 0, color, ""});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_line" && args.size() >= 5) {
      double x1 = stod(args[0]);
      double y1 = stod(args[1]);
      double x2 = stod(args[2]);
      double y2 = stod(args[3]);
      string color = args[4];
      engine->draw_commands.push_back({"line", x1, y1, x2, y2, 0, color, ""});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_circle" && args.size() >= 4) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      double r = stod(args[2]);
      string color = args[3];
      engine->draw_commands.push_back({"circle", x, y, 0, 0, r, color, ""});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_rect_outline" && args.size() >= 5) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      double w = stod(args[2]);
      double h = stod(args[3]);
      string color = args[4];
      engine->draw_commands.push_back({"rect_outline", x, y, x + w, y + h, 0, color, ""});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_circle_outline" && args.size() >= 4) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      double r = stod(args[2]);
      string color = args[3];
      engine->draw_commands.push_back({"circle_outline", x, y, 0, 0, r, color, ""});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_text" && args.size() >= 4) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      string text = args[2];
      string color = args[3];
      engine->draw_commands.push_back({"text", x, y, 0, 0, 0, color, text});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_rounded_rect" && args.size() >= 6) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      double w = stod(args[2]);
      double h = stod(args[3]);
      double r = stod(args[4]);
      string color = args[5];
      engine->draw_commands.push_back({"rounded_rect", x, y, x + w, y + h, r, color, "", "", 0.0, 0.0, 0});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_rounded_rect_outline" && args.size() >= 6) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      double w = stod(args[2]);
      double h = stod(args[3]);
      double r = stod(args[4]);
      string color = args[5];
      engine->draw_commands.push_back({"rounded_rect_outline", x, y, x + w, y + h, r, color, "", "", 0.0, 0.0, 0});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_blur" && args.size() >= 5) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      double w = stod(args[2]);
      double h = stod(args[3]);
      int intensity = stoi(args[4]);
      engine->draw_commands.push_back({"blur", x, y, x + w, y + h, 0, "", "", "", 0.0, 0.0, intensity});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_gradient" && args.size() >= 7) {
      double x = stod(args[0]);
      double y = stod(args[1]);
      double w = stod(args[2]);
      double h = stod(args[3]);
      string color1 = args[4];
      string color2 = args[5];
      int dir = stoi(args[6]); // 0 = horizontal, 1 = vertical
      engine->draw_commands.push_back({"gradient", x, y, x + w, y + h, 0, color1, "", color2, 0.0, 0.0, dir});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_triangle" && args.size() >= 7) {
      double x1 = stod(args[0]);
      double y1 = stod(args[1]);
      double x2 = stod(args[2]);
      double y2 = stod(args[3]);
      double x3 = stod(args[4]);
      double y3 = stod(args[5]);
      string color = args[6];
      engine->draw_commands.push_back({"triangle", x1, y1, x2, y2, 0, color, "", "", x3, y3, 0});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "draw_triangle_outline" && args.size() >= 7) {
      double x1 = stod(args[0]);
      double y1 = stod(args[1]);
      double x2 = stod(args[2]);
      double y2 = stod(args[3]);
      double x3 = stod(args[4]);
      double y3 = stod(args[5]);
      string color = args[6];
      engine->draw_commands.push_back({"triangle_outline", x1, y1, x2, y2, 0, color, "", "", x3, y3, 0});
      engine->has_canvas = true;
      return "1";
    }
    if (func == "clear") {
      engine->gui_widgets.clear();
      engine->gui_buttons.clear();
      engine->gui_entries.clear();
      engine->draw_commands.clear();
      engine->has_canvas = false;
      return "1";
    }
    if (func == "run") {
      engine->show_gui_window();
      return "1";
    }
    if (func == "msg" && !args.empty()) {
#ifndef _WIN32
      show_x11_dialog("msg", args[0]);
#else
      MessageBoxA(NULL, args[0].c_str(), "Nexus", MB_OK | MB_ICONINFORMATION);
#endif
      return "1";
    }
    if (func == "warn" && !args.empty()) {
#ifndef _WIN32
      show_x11_dialog("warn", args[0]);
#else
      MessageBoxA(NULL, args[0].c_str(), "Nexus Warning", MB_OK | MB_ICONWARNING);
#endif
      return "1";
    }
    if (func == "err" && !args.empty()) {
#ifndef _WIN32
      show_x11_dialog("err", args[0]);
#else
      MessageBoxA(NULL, args[0].c_str(), "Nexus Error", MB_OK | MB_ICONERROR);
#endif
      return "1";
    }
    if (func == "ask" && !args.empty()) {
#ifndef _WIN32
      return show_x11_dialog("ask", args[0]);
#else
      int res = MessageBoxA(NULL, args[0].c_str(), "Nexus", MB_YESNO | MB_ICONQUESTION);
      return res == IDYES ? "1" : "0";
#endif
    }
    if (func == "input" && !args.empty()) {
#ifndef _WIN32
      return show_x11_dialog("input", args[0]);
#else
      return "";
#endif
    }
  }

  if (mod == "web") {
    if (func == "render" && !args.empty()) {
      current_active_html = args[0];
      engine->parse_and_render_html(current_active_html);
      return "1";
    }
    if (func == "load" && !args.empty()) {
      string url = args[0];
      string new_html = trim(native_http_request("GET", url));
      current_active_html = new_html;
      engine->parse_and_render_html(current_active_html);
      return "1";
    }
  }

  if (mod == "net") {
    if (func == "get" && !args.empty()) return native_http_request("GET", args[0]);
    if (func == "post" && args.size() > 1) return native_http_request("POST", args[0], args[1]);
    if (func == "download" && args.size() > 1) {
      string res = native_http_request("GET", args[0]);
      ofstream out(args[1], ios::binary);
      if (out.is_open()) {
        out.write(res.c_str(), res.length());
        out.close();
        return "1";
      }
      return "0";
    }
    if (func == "ping" && !args.empty()) {
#ifdef _WIN32
      return to_string(system(("ping -n 1 " + args[0] + " >nul 2>&1").c_str()) == 0);
#else
      return to_string(system(("ping -c 1 " + args[0] + " >/dev/null 2>&1").c_str()) == 0);
#endif
    }
    if (func == "ip") {
      string res = trim(native_http_request("GET", "https://api.ipify.org"));
      if (!res.empty() && res.find("Error") == string::npos && res.find("HTTP") == string::npos) {
        return res;
      }
      return "127.0.0.1";
    }
    if (func == "is_online") {
#ifdef _WIN32
      return to_string(system("ping -n 1 8.8.8.8 >nul 2>&1") == 0);
#else
      return to_string(system("ping -c 1 8.8.8.8 >/dev/null 2>&1") == 0);
#endif
    }
    if (func == "get_json" && !args.empty()) return native_http_request("GET", args[0]);
  }

  return "";
}

string call_stdlib(const string &mod, const string &func, const vector<string> &args) {
  NexusUltraV4* engine = (NexusUltraV4*)g_engine_ptr;
  string r = execute_stdlib_basic(mod, func, args, engine);
  if (!r.empty()) return r;
  r = execute_stdlib_io(mod, func, args, engine);
  if (!r.empty()) return r;
  r = execute_stdlib_gui(mod, func, args, engine);
  return r;
}
