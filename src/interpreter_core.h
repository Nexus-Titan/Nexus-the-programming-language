#pragma once
#include "common.h"

struct Function {
  vector<string> params;
  vector<string> body;
};

class NexusUltraV4 {
public:
  string version = "4.5.0";
  unordered_map<string, string> vars;
  unordered_map<string, Function> funcs;
  unordered_map<string, unordered_map<string, string>> imported_funcs;
  unordered_map<string, unordered_map<string, string>> python_funcs;
  unordered_map<string, vector<string>> js_funcs;
  vector<pair<string, string>> gui_buttons;
  unordered_map<string, string> gui_entries;
  vector<GuiWidget> gui_widgets;
  vector<DrawCommand> draw_commands;
  string gui_port = "0";
  bool gui_active = false;
  bool has_canvas = false;

  NexusUltraV4();
  
  string execute_custom_function(const string &name, const vector<string> &args);
  void execute_javascript(const string &func_name);
  void run(vector<string> lines);
  void pre_scan_functions(const vector<string> &lines);
  string evaluate_expression(string s);
  void handle_add_directive(string block);
  void import_lib(string path);
  void show_gui_window();
  void parse_and_render_html(const string &html);
  bool is_block_start(const string &l);
};

extern void* g_engine_ptr;
