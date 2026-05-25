#include "interpreter_core.h"
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#else
extern string show_x11_dialog(const string &type, const string &text);
#endif

void* g_engine_ptr = NULL;

NexusUltraV4::NexusUltraV4() {
  srand(time(0));
  vars["OS"] = get_os();
  vars["VER"] = version;
  vars["ENGINE"] = "CPP-ULTRA";
  vars["USER"] = get_user();
}

string NexusUltraV4::execute_custom_function(const string &name, const vector<string> &args) {
  if (!funcs.count(name))
    return "";
  Function f = funcs[name];

  unordered_map<string, string> saved_vars;
  for (size_t idx = 0; idx < f.params.size(); ++idx) {
    string p = f.params[idx];
    if (vars.count(p))
      saved_vars[p] = vars[p];
    else
      saved_vars[p] = "";

    if (idx < args.size())
      vars[p] = args[idx];
    else
      vars[p] = "";
  }

  run(f.body);

  string ret_val = "";
  if (vars.count("return")) {
    ret_val = vars["return"];
    vars.erase("return");
  }

  for (auto &pair : saved_vars) {
    if (pair.second.empty())
      vars.erase(pair.first);
    else
      vars[pair.first] = pair.second;
  }

  return ret_val;
}

void NexusUltraV4::execute_javascript(const string &func_name) {
  if (!js_funcs.count(func_name)) return;
  const auto &lines = js_funcs[func_name];
  
  for (string line : lines) {
    line = trim(line);
    if (line.empty()) continue;
    
    if (line.compare(0, 6, "alert(") == 0) {
      size_t start = line.find_first_of("\"'");
      size_t end = line.find_last_of("\"'");
      if (start != string::npos && end != string::npos && end > start) {
        string msg = line.substr(start + 1, end - start - 1);
#ifdef _WIN32
        MessageBoxA(NULL, msg.c_str(), "Nexus JS Alert", MB_OK | MB_ICONINFORMATION);
#else
        show_x11_dialog("msg", msg);
#endif
      }
    }
    
    else if (line.compare(0, 12, "console.log(") == 0) {
      size_t start = line.find_first_of("\"'");
      size_t end = line.find_last_of("\"'");
      if (start != string::npos && end != string::npos && end > start) {
        cout << "[JS Console] " << line.substr(start + 1, end - start - 1) << endl;
      }
    }
    
    else if (line.compare(0, 23, "document.getElementById(") == 0) {
      size_t id_start = line.find_first_of("\"'", 23);
      size_t id_end = line.find_first_of("\"'", id_start + 1);
      if (id_start != string::npos && id_end != string::npos) {
        string el_id = line.substr(id_start + 1, id_end - id_start - 1);
        size_t val_pos = line.find(".value", id_end);
        if (val_pos != string::npos) {
          size_t eq = line.find('=', val_pos);
          if (eq != string::npos) {
            size_t val_start = line.find_first_of("\"'", eq);
            size_t val_end = line.find_last_of("\"'");
            if (val_start != string::npos && val_end != string::npos && val_end > val_start) {
              string val = line.substr(val_start + 1, val_end - val_start - 1);
              gui_entries[el_id] = val;
            }
          }
        }
      }
    }
  }
}
