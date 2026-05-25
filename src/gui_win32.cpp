#include "gui.h"
#include "networking.h"

#ifdef _WIN32
struct Win32Widget {
  string type;
  string id;
  string text;
  string action;
  HWND hwnd;
};
static vector<Win32Widget> win32_widgets;
static HWND g_main_hwnd = NULL;
static int canvas_x = 0, canvas_y = 0, canvas_w = 0, canvas_h = 0;

static void blur_win32_region(HDC hdc, int rx, int ry, int rw, int rh, int intensity) {
  if (rw <= 0 || rh <= 0) return;
  if (intensity <= 0) intensity = 3;
  if (intensity > 15) intensity = 15;
  
  HDC memDC = CreateCompatibleDC(hdc);
  HBITMAP hBmp = CreateCompatibleBitmap(hdc, rw, rh);
  SelectObject(memDC, hBmp);
  BitBlt(memDC, 0, 0, rw, rh, hdc, rx, ry, SRCCOPY);
  
  BITMAPINFO bmi = {0};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = rw;
  bmi.bmiHeader.biHeight = -rh;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  
  vector<uint32_t> pixels(rw * rh);
  GetDIBits(memDC, hBmp, 0, rh, pixels.data(), &bmi, DIB_RGB_COLORS);
  vector<uint32_t> original = pixels;
  
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
          
          uint32_t pixel = original[ny * rw + nx];
          int r = (pixel >> 16) & 0xFF;
          int g = (pixel >> 8) & 0xFF;
          int b = pixel & 0xFF;
          
          r_sum += r;
          g_sum += g;
          b_sum += b;
          count++;
        }
      }
      int avg_r = r_sum / count;
      int avg_g = g_sum / count;
      int avg_b = b_sum / count;
      
      pixels[y * rw + x] = (avg_r << 16) | (avg_g << 8) | avg_b;
    }
  }
  
  SetDIBitsToDevice(hdc, rx, ry, rw, rh, 0, 0, 0, rh, pixels.data(), &bmi, DIB_RGB_COLORS);
  DeleteObject(hBmp);
  DeleteDC(memDC);
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  bool is_light = (g_engine_ptr && ((NexusUltraV4*)g_engine_ptr)->vars.count("GUI_COLOR") && 
                   ((NexusUltraV4*)g_engine_ptr)->vars["GUI_COLOR"] == "white");
                   
  switch (msg) {
    case WM_LBUTTONDOWN: {
      int mx = LOWORD(lParam);
      int my = HIWORD(lParam);
      NexusUltraV4* engine = (NexusUltraV4*)g_engine_ptr;
      if (engine && engine->has_canvas && mx >= canvas_x && mx <= canvas_x + canvas_w &&
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
                engine->parse_and_render_html(current_active_html);
              }
            } else if (he.type == "button") {
              string action_trimmed = trim(he.action);
              string js_func_name = action_trimmed;
              if (js_func_name.length() > 2 && js_func_name.substr(js_func_name.length() - 2) == "()") {
                js_func_name = js_func_name.substr(0, js_func_name.length() - 2);
              }
              if (engine->js_funcs.count(js_func_name)) {
                engine->execute_javascript(js_func_name);
              } else {
                engine->execute_custom_function(he.action, {});
              }
            } else if (he.type == "input") {
              active_web_input_id = he.action;
              clicked_input = true;
            }
          }
        }
        if (!clicked_input) {
          active_web_input_id = "";
        }
        engine->parse_and_render_html(current_active_html);
        InvalidateRect(hwnd, NULL, TRUE);
      }
      break;
    }
    case WM_MOUSEMOVE: {
      int mx = LOWORD(lParam);
      int my = HIWORD(lParam);
      NexusUltraV4* engine = (NexusUltraV4*)g_engine_ptr;
      bool hovering_any = false;
      string prev_hovered = hovered_web_element_action;
      if (engine && engine->has_canvas && mx >= canvas_x && mx <= canvas_x + canvas_w &&
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
        engine->parse_and_render_html(current_active_html);
        InvalidateRect(hwnd, NULL, TRUE);
      }
      break;
    }
    case WM_SETCURSOR: {
      if (LOWORD(lParam) == HTCLIENT) {
        if (hovered_web_element) {
          SetCursor(LoadCursor(NULL, IDC_HAND));
          return TRUE;
        } else {
          SetCursor(LoadCursor(NULL, IDC_ARROW));
          return TRUE;
        }
      }
      break;
    }
    case WM_CHAR: {
      NexusUltraV4* engine = (NexusUltraV4*)g_engine_ptr;
      if (engine && !active_web_input_id.empty()) {
        string &val = engine->gui_entries[active_web_input_id];
        char c = (char)wParam;
        if (c == '\b') {
          if (!val.empty()) val.pop_back();
        } else if (c == 27) {
          active_web_input_id = "";
        } else if (c >= 32 && c < 127) {
          val += c;
        }
        engine->parse_and_render_html(current_active_html);
        InvalidateRect(hwnd, NULL, TRUE);
      }
      break;
    }
    case WM_COMMAND: {
      int ctrl_id = LOWORD(wParam);
      int idx = ctrl_id - 1000;
      if (idx >= 0 && idx < (int)win32_widgets.size()) {
        auto &w = win32_widgets[idx];
        if (w.type == "button" && g_engine_ptr) {
          NexusUltraV4* engine = (NexusUltraV4*)g_engine_ptr;
          for (auto &ew : win32_widgets) {
            if (ew.type == "entry") {
              char buf[512];
              GetWindowTextA(ew.hwnd, buf, sizeof(buf));
              engine->gui_entries[ew.id] = string(buf);
            }
          }
          string action_trimmed = trim(w.action);
          string js_func_name = action_trimmed;
          if (js_func_name.length() > 2 && js_func_name.substr(js_func_name.length() - 2) == "()") {
            js_func_name = js_func_name.substr(0, js_func_name.length() - 2);
          }
          if (engine->js_funcs.count(js_func_name)) {
            engine->execute_javascript(js_func_name);
          } else {
            engine->execute_custom_function(w.action, {});
          }
          InvalidateRect(hwnd, NULL, TRUE);
        }
      }
      break;
    }
    case WM_CTLCOLORSTATIC: {
      HDC hdcStatic = (HDC)wParam;
      SetTextColor(hdcStatic, is_light ? RGB(17, 24, 39) : RGB(255, 255, 255));
      SetBkMode(hdcStatic, TRANSPARENT);
      static HBRUSH hBrushMuted = CreateSolidBrush(is_light ? RGB(243, 244, 246) : RGB(18, 18, 18));
      return (LRESULT)hBrushMuted;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      NexusUltraV4* engine = (NexusUltraV4*)g_engine_ptr;
      if (engine && engine->has_canvas) {
        string canvas_hex = engine->vars.count("GUI_CANVAS_COLOR") ? engine->vars["GUI_CANVAS_COLOR"] : (is_light ? "#ffffff" : "#1e1e1e");
        int cr = 30, cg = 30, cb = 30;
        if (canvas_hex.front() == '#' && canvas_hex.length() >= 7) {
          sscanf(canvas_hex.c_str(), "#%02x%02x%02x", &cr, &cg, &cb);
        } else if (canvas_hex == "white") { cr = 255; cg = 255; cb = 255; }
        
        HBRUSH hCanvasBrush = CreateSolidBrush(RGB(cr, cg, cb));
        RECT rCanvas = { canvas_x, canvas_y, canvas_x + canvas_w, canvas_y + canvas_h };
        FillRect(hdc, &rCanvas, hCanvasBrush);
        DeleteObject(hCanvasBrush);
        
        SetBkMode(hdc, TRANSPARENT);
        for (auto &cmd : engine->draw_commands) {
          int r = 255, g = 255, b = 255;
          if (cmd.color.front() == '#' && cmd.color.length() >= 7) {
            sscanf(cmd.color.c_str(), "#%02x%02x%02x", &r, &g, &b);
          } else if (cmd.color == "red") { r = 239; g = 68; b = 68; }
          else if (cmd.color == "blue") { r = 37; g = 99; b = 235; }
          else if (cmd.color == "green") { r = 16; g = 185; b = 129; }
          else if (cmd.color == "yellow") { r = 245; g = 158; b = 11; }
          else if (cmd.color == "cyan") { r = 6; g = 182; b = 212; }
          else if (cmd.color == "white") { r = 255; g = 255; b = 255; }
          else if (cmd.color == "black") { r = 0; g = 0; b = 0; }
          
          HPEN hPen = CreatePen(PS_SOLID, 2, RGB(r, g, b));
          HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));
          HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
          HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
          
          if (cmd.type == "rect") {
            Rectangle(hdc, canvas_x + cmd.x1, canvas_y + cmd.y1, canvas_x + cmd.x2, canvas_y + cmd.y2);
          } else if (cmd.type == "rect_outline") {
            HBRUSH hHollow = (HBRUSH)GetStockObject(NULL_BRUSH);
            SelectObject(hdc, hHollow);
            Rectangle(hdc, canvas_x + cmd.x1, canvas_y + cmd.y1, canvas_x + cmd.x2, canvas_y + cmd.y2);
          } else if (cmd.type == "line") {
            MoveToEx(hdc, canvas_x + cmd.x1, canvas_y + cmd.y1, NULL);
            LineTo(hdc, canvas_x + cmd.x2, canvas_y + cmd.y2);
          } else if (cmd.type == "circle") {
            Ellipse(hdc, canvas_x + cmd.x1 - cmd.r, canvas_y + cmd.y1 - cmd.r, 
                          canvas_x + cmd.x1 + cmd.r, canvas_y + cmd.y1 + cmd.r);
          } else if (cmd.type == "circle_outline") {
            HBRUSH hHollow = (HBRUSH)GetStockObject(NULL_BRUSH);
            SelectObject(hdc, hHollow);
            Ellipse(hdc, canvas_x + cmd.x1 - cmd.r, canvas_y + cmd.y1 - cmd.r, 
                          canvas_x + cmd.x1 + cmd.r, canvas_y + cmd.y1 + cmd.r);
          } else if (cmd.type == "text") {
            SetTextColor(hdc, RGB(r, g, b));
            TextOutA(hdc, canvas_x + cmd.x1, canvas_y + cmd.y1, cmd.text.c_str(), cmd.text.length());
          } else if (cmd.type == "rounded_rect") {
            RoundRect(hdc, canvas_x + cmd.x1, canvas_y + cmd.y1, canvas_x + cmd.x2, canvas_y + cmd.y2, 2 * cmd.r, 2 * cmd.r);
          } else if (cmd.type == "rounded_rect_outline") {
            HBRUSH hHollow = (HBRUSH)GetStockObject(NULL_BRUSH);
            SelectObject(hdc, hHollow);
            RoundRect(hdc, canvas_x + cmd.x1, canvas_y + cmd.y1, canvas_x + cmd.x2, canvas_y + cmd.y2, 2 * cmd.r, 2 * cmd.r);
          } else if (cmd.type == "blur") {
            blur_win32_region(hdc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1,
                              (int)(cmd.x2 - cmd.x1), (int)(cmd.y2 - cmd.y1), cmd.extra_int);
          } else if (cmd.type == "gradient") {
            bool vertical = (cmd.extra_int != 0);
            int w = (int)(cmd.x2 - cmd.x1);
            int h = (int)(cmd.y2 - cmd.y1);
            int steps = vertical ? h : w;
            for (int s = 0; s < steps; ++s) {
              double factor = (double)s / (steps > 1 ? (steps - 1) : 1);
              string step_color = interpolate_color(cmd.color, cmd.color2, factor);
              int gr = 0, gg = 0, gb = 0;
              if (step_color.front() == '#' && step_color.length() >= 7) {
                sscanf(step_color.c_str(), "#%02x%02x%02x", &gr, &gg, &gb);
              }
              HPEN hGradPen = CreatePen(PS_SOLID, 1, RGB(gr, gg, gb));
              HPEN hOldGradPen = (HPEN)SelectObject(hdc, hGradPen);
              if (vertical) {
                MoveToEx(hdc, canvas_x + (int)cmd.x1, canvas_y + (int)cmd.y1 + s, NULL);
                LineTo(hdc, canvas_x + (int)cmd.x2, canvas_y + (int)cmd.y1 + s);
              } else {
                MoveToEx(hdc, canvas_x + (int)cmd.x1 + s, canvas_y + (int)cmd.y1, NULL);
                LineTo(hdc, canvas_x + (int)cmd.x1 + s, canvas_y + (int)cmd.y2);
              }
              SelectObject(hdc, hOldGradPen);
              DeleteObject(hGradPen);
            }
          } else if (cmd.type == "triangle") {
            POINT pts[3];
            pts[0].x = canvas_x + (int)cmd.x1; pts[0].y = canvas_y + (int)cmd.y1;
            pts[1].x = canvas_x + (int)cmd.x2; pts[1].y = canvas_y + (int)cmd.y2;
            pts[2].x = canvas_x + (int)cmd.x3; pts[2].y = canvas_y + (int)cmd.y3;
            Polygon(hdc, pts, 3);
          } else if (cmd.type == "triangle_outline") {
            HBRUSH hHollow = (HBRUSH)GetStockObject(NULL_BRUSH);
            HBRUSH hOldBrush2 = (HBRUSH)SelectObject(hdc, hHollow);
            POINT pts[3];
            pts[0].x = canvas_x + (int)cmd.x1; pts[0].y = canvas_y + (int)cmd.y1;
            pts[1].x = canvas_x + (int)cmd.x2; pts[1].y = canvas_y + (int)cmd.y2;
            pts[2].x = canvas_x + (int)cmd.x3; pts[2].y = canvas_y + (int)cmd.y3;
            Polygon(hdc, pts, 3);
            SelectObject(hdc, hOldBrush2);
          }
          
          SelectObject(hdc, hOldPen);
          SelectObject(hdc, hOldBrush);
          DeleteObject(hPen);
          DeleteObject(hBrush);
        }
      }
      EndPaint(hwnd, &ps);
      break;
    }
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

void NexusUltraV4::show_gui_window() {
  g_engine_ptr = (void*)this;
  HINSTANCE hInst = GetModuleHandle(NULL);
  
  WNDCLASSEXA wc = { 0 };
  wc.cbSize = sizeof(WNDCLASSEXA);
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInst;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = "NexusWindowClass";
  
  bool is_light = (vars.count("GUI_COLOR") && vars["GUI_COLOR"] == "white");
  string bg_hex = vars.count("GUI_BG_COLOR") ? vars["GUI_BG_COLOR"] : (is_light ? "#f3f4f6" : "#121212");
  int br = 18, bg_c = 18, bb = 18;
  if (bg_hex.front() == '#' && bg_hex.length() >= 7) {
    sscanf(bg_hex.c_str(), "#%02x%02x%02x", &br, &bg_c, &bb);
  } else if (bg_hex == "white") { br = 243; bg_c = 244; bb = 246; }
  wc.hbrBackground = CreateSolidBrush(RGB(br, bg_c, bb));
  RegisterClassExA(&wc);
  
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
  
  DWORD win_style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
  if (vars.count("GUI_RESIZABLE") && vars["GUI_RESIZABLE"] == "1") {
    win_style = WS_OVERLAPPEDWINDOW;
  }
  
  string title = vars.count("GUI_TITLE") ? vars["GUI_TITLE"] : "Nexus Window";
  g_main_hwnd = CreateWindowExA(0, "NexusWindowClass", title.c_str(), win_style, win_x, win_y, win_w, win_h, NULL, NULL, hInst, NULL);
                                
  string font_name = vars.count("GUI_FONT_NAME") ? vars["GUI_FONT_NAME"] : "Segoe UI";
  int font_size = vars.count("GUI_FONT_SIZE") ? stoi(vars["GUI_FONT_SIZE"]) : 16;
                                
  HFONT hFont = CreateFontA(font_size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name.c_str());
  HFONT hFontReg = CreateFontA(font_size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name.c_str());
                            
  int margin_x = 25;
  int margin_y = 25;
  int current_y = margin_y;
  
  win32_widgets.clear();
  for (size_t i = 0; i < gui_widgets.size(); ++i) {
    auto &w = gui_widgets[i];
    Win32Widget ww;
    ww.type = w.type;
    ww.id = w.id;
    ww.text = w.text;
    ww.action = w.action;
    
    int ctrl_id = 1000 + i;
    if (w.type == "label") {
      HWND hwndLabel = CreateWindowExA(0, "STATIC", w.text.c_str(), 
                                       WS_CHILD | WS_VISIBLE, 
                                       margin_x, current_y, win_w - 2 * margin_x, 24, 
                                       g_main_hwnd, (HMENU)(INT_PTR)ctrl_id, hInst, NULL);
      SendMessage(hwndLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
      ww.hwnd = hwndLabel;
      current_y += 24 + 10;
    } else if (w.type == "entry") {
      HWND hwndLabel = CreateWindowExA(0, "STATIC", (w.id + ":").c_str(), 
                                       WS_CHILD | WS_VISIBLE, 
                                       margin_x, current_y, win_w - 2 * margin_x, 16, 
                                       g_main_hwnd, NULL, hInst, NULL);
      SendMessage(hwndLabel, WM_SETFONT, (WPARAM)hFontReg, TRUE);
      current_y += 16 + 2;
      
      HWND hwndEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", w.text.c_str(), 
                                      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 
                                      margin_x, current_y, win_w - 2 * margin_x, 30, 
                                      g_main_hwnd, (HMENU)(INT_PTR)ctrl_id, hInst, NULL);
      SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFontReg, TRUE);
      ww.hwnd = hwndEdit;
      current_y += 30 + 15;
    } else if (w.type == "button") {
      HWND hwndButton = CreateWindowExA(0, "BUTTON", w.text.c_str(), 
                                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
                                        margin_x, current_y, win_w - 2 * margin_x, 38, 
                                        g_main_hwnd, (HMENU)(INT_PTR)ctrl_id, hInst, NULL);
      SendMessage(hwndButton, WM_SETFONT, (WPARAM)hFont, TRUE);
      ww.hwnd = hwndButton;
      current_y += 38 + 12;
    }
    win32_widgets.push_back(ww);
  }
  
  if (has_canvas) {
    canvas_x = margin_x;
    canvas_y = current_y;
    canvas_w = win_w - 2 * margin_x;
    canvas_h = win_h - current_y - margin_y - 35;
    if (canvas_h < 100) canvas_h = 150;
  }
  
  ShowWindow(g_main_hwnd, SW_SHOW);
  UpdateWindow(g_main_hwnd);
  
  gui_active = true;
  MSG msg;
  while (gui_active && GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  DeleteObject(hFont);
  DeleteObject(hFontReg);
}
#endif
