#include "gui.h"
#include "networking.h"
#ifndef _WIN32
#include <X11/cursorfont.h>
#endif





#ifndef _WIN32
static void draw_x11_rounded_rect(Display* dpy, Window win, GC gc, int x, int y, int w, int h, int r, bool fill) {
  if (r <= 0) {
    if (fill) XFillRectangle(dpy, win, gc, x, y, w, h);
    else XDrawRectangle(dpy, win, gc, x, y, w, h);
    return;
  }
  if (r > w / 2) r = w / 2;
  if (r > h / 2) r = h / 2;
  
  if (fill) {
    XFillRectangle(dpy, win, gc, x + r, y, w - 2 * r, h);
    XFillRectangle(dpy, win, gc, x, y + r, r, h - 2 * r);
    XFillRectangle(dpy, win, gc, x + w - r, y + r, r, h - 2 * r);
    XFillArc(dpy, win, gc, x, y, 2 * r, 2 * r, 90 * 64, 90 * 64);
    XFillArc(dpy, win, gc, x + w - 2 * r, y, 2 * r, 2 * r, 0 * 64, 90 * 64);
    XFillArc(dpy, win, gc, x, y + h - 2 * r, 2 * r, 2 * r, 180 * 64, 90 * 64);
    XFillArc(dpy, win, gc, x + w - 2 * r, y + h - 2 * r, 2 * r, 2 * r, 270 * 64, 90 * 64);
  } else {
    XDrawLine(dpy, win, gc, x + r, y, x + w - r, y);
    XDrawLine(dpy, win, gc, x + r, y + h, x + w - r, y + h);
    XDrawLine(dpy, win, gc, x, y + r, x, y + h - r);
    XDrawLine(dpy, win, gc, x + w, y + r, x + w, y + h - r);
    XDrawArc(dpy, win, gc, x, y, 2 * r, 2 * r, 90 * 64, 90 * 64);
    XDrawArc(dpy, win, gc, x + w - 2 * r, y, 2 * r, 2 * r, 0 * 64, 90 * 64);
    XDrawArc(dpy, win, gc, x, y + h - 2 * r, 2 * r, 2 * r, 180 * 64, 90 * 64);
    XDrawArc(dpy, win, gc, x + w - 2 * r, y + h - 2 * r, 2 * r, 2 * r, 270 * 64, 90 * 64);
  }
}

static void blur_x11_region(Display* dpy, Window win, GC gc, int rx, int ry, int rw, int rh, int intensity) {
  if (rw <= 0 || rh <= 0) return;
  if (intensity <= 0) intensity = 3;
  if (intensity > 15) intensity = 15;
  
  XImage* img = XGetImage(dpy, win, rx, ry, rw, rh, AllPlanes, ZPixmap);
  if (!img) return;
  
  vector<unsigned long> original(rw * rh);
  for (int y = 0; y < rh; ++y) {
    for (int x = 0; x < rw; ++x) {
      original[y * rw + x] = XGetPixel(img, x, y);
    }
  }
  
  for (int y = 0; y < rh; ++y) {
    for (int x = 0; x < rw; ++x) {
      long r_sum = 0, g_sum = 0, b_sum = 0;
      int count = 0;
      for (int dy = -intensity; dy <= intensity; ++dy) {
        int ny = y + dy;
        if (ny < 0 || ny >= rh) continue;
        for (int dx = -intensity; dx <= intensity; ++dx) {
          int nx = x + dx;
          if (nx < 0 || nx >= rw) continue;
          
          unsigned long pixel = original[ny * rw + nx];
          int r = (pixel & img->red_mask) >> 16;
          int g = (pixel & img->green_mask) >> 8;
          int b = (pixel & img->blue_mask);
          
          r_sum += r;
          g_sum += g;
          b_sum += b;
          count++;
        }
      }
      
      int avg_r = r_sum / count;
      int avg_g = g_sum / count;
      int avg_b = b_sum / count;
      
      unsigned long new_pixel = ((avg_r & 0xFF) << 16) | ((avg_g & 0xFF) << 8) | (avg_b & 0xFF);
      XPutPixel(img, x, y, new_pixel);
    }
  }
  
  XPutImage(dpy, win, gc, img, 0, 0, rx, ry, rw, rh);
  XDestroyImage(img);
}

static string interpolate_color(const string &c1, const string &c2, double factor) {
  int r1 = 0, g1 = 0, b1 = 0;
  int r2 = 0, g2 = 0, b2 = 0;
  
  auto parse_hex = [](const string &hex, int &r, int &g, int &b) {
    if (hex.front() == '#' && hex.length() >= 7) {
      sscanf(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
    } else if (hex == "white") { r = 255; g = 255; b = 255; }
    else if (hex == "black") { r = 0; g = 0; b = 0; }
    else if (hex == "red") { r = 255; g = 0; b = 0; }
    else if (hex == "green") { r = 0; g = 255; b = 0; }
    else if (hex == "blue") { r = 0; g = 0; b = 255; }
    else if (hex == "yellow") { r = 255; g = 255; b = 0; }
  };
  
  parse_hex(c1, r1, g1, b1);
  parse_hex(c2, r2, g2, b2);
  
  int r = (int)(r1 + (r2 - r1) * factor);
  int g = (int)(g1 + (g2 - g1) * factor);
  int b = (int)(b1 + (b2 - b1) * factor);
  
  char buf[16];
  snprintf(buf, sizeof(buf), "#%02x%02x%02x", r & 0xFF, g & 0xFF, b & 0xFF);
  return string(buf);
}
#endif

void NexusUltraV4::show_gui_window() {
#ifndef _WIN32
  Display* dpy = XOpenDisplay(NULL);
  if (!dpy) {
    cerr << "Error: Unable to open X11 display. GUI is not available." << endl;
    return;
  }
  int screen = DefaultScreen(dpy);
  Window root_win = RootWindow(dpy, screen);
  
  int win_w = 500, win_h = 400;
  if (vars.count("GUI_GEOM")) {
    string geom = vars["GUI_GEOM"];
    size_t x_pos = geom.find('x');
    if (x_pos != string::npos) {
      win_w = stoi(geom.substr(0, x_pos));
      win_h = stoi(geom.substr(x_pos + 1));
    }
  }
  
  int win_x = 100, win_y = 100;
  if (vars.count("GUI_POS_X")) win_x = stoi(vars["GUI_POS_X"]);
  if (vars.count("GUI_POS_Y")) win_y = stoi(vars["GUI_POS_Y"]);
  
  bool is_light = (vars.count("GUI_COLOR") && vars["GUI_COLOR"] == "white");
  
  string bg_hex = vars.count("GUI_BG_COLOR") ? vars["GUI_BG_COLOR"] : (is_light ? "#f3f4f6" : "#121212");
  string canvas_hex = vars.count("GUI_CANVAS_COLOR") ? vars["GUI_CANVAS_COLOR"] : (is_light ? "#ffffff" : "#1e1e1e");
  
  unsigned long col_bg = get_x11_color_by_name(dpy, screen, bg_hex);
  unsigned long col_fg = get_x11_color_by_name(dpy, screen, is_light ? "#111827" : "#ffffff");
  unsigned long col_muted = get_x11_color_by_name(dpy, screen, "#6b7280");
  unsigned long col_btn_bg = get_x11_color_by_name(dpy, screen, "#2563eb");
  unsigned long col_btn_hover = get_x11_color_by_name(dpy, screen, "#3b82f6");
  unsigned long col_btn_fg = get_x11_color_by_name(dpy, screen, "#ffffff");
  unsigned long col_input_bg = get_x11_color_by_name(dpy, screen, is_light ? "#ffffff" : "#2d2d2d");
  unsigned long col_input_border = get_x11_color_by_name(dpy, screen, "#4b5563");
  unsigned long col_canvas_bg = get_x11_color_by_name(dpy, screen, canvas_hex);
  
  Window win = XCreateSimpleWindow(dpy, root_win, win_x, win_y, win_w, win_h, 1, 
                                   get_x11_color_by_name(dpy, screen, "#444444"), col_bg);
                                   
  Cursor hand_cursor = XCreateFontCursor(dpy, XC_hand2);
  Cursor arrow_cursor = XCreateFontCursor(dpy, XC_left_ptr);
                                   
  if (vars.count("GUI_RESIZABLE") && vars["GUI_RESIZABLE"] == "0") {
    XSizeHints* hints = XAllocSizeHints();
    hints->flags = PMinSize | PMaxSize;
    hints->min_width = hints->max_width = win_w;
    hints->min_height = hints->max_height = win_h;
    XSetWMNormalHints(dpy, win, hints);
    XFree(hints);
  }
                                   
  string win_title = vars.count("GUI_TITLE") ? vars["GUI_TITLE"] : "Nexus Window";
  XStoreName(dpy, win, win_title.c_str());
  
  Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);
  XSelectInput(dpy, win, ExposureMask | ButtonPressMask | KeyPressMask | PointerMotionMask);
  XMapWindow(dpy, win);
  
  GC gc = XCreateGC(dpy, win, 0, NULL);
  
  string font_name = vars.count("GUI_FONT_NAME") ? vars["GUI_FONT_NAME"] : "helvetica";
  string font_sz = vars.count("GUI_FONT_SIZE") ? vars["GUI_FONT_SIZE"] : "12";
  string font_pattern = "-*-" + font_name + "-bold-r-normal-*-" + font_sz + "-*-*-*-*-*-*-*";
  
  XFontStruct* font = XLoadQueryFont(dpy, font_pattern.c_str());
  if (!font) font = XLoadQueryFont(dpy, "-*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-*");
  if (!font) font = XLoadQueryFont(dpy, "fixed");
  if (font) XSetFont(dpy, gc, font->fid);
  
  int margin_x = 25;
  int margin_y = 25;
  int current_y = margin_y;
  
  struct LayoutWidget {
    string type;
    string id;
    string text;
    string action;
    int x, y, w, h;
    bool hover;
  };
  
  vector<LayoutWidget> layout_widgets;
  for (auto &w : gui_widgets) {
    LayoutWidget lw;
    lw.type = w.type;
    lw.id = w.id;
    lw.text = w.text;
    lw.action = w.action;
    lw.hover = false;
    
    if (w.type == "label") {
      lw.x = margin_x;
      lw.y = current_y;
      lw.w = win_w - 2 * margin_x;
      lw.h = 24;
      current_y += lw.h + 10;
    } else if (w.type == "entry") {
      current_y += 16 + 2;
      lw.x = margin_x;
      lw.y = current_y;
      lw.w = win_w - 2 * margin_x;
      lw.h = 30;
      current_y += lw.h + 15;
    } else if (w.type == "button") {
      lw.x = margin_x;
      lw.y = current_y;
      lw.w = win_w - 2 * margin_x;
      lw.h = 38;
      current_y += lw.h + 12;
    }
    layout_widgets.push_back(lw);
  }
  
  int canvas_x = 0, canvas_y = 0, canvas_w = 0, canvas_h = 0;
  if (has_canvas) {
    canvas_x = margin_x;
    canvas_y = current_y;
    canvas_w = win_w - 2 * margin_x;
    canvas_h = win_h - current_y - margin_y;
    if (canvas_h < 100) canvas_h = 150;
  }
  
  string active_entry_id = "";
  if (!layout_widgets.empty()) {
    for (auto &lw : layout_widgets) {
      if (lw.type == "entry") {
        active_entry_id = lw.id;
        break;
      }
    }
  }
  
  gui_active = true;
  XEvent ev;
  
  auto draw_all = [&]() {
    XClearWindow(dpy, win);
    if (has_canvas) {
      XSetForeground(dpy, gc, col_canvas_bg);
      XFillRectangle(dpy, win, gc, canvas_x, canvas_y, canvas_w, canvas_h);
      for (auto &cmd : draw_commands) {
        unsigned long col = get_x11_color_by_name(dpy, screen, cmd.color);
        XSetForeground(dpy, gc, col);
        if (cmd.type == "rect") {
          XFillRectangle(dpy, win, gc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1, 
                                  (int)(cmd.x2 - cmd.x1), (int)(cmd.y2 - cmd.y1));
        } else if (cmd.type == "rect_outline") {
          XDrawRectangle(dpy, win, gc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1, 
                                  (int)(cmd.x2 - cmd.x1), (int)(cmd.y2 - cmd.y1));
        } else if (cmd.type == "line") {
          XDrawLine(dpy, win, gc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1, 
                                  canvas_x + (int)cmd.x2, canvas_y + (int)cmd.y2);
        } else if (cmd.type == "circle") {
          XFillArc(dpy, win, gc, canvas_x + (int)cmd.x1 - (int)cmd.r, canvas_y + (int)cmd.y1 - (int)cmd.r, 
                                 2 * (int)cmd.r, 2 * (int)cmd.r, 0, 360 * 64);
        } else if (cmd.type == "circle_outline") {
          XDrawArc(dpy, win, gc, canvas_x + (int)cmd.x1 - (int)cmd.r, canvas_y + (int)cmd.y1 - (int)cmd.r, 
                                 2 * (int)cmd.r, 2 * (int)cmd.r, 0, 360 * 64);
        } else if (cmd.type == "text") {
          XDrawString(dpy, win, gc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1, 
                                  cmd.text.c_str(), cmd.text.length());
        } else if (cmd.type == "rounded_rect") {
          draw_x11_rounded_rect(dpy, win, gc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1,
                                (int)(cmd.x2 - cmd.x1), (int)(cmd.y2 - cmd.y1), (int)cmd.r, true);
        } else if (cmd.type == "rounded_rect_outline") {
          draw_x11_rounded_rect(dpy, win, gc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1,
                                (int)(cmd.x2 - cmd.x1), (int)(cmd.y2 - cmd.y1), (int)cmd.r, false);
        } else if (cmd.type == "blur") {
          blur_x11_region(dpy, win, gc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1,
                          (int)(cmd.x2 - cmd.x1), (int)(cmd.y2 - cmd.y1), cmd.extra_int);
        } else if (cmd.type == "gradient") {
          bool vertical = (cmd.extra_int != 0);
          int w = (int)(cmd.x2 - cmd.x1);
          int h = (int)(cmd.y2 - cmd.y1);
          int steps = vertical ? h : w;
          for (int s = 0; s < steps; ++s) {
            double factor = (double)s / (steps > 1 ? (steps - 1) : 1);
            string step_color = interpolate_color(cmd.color, cmd.color2, factor);
            unsigned long col_grad = get_x11_color_by_name(dpy, screen, step_color);
            XSetForeground(dpy, gc, col_grad);
            if (vertical) {
              XDrawLine(dpy, win, gc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1 + s, 
                                      canvas_x + (int)cmd.x2, canvas_y + (int)cmd.y1 + s);
            } else {
              XDrawLine(dpy, win, gc, canvas_x + (int)cmd.x1 + s, canvas_y + (int)cmd.y1, 
                                      canvas_x + (int)cmd.x1 + s, canvas_y + (int)cmd.y2);
            }
          }
        } else if (cmd.type == "triangle") {
          XPoint pts[3];
          pts[0].x = canvas_x + (int)cmd.x1; pts[0].y = canvas_y + (int)cmd.y1;
          pts[1].x = canvas_x + (int)cmd.x2; pts[1].y = canvas_y + (int)cmd.y2;
          pts[2].x = canvas_x + (int)cmd.x3; pts[2].y = canvas_y + (int)cmd.y3;
          XFillPolygon(dpy, win, gc, pts, 3, Complex, CoordModeOrigin);
        } else if (cmd.type == "triangle_outline") {
          XPoint pts[4];
          pts[0].x = canvas_x + (int)cmd.x1; pts[0].y = canvas_y + (int)cmd.y1;
          pts[1].x = canvas_x + (int)cmd.x2; pts[1].y = canvas_y + (int)cmd.y2;
          pts[2].x = canvas_x + (int)cmd.x3; pts[2].y = canvas_y + (int)cmd.y3;
          pts[3].x = pts[0].x;               pts[3].y = pts[0].y;
          XDrawLines(dpy, win, gc, pts, 4, CoordModeOrigin);
        }
      }
    }
    
    for (auto &lw : layout_widgets) {
      if (lw.type == "label") {
        XSetForeground(dpy, gc, col_fg);
        XDrawString(dpy, win, gc, lw.x, lw.y + 16, lw.text.c_str(), lw.text.length());
      } else if (lw.type == "entry") {
        XSetForeground(dpy, gc, col_muted);
        string label_txt = lw.id + ":";
        XDrawString(dpy, win, gc, lw.x, lw.y - 6, label_txt.c_str(), label_txt.length());
        XSetForeground(dpy, gc, col_input_bg);
        XFillRectangle(dpy, win, gc, lw.x, lw.y, lw.w, lw.h);
        XSetForeground(dpy, gc, active_entry_id == lw.id ? col_btn_bg : col_input_border);
        XDrawRectangle(dpy, win, gc, lw.x, lw.y, lw.w, lw.h);
        XSetForeground(dpy, gc, col_fg);
        string val = gui_entries[lw.id];
        XDrawString(dpy, win, gc, lw.x + 8, lw.y + 19, val.c_str(), val.length());
        if (active_entry_id == lw.id) {
          int cur_x = lw.x + 8 + (font ? XTextWidth(font, val.c_str(), val.length()) : val.length() * 8);
          XDrawLine(dpy, win, gc, cur_x, lw.y + 6, cur_x, lw.y + 24);
        }
      } else if (lw.type == "button") {
        XSetForeground(dpy, gc, lw.hover ? col_btn_hover : col_btn_bg);
        XFillRectangle(dpy, win, gc, lw.x, lw.y, lw.w, lw.h);
        XSetForeground(dpy, gc, col_btn_fg);
        int tw = font ? XTextWidth(font, lw.text.c_str(), lw.text.length()) : lw.text.length() * 8;
        int tx = lw.x + (lw.w - tw) / 2;
        int ty = lw.y + (lw.h + 12) / 2;
        XDrawString(dpy, win, gc, tx, ty, lw.text.c_str(), lw.text.length());
      }
    }
    XFlush(dpy);
  };
  
  while (gui_active) {
    XNextEvent(dpy, &ev);
    if (ev.type == Expose && ev.xexpose.count == 0) {
      draw_all();
    } else if (ev.type == ClientMessage) {
      if ((Atom)ev.xclient.data.l[0] == wmDeleteMessage) {
        gui_active = false;
        break;
      }
    } else if (ev.type == MotionNotify) {
      int mx = ev.xmotion.x;
      int my = ev.xmotion.y;
      bool needs_redraw = false;
      
      for (auto &lw : layout_widgets) {
        if (lw.type == "button") {
          bool current_hover = (mx >= lw.x && mx <= lw.x + lw.w && my >= lw.y && my <= lw.y + lw.h);
          if (current_hover != lw.hover) {
            lw.hover = current_hover;
            needs_redraw = true;
          }
        }
      }
      
      bool hovering_any = false;
      string prev_hovered = hovered_web_element_action;
      if (has_canvas && mx >= canvas_x && mx <= canvas_x + canvas_w &&
          my >= canvas_y && my <= canvas_y + canvas_h) {
        int cx = mx - canvas_x;
        int cy = my - canvas_y;
        for (auto &he : interactive_html_elements) {
          if (cx >= he.x1 && cx <= he.x2 && cy >= he.y1 && cy <= he.y2) {
            hovering_any = true;
            hovered_web_element_action = he.action;
          }
        }
      }
      
      if (hovering_any != hovered_web_element || (hovering_any && hovered_web_element_action != prev_hovered)) {
        hovered_web_element = hovering_any;
        if (!hovering_any) hovered_web_element_action = "";
        XDefineCursor(dpy, win, hovered_web_element ? hand_cursor : arrow_cursor);
        parse_and_render_html(current_active_html);
        needs_redraw = true;
      }
      
      if (needs_redraw) draw_all();
    } else if (ev.type == ButtonPress && ev.xbutton.button == Button1) {
      int mx = ev.xbutton.x;
      int my = ev.xbutton.y;
      if (has_canvas && mx >= canvas_x && mx <= canvas_x + canvas_w &&
          my >= canvas_y && my <= canvas_y + canvas_h) {
        int cx = mx - canvas_x;
        int cy = my - canvas_y;
        bool clicked_input = false;
        for (auto &he : interactive_html_elements) {
          if (cx >= he.x1 && cx <= he.x2 && cy >= he.y1 && cy <= he.y2) {
            if (he.type == "link") {
              string url = he.action;
              if (!url.empty()) {
                string new_html = trim(native_http_request("GET", url));
                current_active_html = new_html;
                parse_and_render_html(current_active_html);
                draw_all();
              }
            } else if (he.type == "button") {
              string action_trimmed = trim(he.action);
              string js_func_name = action_trimmed;
              if (js_func_name.length() > 2 && js_func_name.substr(js_func_name.length() - 2) == "()") {
                js_func_name = js_func_name.substr(0, js_func_name.length() - 2);
              }
              if (js_funcs.count(js_func_name)) {
                execute_javascript(js_func_name);
              } else {
                execute_custom_function(he.action, {});
              }
              draw_all();
            } else if (he.type == "input") {
              active_web_input_id = he.action;
              clicked_input = true;
            }
          }
        }
        if (!clicked_input) {
          active_web_input_id = "";
        }
        parse_and_render_html(current_active_html);
        draw_all();
      }
      for (auto &lw : layout_widgets) {
        if (mx >= lw.x && mx <= lw.x + lw.w && my >= lw.y && my <= lw.y + lw.h) {
          if (lw.type == "entry") {
            active_entry_id = lw.id;
            draw_all();
          } else if (lw.type == "button") {
            string action_trimmed = trim(lw.action);
            string js_func_name = action_trimmed;
            if (js_func_name.length() > 2 && js_func_name.substr(js_func_name.length() - 2) == "()") {
              js_func_name = js_func_name.substr(0, js_func_name.length() - 2);
            }
            if (js_funcs.count(js_func_name)) {
              execute_javascript(js_func_name);
            } else {
              execute_custom_function(lw.action, {});
            }
            draw_all();
          }
        }
      }
    } else if (ev.type == KeyPress) {
      char buf[32];
      KeySym ksym;
      int len = XLookupString(&ev.xkey, buf, sizeof(buf), &ksym, NULL);
      if (!active_entry_id.empty()) {
        string &val = gui_entries[active_entry_id];
        if (ksym == XK_BackSpace) {
          if (!val.empty()) {
            val.pop_back();
            draw_all();
          }
        } else if (ksym == XK_Escape) {
          active_entry_id = "";
          draw_all();
        } else if (len > 0 && buf[0] >= 32 && buf[0] < 127) {
          val += buf[0];
          draw_all();
        }
      } else if (!active_web_input_id.empty()) {
        string &val = gui_entries[active_web_input_id];
        if (ksym == XK_BackSpace) {
          if (!val.empty()) {
            val.pop_back();
            parse_and_render_html(current_active_html);
            draw_all();
          }
        } else if (ksym == XK_Escape) {
          active_web_input_id = "";
          parse_and_render_html(current_active_html);
          draw_all();
        } else if (len > 0 && buf[0] >= 32 && buf[0] < 127) {
          val += buf[0];
          parse_and_render_html(current_active_html);
          draw_all();
        }
      }
    }
  }
  
  if (font) XFreeFont(dpy, font);
  XFreeCursor(dpy, hand_cursor);
  XFreeCursor(dpy, arrow_cursor);
  XFreeGC(dpy, gc);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
#endif
}
