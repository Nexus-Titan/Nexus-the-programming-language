#include "gui.h"
#include "networking.h"

string show_x11_dialog(const string &type, const string &text) {
#ifndef _WIN32
  Display* dpy = XOpenDisplay(NULL);
  if (!dpy) return "";
  int screen = DefaultScreen(dpy);
  Window root = RootWindow(dpy, screen);
  
  int win_w = 350, win_h = 130;
  unsigned long col_bg = get_x11_color_by_name(dpy, screen, "#1e1e1e");
  Window win = XCreateSimpleWindow(dpy, root, 150, 150, win_w, win_h, 1, 
                                   get_x11_color_by_name(dpy, screen, "#444444"), col_bg);
                                   
  string title = "Nexus Dialogue";
  if (type == "warn") title = "Nexus Warning";
  if (type == "err") title = "Nexus Error";
  XStoreName(dpy, win, title.c_str());
  
  Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);
  XSelectInput(dpy, win, ExposureMask | ButtonPressMask | KeyPressMask | PointerMotionMask);
  XMapWindow(dpy, win);
  
  GC gc = XCreateGC(dpy, win, 0, NULL);
  XFontStruct* font = XLoadQueryFont(dpy, "-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*");
  if (!font) font = XLoadQueryFont(dpy, "fixed");
  if (font) XSetFont(dpy, gc, font->fid);
  
  unsigned long col_fg = get_x11_color_by_name(dpy, screen, "#ffffff");
  unsigned long col_btn_bg = get_x11_color_by_name(dpy, screen, type == "err" ? "#dc2626" : (type == "warn" ? "#f59e0b" : "#2563eb"));
  unsigned long col_btn_hover = get_x11_color_by_name(dpy, screen, type == "err" ? "#ef4444" : (type == "warn" ? "#fbbf24" : "#3b82f6"));
  unsigned long col_input_bg = get_x11_color_by_name(dpy, screen, "#2d2d2d");
  unsigned long col_input_border = get_x11_color_by_name(dpy, screen, "#444444");
  
  struct DlgBtn {
    string text;
    int x, y, w, h;
    bool hover;
    string value;
  };
  vector<DlgBtn> buttons;
  int input_x = 25, input_y = 50, input_w = 300, input_h = 24;
  string input_val = "";
  
  if (type == "msg" || type == "warn" || type == "err") {
    buttons.push_back({"OK", 137, 85, 75, 28, false, "1"});
  } else if (type == "ask") {
    buttons.push_back({"Yes", 80, 85, 80, 28, false, "1"});
    buttons.push_back({"No", 190, 85, 80, 28, false, "0"});
  } else if (type == "input") {
    buttons.push_back({"OK", 137, 85, 75, 28, false, "ok"});
  }
  
  bool active = true;
  string result = "";
  XEvent ev;
  
  auto draw_dlg = [&]() {
    XClearWindow(dpy, win);
    XSetForeground(dpy, gc, col_fg);
    XDrawString(dpy, win, gc, 25, 32, text.c_str(), text.length());
    
    if (type == "input") {
      XSetForeground(dpy, gc, col_input_bg);
      XFillRectangle(dpy, win, gc, input_x, input_y, input_w, input_h);
      XSetForeground(dpy, gc, col_input_border);
      XDrawRectangle(dpy, win, gc, input_x, input_y, input_w, input_h);
      XSetForeground(dpy, gc, col_fg);
      XDrawString(dpy, win, gc, input_x + 8, input_y + 16, input_val.c_str(), input_val.length());
      int cur_x = input_x + 8 + (font ? XTextWidth(font, input_val.c_str(), input_val.length()) : input_val.length() * 8);
      XDrawLine(dpy, win, gc, cur_x, input_y + 4, cur_x, input_y + 20);
    }
    for (auto &b : buttons) {
      XSetForeground(dpy, gc, b.hover ? col_btn_hover : col_btn_bg);
      XFillRectangle(dpy, win, gc, b.x, b.y, b.w, b.h);
      XSetForeground(dpy, gc, col_fg);
      int tw = font ? XTextWidth(font, b.text.c_str(), b.text.length()) : b.text.length() * 8;
      XDrawString(dpy, win, gc, b.x + (b.w - tw) / 2, b.y + 18, b.text.c_str(), b.text.length());
    }
    XFlush(dpy);
  };
  
  while (active) {
    XNextEvent(dpy, &ev);
    if (ev.type == Expose && ev.xexpose.count == 0) {
      draw_dlg();
    } else if (ev.type == ClientMessage) {
      if ((Atom)ev.xclient.data.l[0] == wmDeleteMessage) {
        result = "0";
        active = false;
      }
    } else if (ev.type == MotionNotify) {
      int mx = ev.xmotion.x;
      int my = ev.xmotion.y;
      bool needs_redraw = false;
      for (auto &b : buttons) {
        bool h = (mx >= b.x && mx <= b.x + b.w && my >= b.y && my <= b.y + b.h);
        if (h != b.hover) {
          b.hover = h;
          needs_redraw = true;
        }
      }
      if (needs_redraw) draw_dlg();
    } else if (ev.type == ButtonPress && ev.xbutton.button == Button1) {
      int mx = ev.xbutton.x;
      int my = ev.xbutton.y;
      for (auto &b : buttons) {
        if (mx >= b.x && mx <= b.x + b.w && my >= b.y && my <= b.y + b.h) {
          result = (type == "input") ? input_val : b.value;
          active = false;
        }
      }
    } else if (ev.type == KeyPress && type == "input") {
      char buf[32];
      KeySym ksym;
      int len = XLookupString(&ev.xkey, buf, sizeof(buf), &ksym, NULL);
      if (ksym == XK_BackSpace) {
        if (!input_val.empty()) {
          input_val.pop_back();
          draw_dlg();
        }
      } else if (ksym == XK_Return) {
        result = input_val;
        active = false;
      } else if (len > 0 && buf[0] >= 32 && buf[0] < 127) {
        input_val += buf[0];
        draw_dlg();
      }
    }
  }
  
  if (font) XFreeFont(dpy, font);
  XFreeGC(dpy, gc);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  return result;
#else
  return "";
#endif
}
