#include "parser.h"
#include "stdlib.h"
#include "embedded_libs.h"

bool NexusUltraV4::is_block_start(const string &l) {
  if (l.compare(0, 3, "if ") == 0 || l.compare(0, 3, "if(") == 0 || l == "if") return true;
  if (l.compare(0, 5, "loop ") == 0 || l.compare(0, 5, "loop(") == 0 || l == "loop") return true;
  if (l.compare(0, 6, "while ") == 0 || l.compare(0, 6, "while(") == 0 || l == "while") return true;
  if (l.compare(0, 3, "fn ") == 0 || l.compare(0, 3, "fn(") == 0 || l == "fn") return true;
  return false;
}

void NexusUltraV4::pre_scan_functions(const vector<string> &lines) {
  for (size_t i = 0; i < lines.size(); ++i) {
    string line = trim(lines[i]);
    if (line.length() >= 2 && line.substr(0, 2) == "fn") {
      size_t lp = line.find('(');
      size_t rp = line.find(')', lp);
      if (lp != string::npos && rp != string::npos) {
        string name = trim(line.substr(2, lp - 2));
        string params_raw = line.substr(lp + 1, rp - lp - 1);
        vector<string> params;
        stringstream ss(params_raw);
        string param;
        while (getline(ss, param, ',')) {
          params.push_back(trim(param));
        }
        vector<string> block;
        i++;
        int d = 1;
        while (d > 0 && i < lines.size()) {
          string l = trim(lines[i]);
          if (is_block_start(l)) d++;
          else if (l == "end") d--;
          if (d > 0) block.push_back(lines[i]);
          i++;
        }
        funcs[name] = {params, block};
        i--;
      }
    }
  }
}

string NexusUltraV4::evaluate_expression(string s) {
  s = trim(s);
  if (s.empty()) return "";
  if (s.front() == '"' && s.back() == '"') return s.substr(1, s.length() - 2);

  size_t plus = s.find(" + ");
  if (plus != string::npos) {
    string left = evaluate_expression(s.substr(0, plus));
    string right = evaluate_expression(s.substr(plus + 3));
    try {
      double l_val = stod(left);
      double r_val = stod(right);
      return to_string(l_val + r_val);
    } catch (...) {
      return left + right;
    }
  }
  
  size_t modulo = s.find(" % ");
  if (modulo != string::npos) {
    string left = evaluate_expression(s.substr(0, modulo));
    string right = evaluate_expression(s.substr(modulo + 3));
    return to_string(stoi(left) % stoi(right));
  }

  size_t eq = s.find(" == ");
  if (eq != string::npos) {
    return to_string(evaluate_expression(s.substr(0, eq)) == evaluate_expression(s.substr(eq + 4)));
  }

  size_t neq = s.find(" != ");
  if (neq != string::npos) {
    return to_string(evaluate_expression(s.substr(0, neq)) != evaluate_expression(s.substr(neq + 4)));
  }

  size_t le = s.find(" <= ");
  if (le != string::npos) {
    return to_string(stod(evaluate_expression(s.substr(0, le))) <= stod(evaluate_expression(s.substr(le + 4))));
  }

  size_t ge = s.find(" >= ");
  if (ge != string::npos) {
    return to_string(stod(evaluate_expression(s.substr(0, ge))) >= stod(evaluate_expression(s.substr(ge + 4))));
  }

  size_t lt = s.find(" < ");
  if (lt != string::npos) {
    return to_string(stod(evaluate_expression(s.substr(0, lt))) < stod(evaluate_expression(s.substr(lt + 3))));
  }

  size_t gt = s.find(" > ");
  if (gt != string::npos) {
    return to_string(stod(evaluate_expression(s.substr(0, gt))) > stod(evaluate_expression(s.substr(gt + 3))));
  }

  size_t and_op = s.find(" && ");
  if (and_op != string::npos) {
    string l_val = evaluate_expression(s.substr(0, and_op));
    string r_val = evaluate_expression(s.substr(and_op + 4));
    return to_string((l_val != "0" && !l_val.empty()) && (r_val != "0" && !r_val.empty()));
  }

  size_t dot = s.find('.');
  if (dot != string::npos) {
    size_t lp = s.find('(', dot);
    size_t rp = s.rfind(')');
    if (lp != string::npos && rp == s.length() - 1) {
      string mod = trim(s.substr(0, dot));
      string func = trim(s.substr(dot + 1, lp - dot - 1));
      string args_raw = s.substr(lp + 1, rp - lp - 1);
      vector<string> args;
      int depth = 0;
      bool in_quotes = false;
      string current_arg = "";
      for (size_t idx = 0; idx < args_raw.length(); ++idx) {
        char c = args_raw[idx];
        if (c == '"' && (idx == 0 || args_raw[idx - 1] != '\\')) in_quotes = !in_quotes;
        if (!in_quotes) {
          if (c == '(') depth++;
          else if (c == ')') depth--;
        }
        if (c == ',' && depth == 0 && !in_quotes) {
          args.push_back(evaluate_expression(current_arg));
          current_arg = "";
        } else {
          current_arg += c;
        }
      }
      if (!trim(current_arg).empty() || args.size() > 0) {
        args.push_back(evaluate_expression(current_arg));
      }
      return call_stdlib(mod, func, args);
    }
  }

  if (vars.count(s)) return vars[s];
  return s;
}

void NexusUltraV4::handle_add_directive(string block) {
}

void NexusUltraV4::import_lib(string path) {
  if (path.length() >= 2 && path.front() == '"' && path.back() == '"') {
    path = path.substr(1, path.length() - 2);
  }

  auto embedded = get_embedded_libs();
  if (embedded.count(path)) {
    vector<string> lines;
    stringstream ss(embedded[path]);
    string line;
    while (getline(ss, line)) {
      lines.push_back(line);
    }
    run(lines);
    return;
  }

  string resolved_path = "";
  vector<string> candidates = {path, vars["CWD"] + "/" + path, vars["CWD"] + "/libs/" + path};
  ifstream check;
  for (const string &cand : candidates) {
    check.open(cand);
    if (check.is_open()) {
      resolved_path = cand;
      check.close();
      break;
    }
  }
  if (resolved_path.empty()) return;

  ifstream in(resolved_path);
  vector<string> body;
  string l;
  while (getline(in, l)) body.push_back(l);
  
  unordered_map<string, Function> old_funcs = funcs;
  run(body);
  
  string file_name = path;
  size_t last_slash = file_name.find_last_of("/\\");
  if (last_slash != string::npos) file_name = file_name.substr(last_slash + 1);
  size_t dot = file_name.find_last_of('.');
  string mod_name = dot == string::npos ? file_name : file_name.substr(0, dot);

  for (auto &pair : funcs) {
    if (old_funcs.find(pair.first) == old_funcs.end()) {
      imported_funcs[mod_name][pair.first] = pair.first;
    }
  }
}

void NexusUltraV4::run(vector<string> lines) {
  g_engine_ptr = (void*)this;
  pre_scan_functions(lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    if (vars.count("return")) return;
    if (vars.count("break")) return;

    string line = trim(lines[i]);
    if (line.empty() || line[0] == '#') continue;

    if (line.substr(0, 4) == "+add") {
      i++;
      while (i < lines.size() && trim(lines[i]).find(')') == string::npos) i++;
      continue;
    }
    if (line.substr(0, 4) == "set ") {
      size_t eq = line.find('=');
      if (eq != string::npos) {
        string name = trim(line.substr(4, eq - 4));
        vars[name] = evaluate_expression(line.substr(eq + 1));
      }
      continue;
    }
    if (line.substr(0, 4) == "out ") {
      cout << "Nexus › " << evaluate_expression(line.substr(4)) << endl;
      continue;
    }
    if (line.substr(0, 5) == "wait ") {
      double sec = stod(evaluate_expression(line.substr(5)));
      this_thread::sleep_for(chrono::milliseconds((int)(sec * 1000)));
      continue;
    }
    if (line.substr(0, 6) == "input ") {
      size_t space = line.find(' ', 6);
      if (space != string::npos) {
        string var_name = trim(line.substr(6, space - 6));
        string prompt = evaluate_expression(line.substr(space + 1));
        cout << prompt << ": ";
        string user_in;
        getline(cin, user_in);
        vars[var_name] = trim(user_in);
      }
      continue;
    }
    if (line.substr(0, 7) == "return ") {
      vars["return"] = evaluate_expression(line.substr(7));
      return;
    }
    if (line == "return") {
      vars["return"] = "";
      return;
    }
    if (line == "break") {
      vars["break"] = "1";
      return;
    }
    if (line.substr(0, 7) == "import ") {
      string path = evaluate_expression(line.substr(7));
      import_lib(path);
      continue;
    }
    if (line.substr(0, 3) == "if ") {
      string cond_raw = line.substr(3);
      string cond_val = evaluate_expression(cond_raw);
      bool cond = cond_val != "0" && !cond_val.empty();
      i++;
      size_t start = i;
      int d = 1;
      int else_pos = -1;
      while (d > 0 && i < lines.size()) {
        string l = trim(lines[i]);
        if (is_block_start(l)) d++;
        else if (l == "else" && d == 1) else_pos = i;
        else if (l == "end") d--;
        i++;
      }
      if (cond) {
        vector<string> block;
        for (size_t j = start; j < (else_pos != -1 ? else_pos : i - 1); j++) block.push_back(lines[j]);
        run(block);
      } else if (else_pos != -1) {
        vector<string> block;
        for (size_t j = else_pos + 1; j < i - 1; j++) block.push_back(lines[j]);
        run(block);
      }
      i--;
      continue;
    }
    if (line.substr(0, 5) == "loop ") {
      int count = stoi(evaluate_expression(line.substr(5)));
      i++;
      size_t start = i;
      int d = 1;
      while (d > 0 && i < lines.size()) {
        string l = trim(lines[i]);
        if (is_block_start(l)) d++;
        else if (l == "end") d--;
        i++;
      }
      vector<string> block;
      for (size_t j = start; j < i - 1; j++) block.push_back(lines[j]);
      for (int c = 0; c < count; c++) {
        if (vars.count("return")) break;
        run(block);
        if (vars.count("break")) {
          vars.erase("break");
          break;
        }
      }
      i--;
      continue;
    }
    if (line.substr(0, 6) == "while ") {
      string cond_raw = line.substr(6);
      i++;
      size_t start = i;
      int d = 1;
      while (d > 0 && i < lines.size()) {
        string l = trim(lines[i]);
        if (is_block_start(l)) d++;
        else if (l == "end") d--;
        i++;
      }
      vector<string> block;
      for (size_t j = start; j < i - 1; j++) block.push_back(lines[j]);
      while (true) {
        string cond_val = evaluate_expression(cond_raw);
        if (cond_val == "0" || cond_val.empty()) break;
        run(block);
        if (vars.count("return")) break;
        if (vars.count("break")) {
          vars.erase("break");
          break;
        }
      }
      i--;
      continue;
    }
    if (line.substr(0, 2) == "fn") {
      size_t lp = line.find('(');
      size_t rp = line.find(')', lp);
      string name = trim(line.substr(2, lp - 2));
      string params_raw = line.substr(lp + 1, rp - lp - 1);
      vector<string> params;
      stringstream ss(params_raw);
      string param;
      while (getline(ss, param, ',')) params.push_back(trim(param));
      
      vector<string> block;
      i++;
      int d = 1;
      while (d > 0 && i < lines.size()) {
        string l = trim(lines[i]);
        if (is_block_start(l)) d++;
        else if (l == "end") d--;
        if (d > 0) block.push_back(lines[i]);
        i++;
      }
      funcs[name] = {params, block};
      i--;
      continue;
    }

    size_t lp = line.find('(');
    size_t rp = line.rfind(')');
    if (lp != string::npos && rp == line.length() - 1 && line.substr(0, lp).find('.') == string::npos) {
      string name = trim(line.substr(0, lp));
      if (funcs.count(name)) {
        string args_raw = line.substr(lp + 1, rp - lp - 1);
        vector<string> args;
        int depth = 0;
        bool in_quotes = false;
        string current_arg = "";
        for (size_t idx = 0; idx < args_raw.length(); ++idx) {
          char c = args_raw[idx];
          if (c == '"' && (idx == 0 || args_raw[idx - 1] != '\\')) in_quotes = !in_quotes;
          if (!in_quotes) {
            if (c == '(') depth++;
            else if (c == ')') depth--;
          }
          if (c == ',' && depth == 0 && !in_quotes) {
            args.push_back(evaluate_expression(current_arg));
            current_arg = "";
          } else {
            current_arg += c;
          }
        }
        if (!trim(current_arg).empty() || args.size() > 0) {
          args.push_back(evaluate_expression(current_arg));
        }
        execute_custom_function(name, args);
      }
      continue;
    }
    if (line.find('.') != string::npos && line.find('(') != string::npos) {
      evaluate_expression(line);
    }
  }
}
