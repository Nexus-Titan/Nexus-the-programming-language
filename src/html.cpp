#include "html.h"
#include "networking.h"
#include <algorithm>

static string get_attr(const string &tag, const string &attr) {
  size_t pos = tag.find(attr + "=");
  if (pos == string::npos) return "";
  
  size_t start = pos + attr.length();
  while (start < tag.length() && (tag[start] == ' ' || tag[start] == '=')) {
    start++;
  }
  
  if (start < tag.length() && (tag[start] == '"' || tag[start] == '\'')) {
    char quote = tag[start];
    size_t end = tag.find(quote, start + 1);
    if (end != string::npos) {
      return tag.substr(start + 1, end - start - 1);
    }
  }
  return "";
}

static unordered_map<string, string> parse_css_style(const string &style_str) {
  unordered_map<string, string> props;
  stringstream ss(style_str);
  string item;
  while (getline(ss, item, ';')) {
    size_t colon = item.find(':');
    if (colon != string::npos) {
      string key = trim(item.substr(0, colon));
      string val = trim(item.substr(colon + 1));
      transform(key.begin(), key.end(), key.begin(), ::tolower);
      props[key] = val;
    }
  }
  
  if (props.count("padding")) {
    string p = props["padding"];
    stringstream pss(p);
    vector<string> parts;
    string part;
    while (pss >> part) parts.push_back(part);
    if (parts.size() == 1) {
      props["padding-top"] = props["padding-bottom"] = props["padding-left"] = props["padding-right"] = parts[0];
    } else if (parts.size() == 2) {
      props["padding-top"] = props["padding-bottom"] = parts[0];
      props["padding-left"] = props["padding-right"] = parts[1];
    } else if (parts.size() == 4) {
      props["padding-top"] = parts[0];
      props["padding-right"] = parts[1];
      props["padding-bottom"] = parts[2];
      props["padding-left"] = parts[3];
    }
  }
  
  if (props.count("margin")) {
    string m = props["margin"];
    stringstream mss(m);
    vector<string> parts;
    string part;
    while (mss >> part) parts.push_back(part);
    if (parts.size() == 1) {
      props["margin-top"] = props["margin-bottom"] = props["margin-left"] = props["margin-right"] = parts[0];
    } else if (parts.size() == 2) {
      props["margin-top"] = props["margin-bottom"] = parts[0];
      props["margin-left"] = props["margin-right"] = parts[1];
    } else if (parts.size() == 4) {
      props["margin-top"] = parts[0];
      props["margin-right"] = parts[1];
      props["margin-bottom"] = parts[2];
      props["margin-left"] = parts[3];
    }
  }

  if (props.count("border")) {
    string b = props["border"];
    stringstream bss(b);
    string width_val = "1px", style_val = "solid", color_val = "#4b5563";
    string part;
    while (bss >> part) {
      if (part.find("px") != string::npos || isdigit(part[0])) {
        width_val = part;
      } else if (part == "solid" || part == "dashed" || part == "dotted" || part == "none") {
        style_val = part;
      } else {
        color_val = part;
      }
    }
    props["border-width"] = width_val;
    props["border-style"] = style_val;
    props["border-color"] = color_val;
  }
  
  return props;
}

void NexusUltraV4::parse_and_render_html(const string &html) {
  interactive_html_elements.clear();
  draw_commands.clear();
  
  int cx = 15;
  int cy = 20;
  bool is_strikethrough = false;
  int table_start_x = 15;
  int cell_w = 110;
  int row_h = 28;
  int line_h = 22;
  int canvas_w_bound = 450;
  
  if (vars.count("GUI_GEOM")) {
    string geom = vars["GUI_GEOM"];
    size_t x_pos = geom.find('x');
    if (x_pos != string::npos) {
      canvas_w_bound = stoi(geom.substr(0, x_pos)) - 40;
    }
  }
  
  size_t i = 0;
  string theme_fg = (vars.count("GUI_COLOR") && vars["GUI_COLOR"] == "white") ? "#111827" : "#ffffff";
  string active_color = theme_fg;
  
  bool is_link = false;
  string link_href = "";
  
  while (i < html.length()) {
    if (html[i] == '<') {
      size_t close = html.find('>', i);
      if (close == string::npos) break;
      string tag = html.substr(i + 1, close - i - 1);
      i = close + 1;
      
      string tag_trimmed = trim(tag);
      string tag_name = tag_trimmed;
      size_t space_pos = tag_trimmed.find(' ');
      if (space_pos != string::npos) {
        tag_name = tag_trimmed.substr(0, space_pos);
      }
      transform(tag_name.begin(), tag_name.end(), tag_name.begin(), ::tolower);
      
      unordered_map<string, string> css;
      string style_attr = get_attr(tag_trimmed, "style");
      if (!style_attr.empty()) {
        css = parse_css_style(style_attr);
      }
      
      if (css.count("color")) {
        active_color = css["color"];
      }
      
      if (tag_name == "script") {
        size_t end_script = string::npos;
        for (size_t k = i; k + 8 < html.length(); ++k) {
          if (html[k] == '<' && html[k+1] == '/' &&
              (html[k+2] == 's' || html[k+2] == 'S') &&
              (html[k+3] == 'c' || html[k+3] == 'C') &&
              (html[k+4] == 'r' || html[k+4] == 'R') &&
              (html[k+5] == 'i' || html[k+5] == 'I') &&
              (html[k+6] == 'p' || html[k+6] == 'P') &&
              (html[k+7] == 't' || html[k+7] == 'T') &&
              html[k+8] == '>') {
            end_script = k;
            break;
          }
        }
        if (end_script != string::npos) {
          string script_content = html.substr(i, end_script - i);
          stringstream ss(script_content);
          string sline;
          string current_func = "";
          vector<string> current_body;
          int brace_count = 0;
          bool inside_func = false;

          while (getline(ss, sline)) {
            string trimmed_sline = trim(sline);
            if (trimmed_sline.empty()) continue;

            if (!inside_func) {
              size_t func_pos = trimmed_sline.find("function ");
              if (func_pos != string::npos) {
                size_t name_start = func_pos + 9;
                while (name_start < trimmed_sline.length() && (trimmed_sline[name_start] == ' ' || trimmed_sline[name_start] == '\t')) {
                  name_start++;
                }
                size_t name_end = trimmed_sline.find_first_of(" \t(", name_start);
                if (name_end != string::npos && name_end > name_start) {
                  current_func = trimmed_sline.substr(name_start, name_end - name_start);
                  inside_func = true;
                  current_body.clear();
                  brace_count = 0;
                  
                  size_t brace_pos = trimmed_sline.find('{', name_end);
                  if (brace_pos != string::npos) {
                    brace_count++;
                    string remainder = trimmed_sline.substr(brace_pos + 1);
                    remainder = trim(remainder);
                    if (!remainder.empty()) {
                      size_t close_pos = remainder.find('}');
                      if (close_pos != string::npos) {
                        brace_count--;
                        remainder = remainder.substr(0, close_pos);
                        remainder = trim(remainder);
                      }
                      if (!remainder.empty()) {
                        current_body.push_back(remainder);
                      }
                      if (brace_count == 0) {
                        js_funcs[current_func] = current_body;
                        inside_func = false;
                        current_func = "";
                      }
                    }
                  }
                }
              }
            } else {
              for (char c : trimmed_sline) {
                if (c == '{') brace_count++;
                else if (c == '}') brace_count--;
              }

              if (brace_count <= 0) {
                string cleaned_line = trimmed_sline;
                size_t close_brace_pos = cleaned_line.find('}');
                if (close_brace_pos != string::npos) {
                  cleaned_line = cleaned_line.substr(0, close_brace_pos);
                }
                cleaned_line = trim(cleaned_line);
                if (!cleaned_line.empty()) {
                  current_body.push_back(cleaned_line);
                }
                
                js_funcs[current_func] = current_body;
                inside_func = false;
                current_func = "";
              } else {
                string cleaned_line = trimmed_sline;
                size_t open_brace_pos = cleaned_line.find('{');
                if (open_brace_pos != string::npos) {
                  cleaned_line = cleaned_line.substr(open_brace_pos + 1);
                }
                cleaned_line = trim(cleaned_line);
                if (!cleaned_line.empty()) {
                  current_body.push_back(cleaned_line);
                }
              }
            }
          }
          i = end_script + 9;
        }
      } else if (tag_name == "h1") {
        active_color = css.count("color") ? css["color"] : "#3b82f6";
        line_h = 32;
        cy += 12;
      } else if (tag_name == "/h1") {
        active_color = theme_fg;
        line_h = 22;
        cy += 35;
        cx = 15;
      } else if (tag_name == "h2" || tag_name == "h3" || tag_name == "h4") {
        active_color = css.count("color") ? css["color"] : "#10b981";
        line_h = 28;
        cy += 10;
      } else if (tag_name == "/h2" || tag_name == "/h3" || tag_name == "/h4") {
        active_color = theme_fg;
        line_h = 22;
        cy += 30;
        cx = 15;
      } else if (tag_name == "strong" || tag_name == "b") {
        active_color = css.count("color") ? css["color"] : "#fbbf24";
      } else if (tag_name == "/strong" || tag_name == "/b") {
        active_color = theme_fg;
      } else if (tag_name == "em" || tag_name == "i") {
        active_color = css.count("color") ? css["color"] : "#a78bfa";
      } else if (tag_name == "/em" || tag_name == "/i") {
        active_color = theme_fg;
      } else if (tag_name == "blockquote") {
        cy += 6;
        cx = 40;
        draw_commands.push_back({"line", 20, (double)cy, 20, (double)(cy + 40), 0, "#4b5563", ""});
      } else if (tag_name == "/blockquote") {
        cy += 15;
        cx = 15;
      } else if (tag_name == "pre" || tag_name == "code") {
        active_color = "#a78bfa";
        if (tag_name == "pre") {
          cy += 8;
          int pre_w = canvas_w_bound - cx - 10;
          draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + pre_w), (double)(cy + 50), 0, "#0b0f19", ""});
          draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + pre_w), (double)(cy + 50), 0, "#312e81", ""});
          cx += 10;
          cy += 10;
        }
      } else if (tag_name == "/pre" || tag_name == "/code") {
        if (tag_name == "/pre") {
          cy += 35;
          cx = 15;
        }
        active_color = theme_fg;
      } else if (tag_name == "mark") {
        draw_commands.push_back({"rect", (double)cx, (double)(cy - 12), (double)(cx + 80), (double)(cy + 4), 0, "#fef08a", ""});
        active_color = "#0f172a";
      } else if (tag_name == "/mark") {
        active_color = theme_fg;
      } else if (tag_name == "u") {
        is_link = true;
      } else if (tag_name == "/u") {
        is_link = false;
      } else if (tag_name == "del" || tag_name == "s" || tag_name == "strike") {
        is_strikethrough = true;
      } else if (tag_name == "/del" || tag_name == "/s" || tag_name == "/strike") {
        is_strikethrough = false;
      } else if (tag_name == "progress" || tag_name == "meter") {
        string val_str = get_attr(tag_trimmed, "value");
        double val = 0.5;
        if (!val_str.empty()) {
          val = stod(val_str);
          if (val > 1.0) val /= 100.0;
        }
        int bar_w = 120;
        int bar_h = 16;
        cy += 4;
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + bar_w), (double)(cy + bar_h), 0, "#1e293b", ""});
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + (int)(bar_w * val)), (double)(cy + bar_h), 0, tag_name == "progress" ? "#10b981" : "#f59e0b", ""});
        draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + bar_w), (double)(cy + bar_h), 0, "#475569", ""});
        cx += bar_w + 15;
      } else if (tag_name == "textarea") {
        string input_id = get_attr(tag_trimmed, "id");
        string input_val = get_attr(tag_trimmed, "value");
        string placeholder = get_attr(tag_trimmed, "placeholder");
        int input_w = 260;
        int input_h = 60;
        cy += 4;
        string box_bg = (vars.count("GUI_COLOR") && vars["GUI_COLOR"] == "white") ? "#ffffff" : "#2d2d2d";
        string border_c = (active_web_input_id == input_id) ? "#3b82f6" : "#4b5563";
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + input_w), (double)(cy + input_h), 0, box_bg, ""});
        draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + input_w), (double)(cy + input_h), 0, border_c, ""});
        string display_val = gui_entries.count(input_id) ? gui_entries[input_id] : input_val;
        bool is_placeholder = display_val.empty();
        string text_to_draw = is_placeholder ? placeholder : display_val;
        string text_color = is_placeholder ? "#6b7280" : theme_fg;
        draw_commands.push_back({"text", (double)(cx + 8), (double)(cy + 18), 0, 0, 0, text_color, text_to_draw});
        interactive_html_elements.push_back({cx, cy, cx + input_w, cy + input_h, "input", input_id});
        cy += input_h + 10;
        cx = 15;
      } else if (tag_name == "select") {
        string select_id = get_attr(tag_trimmed, "id");
        int select_w = 150;
        int select_h = 28;
        cy += 4;
        string box_bg = "#1e293b";
        string border_c = "#3b82f6";
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + select_w), (double)(cy + select_h), 0, box_bg, ""});
        draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + select_w), (double)(cy + select_h), 0, border_c, ""});
        string current_sel = gui_entries.count(select_id) ? gui_entries[select_id] : "Select Option...";
        draw_commands.push_back({"text", (double)(cx + 8), (double)(cy + 18), 0, 0, 0, "#38bdf8", current_sel});
        draw_commands.push_back({"text", (double)(cx + select_w - 18), (double)(cy + 18), 0, 0, 0, "#60a5fa", "▼"});
        interactive_html_elements.push_back({cx, cy, cx + select_w, cy + select_h, "button", select_id + "_toggle"});
        cx += select_w + 15;
      } else if (tag_name == "table") {
        table_start_x = cx;
        cy += 10;
      } else if (tag_name == "/table") {
        cy += 10;
        cx = 15;
      } else if (tag_name == "tr") {
        cx = table_start_x;
        cy += row_h;
      } else if (tag_name == "th") {
        active_color = "#38bdf8";
        string box_bg = "#1e293b";
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + cell_w), (double)(cy + row_h), 0, box_bg, ""});
        draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + cell_w), (double)(cy + row_h), 0, "#475569", ""});
      } else if (tag_name == "/th") {
        cx += cell_w;
      } else if (tag_name == "td") {
        string box_bg = "#0f172a";
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + cell_w), (double)(cy + row_h), 0, box_bg, ""});
        draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + cell_w), (double)(cy + row_h), 0, "#334155", ""});
      } else if (tag_name == "/td") {
        cx += cell_w;
      } else if (tag_name == "li") {
        cy += 5;
        cx = 15;
        draw_commands.push_back({"text", 20, (double)cy + 13, 0, 0, 0, "#fbbf24", "•"});
        cx = 35;
      } else if (tag_name == "/li") {
        cy += 22;
        cx = 15;
      } else if (tag_name == "ul" || tag_name == "ol") {
        cy += 6;
        cx = 15;
      } else if (tag_name == "/ul" || tag_name == "/ol") {
        cy += 12;
        cx = 15;
      } else if (tag_name == "img") {
        string src = get_attr(tag_trimmed, "src");
        string alt = get_attr(tag_trimmed, "alt");
        string filename = "";
        if (!src.empty()) {
          size_t last_slash = src.find_last_of('/');
          filename = (last_slash != string::npos) ? src.substr(last_slash + 1) : src;
        }
        if (filename.length() > 24) filename = filename.substr(0, 21) + "...";
        string img_txt = "[🖼️ " + (alt.empty() ? "Image" : alt) + "]";
        if (img_txt.length() > 24) img_txt = img_txt.substr(0, 21) + "...]";
        
        cy += 5;
        string box_bg = css.count("background-color") ? css["background-color"] : "#2a2a2a";
        string border_c = css.count("border-color") ? css["border-color"] : "#4b5563";
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + 220), (double)(cy + 60), 0, box_bg, ""});
        draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + 220), (double)(cy + 60), 0, border_c, ""});
        draw_commands.push_back({"text", (double)(cx + 15), (double)(cy + 25), 0, 0, 0, "#fbbf24", img_txt});
        if (!filename.empty()) {
          draw_commands.push_back({"text", (double)(cx + 15), (double)(cy + 45), 0, 0, 0, "#9ca3af", filename});
        }
        cy += 75;
        cx = 15;
      } else if (tag_name == "p" || tag_name == "div") {
        cy += 6;
        if (tag_name == "div" && (css.count("background-color") || css.count("border"))) {
          int block_w = css.count("width") ? stoi(css["width"]) : (canvas_w_bound - cx - 10);
          int block_h = css.count("height") ? stoi(css["height"]) : 30;
          string box_bg = css.count("background-color") ? css["background-color"] : "none";
          string border_c = css.count("border-color") ? css["border-color"] : (css.count("border") ? "#4b5563" : "none");
          if (box_bg != "none") {
            draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + block_w), (double)(cy + block_h), 0, box_bg, ""});
          }
          if (border_c != "none") {
            draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + block_w), (double)(cy + block_h), 0, border_c, ""});
          }
        }
      } else if (tag_name == "/p" || tag_name == "/div") {
        cy += 25;
        cx = 15;
        active_color = theme_fg;
      } else if (tag_name == "br") {
        cy += 22;
        cx = 15;
      } else if (tag_name == "hr") {
        cy += 12;
        string border_c = css.count("border-color") ? css["border-color"] : "#444444";
        draw_commands.push_back({"line", 15, (double)cy, (double)(canvas_w_bound - 15), (double)cy, 0, border_c, ""});
        cy += 15;
        cx = 15;
      } else if (tag_name == "a") {
        is_link = true;
        link_href = get_attr(tag_trimmed, "href");
        active_color = css.count("color") ? css["color"] : "#60a5fa";
        if (hovered_web_element && hovered_web_element_action == link_href) {
          active_color = "#93c5fd";
        }
      } else if (tag_name == "/a") {
        is_link = false;
        active_color = theme_fg;
      } else if (tag_name == "input") {
        string input_id = get_attr(tag_trimmed, "id");
        string input_val = get_attr(tag_trimmed, "value");
        string placeholder = get_attr(tag_trimmed, "placeholder");
        string input_type = get_attr(tag_trimmed, "type");
        
        int input_w = css.count("width") ? stoi(css["width"]) : 180;
        int input_h = css.count("height") ? stoi(css["height"]) : 28;
        
        cy += 4;
        string box_bg = css.count("background-color") ? css["background-color"] : ((vars.count("GUI_COLOR") && vars["GUI_COLOR"] == "white") ? "#ffffff" : "#2d2d2d");
        string border_c = (active_web_input_id == input_id) ? "#3b82f6" : (css.count("border-color") ? css["border-color"] : "#4b5563");
        if (hovered_web_element && hovered_web_element_action == input_id) {
          border_c = "#60a5fa";
        }
        
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + input_w), (double)(cy + input_h), 0, box_bg, ""});
        draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + input_w), (double)(cy + input_h), 0, border_c, ""});
        
        string display_val = gui_entries.count(input_id) ? gui_entries[input_id] : input_val;
        bool is_placeholder = display_val.empty();
        string text_to_draw = is_placeholder ? placeholder : display_val;
        
        if (!is_placeholder && input_type == "password") {
          text_to_draw = string(text_to_draw.length(), '*');
        }
        
        string text_color = is_placeholder ? "#6b7280" : (css.count("color") ? css["color"] : theme_fg);
        
        draw_commands.push_back({"text", (double)(cx + 8), (double)(cy + 18), 0, 0, 0, text_color, text_to_draw});
        
        if (active_web_input_id == input_id) {
          int tw = text_to_draw.length() * 8;
          draw_commands.push_back({"line", (double)(cx + 8 + tw), (double)(cy + 5), (double)(cx + 8 + tw), (double)(cy + input_h - 5), 0, "#3b82f6", ""});
        }
        
        interactive_html_elements.push_back({cx, cy, cx + input_w, cy + input_h, "input", input_id});
        cx += input_w + 15;
      } else if (tag_name == "button") {
        string onclick = get_attr(tag_trimmed, "onclick");
        int btn_w = css.count("width") ? stoi(css["width"]) : 120;
        int btn_h = css.count("height") ? stoi(css["height"]) : 32;
        
        size_t end_btn = html.find("</button>", i);
        string btn_text = "Button";
        if (end_btn != string::npos) {
          btn_text = html.substr(i, end_btn - i);
          i = end_btn + 9;
        }
        
        cy += 4;
        string box_bg = css.count("background-color") ? css["background-color"] : "#2563eb";
        string border_c = css.count("border-color") ? css["border-color"] : "#3b82f6";
        
        if (hovered_web_element && hovered_web_element_action == onclick) {
          box_bg = "#3b82f6";
          border_c = "#60a5fa";
        }
        
        string text_color = css.count("color") ? css["color"] : "#ffffff";
        
        draw_commands.push_back({"rect", (double)cx, (double)cy, (double)(cx + btn_w), (double)(cy + btn_h), 0, box_bg, ""});
        draw_commands.push_back({"rect_outline", (double)cx, (double)cy, (double)(cx + btn_w), (double)(cy + btn_h), 0, border_c, ""});
        
        int text_offset_x = (btn_w - (int)btn_text.length() * 8) / 2;
        if (text_offset_x < 4) text_offset_x = 8;
        draw_commands.push_back({"text", (double)(cx + text_offset_x), (double)(cy + 20), 0, 0, 0, text_color, btn_text});
        
        interactive_html_elements.push_back({cx, cy, cx + btn_w, cy + btn_h, "button", onclick});
        cx += btn_w + 15;
      }
    } else {
      size_t next_tag = html.find('<', i);
      string text = html.substr(i, next_tag - i);
      i = (next_tag == string::npos) ? html.length() : next_tag;
      
      text = trim(text);
      if (text.empty()) continue;
      
      int tw = text.length() * 8;
      if (cx + tw > canvas_w_bound) {
        cy += line_h;
        cx = 15;
      }
      
      draw_commands.push_back({"text", (double)cx, (double)cy, 0, 0, 0, active_color, text});
      
      if (is_link) {
        draw_commands.push_back({"line", (double)cx, (double)(cy + 2), (double)(cx + tw), (double)(cy + 2), 0, active_color, ""});
        interactive_html_elements.push_back({cx, cy - 12, cx + tw, cy + 4, "link", link_href});
      }
      if (is_strikethrough) {
        draw_commands.push_back({"line", (double)cx, (double)(cy - 6), (double)(cx + tw), (double)(cy - 6), 0, active_color, ""});
      }
      
      cx += tw + 10;
    }
  }
}
