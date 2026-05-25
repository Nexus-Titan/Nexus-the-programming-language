#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#ifndef _WIN32
#include <unistd.h>
#include <sys/stat.h>
#endif
#ifdef _WIN32
#include <windows.h>
#define popen _popen
#define pclose _pclose
#endif
using namespace std;
class NexusUltraV4 {
public:
  string version = "4.0-ULTRA";
  unordered_map<string, string> vars;
  unordered_map<string, vector<string>> funcs;
  NexusUltraV4() {
    vars["OS"] = get_os();
    vars["VER"] = version;
    vars["ENGINE"] = "CPP-ULTRA";
    vars["USER"] = get_user();
  }
  string get_os() {
#ifdef _WIN32
    return "Windows";
#elif __APPLE__
    return "MacOS";
#else
    return "Linux";
#endif
  }
  string get_user() {
    char *u = getenv("USER");
    if (!u) u = getenv("USERNAME");
    return u ? u : "NexusUser";
  }
  string trim(string s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
  }
  string call_stdlib(string mod, string func, string args_raw) {
    vector<string> args;
    stringstream ss(args_raw);
    string arg;
    while (getline(ss, arg, ',')) args.push_back(resolve_complex(trim(arg)));
    if (mod == "api") {
      if (func == "type") return "PC";
      if (func == "engine") return "CPP";
      if (func == "is_web") return "0";
      if (func == "is_pc") return "1";
    }
    if (mod == "math") {
      if (func == "pi") return "3.1415926535";
      if (args.empty()) return "0";
      double v = stod(args[0]);
      if (func == "sin") return to_string(sin(v));
      if (func == "cos") return to_string(cos(v));
      if (func == "tan") return to_string(tan(v));
      if (func == "sqrt") return to_string(sqrt(v));
      if (func == "abs") return to_string(abs(v));
      if (func == "pow" && args.size() > 1) return to_string(pow(v, stod(args[1])));
    }
    if (mod == "str") {
      if (args.empty()) return "";
      string s = args[0];
      if (func == "len") return to_string(s.length());
      if (func == "upper") { transform(s.begin(), s.end(), s.begin(), ::toupper); return s; }
      if (func == "lower") { transform(s.begin(), s.end(), s.begin(), ::tolower); return s; }
      if (func == "reverse") { reverse(s.begin(), s.end()); return s; }
    }
    if (mod == "sys") {
      if (func == "time") return to_string(time(0));
      if (func == "exit") exit(0);
      if (func == "os") return get_os();
      if (func == "shell" && !args.empty()) {
        char buffer[128];
        string result = "";
        FILE* pipe = popen(args[0].c_str(), "r");
        if (!pipe) return "ERR";
        while (fgets(buffer, 128, pipe)) result += buffer;
        pclose(pipe);
        return result;
      }
    }
    if (mod == "io") {
      if (func == "read" && !args.empty()) {
        ifstream f(args[0]);
        if (!f.is_open()) return "ERR";
        stringstream buffer; buffer << f.rdbuf();
        return buffer.str();
      }
      if (func == "exists" && !args.empty()) {
        struct stat buffer;
        return (stat(args[0].c_str(), &buffer) == 0) ? "1" : "0";
      }
    }
    return "";
  }
  
  bool is_number(const string& s) {
    if (s.empty()) return false;
    try {
      size_t pos;
      stod(s, &pos);
      return pos == s.length();
    } catch (...) {
      return false;
    }
  }

  string resolve_complex(string input) {
    input = trim(input);
    if (input.empty()) return "";

    string ops[] = {"==", "!=", "<=", ">=", "<", ">"};
    for (const string& op : ops) {
      size_t pos = input.find(op);
      if (pos != string::npos) {
        string left = resolve_complex(input.substr(0, pos));
        string right = resolve_complex(input.substr(pos + op.length()));
        
        bool num_cmp = is_number(left) && is_number(right);
        double l_num = num_cmp ? stod(left) : 0;
        double r_num = num_cmp ? stod(right) : 0;

        if (op == "==") return (left == right) ? "1" : "0";
        if (op == "!=") return (left != right) ? "1" : "0";
        
        if (op == "<=") return num_cmp ? (l_num <= r_num ? "1" : "0") : (left <= right ? "1" : "0");
        if (op == ">=") return num_cmp ? (l_num >= r_num ? "1" : "0") : (left >= right ? "1" : "0");
        if (op == "<")  return num_cmp ? (l_num < r_num ? "1" : "0") : (left < right ? "1" : "0");
        if (op == ">")  return num_cmp ? (l_num > r_num ? "1" : "0") : (left > right ? "1" : "0");
      }
    }

    static const regex mod_regex(R"((\w+)\.(\w+)\((.*)\))");
    smatch m;
    if (regex_search(input, m, mod_regex)) {
      return call_stdlib(m[1], m[2], m[3]);
    }
    
    if (input.front() == '"' && input.back() == '"')
      return input.substr(1, input.size() - 2);
      
    if (vars.count(input)) return vars[input];
    
    size_t plus = input.find('+');
    if (plus != string::npos) {
        return resolve_complex(input.substr(0, plus)) + resolve_complex(input.substr(plus + 1));
    }
    
    return input;
  }
  void run(vector<string> lines) {
    for (size_t i = 0; i < lines.size(); ++i) {
      string line = trim(lines[i]);
      if (line.empty() || line[0] == '#') continue;
      smatch m;
      if (line.substr(0, 4) == "set ") {
        size_t eq = line.find('=');
        if (eq != string::npos) {
          string name = trim(line.substr(4, eq - 4));
          vars[name] = resolve_complex(line.substr(eq + 1));
        }
      } else if (line.substr(0, 4) == "out ") {
        cout << "Nexus › " << resolve_complex(line.substr(4)) << endl;
      } else if (line.substr(0, 3) == "if ") {
        string cond_raw = line.substr(3);
        bool cond = resolve_complex(cond_raw) != "0" && !resolve_complex(cond_raw).empty();
        i++; size_t start = i; int d = 1; int else_pos = -1;
        while (d > 0 && i < lines.size()) {
          string l = trim(lines[i]);
          if (l.substr(0, 2) == "if") d++;
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
      } else if (line.substr(0, 5) == "loop ") {
        int count = stoi(resolve_complex(line.substr(5)));
        i++; size_t start = i; int d = 1;
        while (d > 0 && i < lines.size()) {
          string l = trim(lines[i]);
          if (l.substr(0, 5) == "loop") d++;
          else if (l == "end") d--;
          i++;
        }
        vector<string> block;
        for (size_t j = start; j < i - 1; j++) block.push_back(lines[j]);
        for (int c = 0; c < count; c++) run(block);
        i--;
      } else if (line.substr(0, 2) == "fn") {
        size_t lp = line.find('(');
        string name = trim(line.substr(3, lp - 3));
        vector<string> block; i++;
        while (i < lines.size() && trim(lines[i]) != "end") {
          block.push_back(lines[i]); i++;
        }
        funcs[name] = block;
      } else if (line.find("()") != string::npos) {
        string name = line.substr(0, line.find("("));
        if (funcs.count(name)) run(funcs[name]);
      } else if (line.find('.') != string::npos && line.find('(') != string::npos) {
          resolve_complex(line);
      }
    }
  }
};
int main(int argc, char *argv[]) {
  NexusUltraV4 engine;
  if (argc < 2) {
    cout << "Nexus Titan Language v" << engine.version << " (CPP-ULTRA)" << endl;
    cout << "Easier than Python, Faster than Python." << endl;
    return 0;
  }
  ifstream f(argv[1]);
  if (!f.is_open()) { cerr << "Error: " << argv[1] << " not found." << endl; return 1; }
  vector<string> l; string s;
  while (getline(f, s)) l.push_back(s);
  engine.run(l);
  return 0;
}
