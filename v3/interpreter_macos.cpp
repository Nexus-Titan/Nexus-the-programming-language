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
#include <unistd.h>
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

using namespace std;

map<string, string> vars;
map<string, vector<string>> funcs;
vector<pair<string, string>> gui_buttons;

string trim(string s) {
  if (s.empty())
    return s;
  s.erase(0, s.find_first_not_of(" \t\r\n"));
  size_t last = s.find_last_not_of(" \t\r\n");
  if (last != string::npos)
    s.erase(last + 1);
  return s;
}

string to_lower(string s) {
  transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}

string to_upper(string s) {
  transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

string resolve_complex(string input);

string call_module(string mod, string func, string args_raw) {
  vector<string> args;
  stringstream ss(args_raw);
  string arg;
  while (getline(ss, arg, ','))
    args.push_back(resolve_complex(trim(arg)));

  if (mod == "math") {
    if (func == "pi")
      return "3.1415926535";
    if (func == "e")
      return "2.7182818284";
    if (func == "tau")
      return "6.2831853071";
    if (func == "inf")
      return "inf";
    if (func == "nan")
      return "nan";
    if (args.empty())
      return "0";
    double v = stod(args[0]);
    if (func == "sin")
      return to_string(sin(v));
    if (func == "cos")
      return to_string(cos(v));
    if (func == "tan")
      return to_string(tan(v));
    if (func == "asin")
      return to_string(asin(v));
    if (func == "acos")
      return to_string(acos(v));
    if (func == "atan")
      return to_string(atan(v));
    if (func == "sinh")
      return to_string(sinh(v));
    if (func == "cosh")
      return to_string(cosh(v));
    if (func == "tanh")
      return to_string(tanh(v));
    if (func == "sqrt")
      return to_string(sqrt(v));
    if (func == "cbrt")
      return to_string(cbrt(v));
    if (func == "abs")
      return to_string(abs(v));
    if (func == "ceil")
      return to_string(ceil(v));
    if (func == "floor")
      return to_string(floor(v));
    if (func == "round")
      return to_string(round(v));
    if (func == "trunc")
      return to_string(trunc(v));
    if (func == "log")
      return to_string(log(v));
    if (func == "log10")
      return to_string(log10(v));
    if (func == "log2")
      return to_string(log2(v));
    if (func == "exp")
      return to_string(exp(v));
    if (func == "pow" && args.size() > 1)
      return to_string(pow(v, stod(args[1])));
    if (func == "deg")
      return to_string(v * 180.0 / 3.1415926535);
    if (func == "rad")
      return to_string(v * 3.1415926535 / 180.0);
    if (func == "min" && args.size() > 1)
      return to_string(min(v, stod(args[1])));
    if (func == "max" && args.size() > 1)
      return to_string(max(v, stod(args[1])));
    if (func == "fact") {
      double f = 1;
      for (int i = 1; i <= (int)v; i++)
        f *= i;
      return to_string(f);
    }
  }
  if (mod == "str") {
    if (args.empty())
      return "";
    string s = args[0];
    if (func == "len")
      return to_string(s.length());
    if (func == "upper")
      return to_upper(s);
    if (func == "lower")
      return to_lower(s);
    if (func == "trim")
      return trim(s);
    if (func == "reverse") {
      reverse(s.begin(), s.end());
      return s;
    }
    if (func == "cap") {
      if (!s.empty())
        s[0] = toupper(s[0]);
      return s;
    }
    if (func == "at" && args.size() > 1)
      return string(1, s[stoi(args[1])]);
    if (func == "starts" && args.size() > 1)
      return s.find(args[1]) == 0 ? "1" : "0";
    if (func == "ends" && args.size() > 1)
      return s.length() >= args[1].length() &&
                     s.substr(s.length() - args[1].length()) == args[1]
                 ? "1"
                 : "0";
    if (func == "find" && args.size() > 1)
      return to_string(s.find(args[1]));
    if (func == "count" && args.size() > 1) {
      int c = 0;
      size_t p = 0;
      while ((p = s.find(args[1], p)) != string::npos) {
        c++;
        p += args[1].length();
      }
      return to_string(c);
    }
    if (func == "repeat" && args.size() > 1) {
      string r = "";
      for (int i = 0; i < stoi(args[1]); i++)
        r += s;
      return r;
    }
  }
  if (mod == "sys") {
    if (func == "time")
      return to_string(time(0));
    if (func == "os") {
#ifdef _WIN32
      return "Windows";
#elif defined(__APPLE__)
      return "macOS";
#else
      return "Linux";
#endif
    }
    if (func == "info")
      return "Nexus OS | v3.0-NEXUS | CPP-Engine";
    if (func == "user") {
#ifdef _WIN32
      return getenv("USERNAME") ? getenv("USERNAME") : "User";
#else
      return getenv("USER") ? getenv("USER") : "User";
#endif
    }
    if (func == "exit")
      exit(0);
    if (func == "sleep" && !args.empty())
      this_thread::sleep_for(chrono::milliseconds(stoi(args[0])));
  }
  if (mod == "io") {
    if (func == "read" && !args.empty()) {
      ifstream f(args[0]);
      if (!f.is_open())
        return "Error";
      string l, ct = "";
      while (getline(f, l))
        ct += l + " | ";
      return ct.empty() ? "Empty" : ct;
    }
    if (func == "exists" && !args.empty()) {
      ifstream f(args[0]);
      return f.good() ? "1" : "0";
    }
    if (func == "size" && !args.empty()) {
      ifstream f(args[0], ios::binary | ios::ate);
      return to_string(f.tellg());
    }
    if (func == "remove" && !args.empty())
      return to_string(remove(args[0].c_str()) == 0);
  }
  if (mod == "gui") {
#ifndef __APPLE__
    string pref = "zenity ";
#endif
    if (func == "msg" && !args.empty())
#ifdef __APPLE__
      system(("osascript -e 'display alert \"" + args[0] + "\"' 2>/dev/null").c_str());
#else
      system((pref + "--info --text=\"" + args[0] + "\" 2>/dev/null").c_str());
#endif
    if (func == "warn" && !args.empty())
#ifdef __APPLE__
      system(("osascript -e 'display alert \"" + args[0] + "\" as warning' 2>/dev/null").c_str());
#else
      system((pref + "--warning --text=\"" + args[0] + "\" 2>/dev/null").c_str());
#endif
    if (func == "err" && !args.empty())
#ifdef __APPLE__
      system(("osascript -e 'display alert \"" + args[0] + "\" as critical' 2>/dev/null").c_str());
#else
      system((pref + "--error --text=\"" + args[0] + "\" 2>/dev/null").c_str());
#endif
    if (func == "ask" && !args.empty())
#ifdef __APPLE__
      return to_string(
          system(("osascript -e 'button returned of (display dialog \"" + args[0] + "\" buttons {\"No\", \"Yes\"} default button \"Yes\")' | grep -q '^Yes$'").c_str()) == 0);
#else
      return to_string(
          system((pref + "--question --text=\"" + args[0] + "\" 2>/dev/null")
                     .c_str()) == 0);
#endif
    if (func == "button" && args.size() >= 2)
      gui_buttons.push_back({args[0], args[1]});
  }

  if (mod == "admin") {
    if (func == "is_admin") {
      return to_string(geteuid() == 0);
    }
  }
  if (mod == "browser") {
    if (func == "open" && !args.empty()) {
      system(("open " + args[0]).c_str());
      return "1";
    }
  }
  if (mod == "zip") {
    if (func == "extract" && args.size() >= 2) {
      system(("unzip -o " + args[0] + " -d " + args[1]).c_str());
      return "1";
    }
  }
  return "";
}

string resolve_complex(string input) {
  input = trim(input);
  if (input.empty())
    return "";

  smatch m;
  if (regex_search(input, m, regex(R"((\w+)\.(\w+)\((.*)\))"))) {
    return call_module(m[1], m[2], m[3]);
  }

  if (input.front() == '"' && input.back() == '"')
    return input.substr(1, input.size() - 2);

  size_t plus = input.find('+');
  if (plus != string::npos) {
    return resolve_complex(input.substr(0, plus)) +
           resolve_complex(input.substr(plus + 1));
  }

  if (vars.count(input))
    return vars[input];
  return input;
}

void runNexus(vector<string> lines);

void show_cpp_dashboard() {
  while (true) {
#ifdef __APPLE__
    string cmd = "osascript -e 'choose from list {";
    for (size_t i = 0; i < gui_buttons.size(); ++i) {
      cmd += "\"" + gui_buttons[i].first + "\", ";
    }
    cmd += "\"Exit\"} with title \"Nexus Dashboard\" with prompt \"Actions\"' 2>/dev/null";
#else
    string cmd =
        "zenity --list --title=\"Nexus Dashboard\" --column=\"Actions\" ";
    for (auto &btn : gui_buttons)
      cmd += "\"" + btn.first + "\" ";
    cmd += "\"Exit\" 2>/dev/null";
#endif
    char buf[128];
    string choice = "";
    FILE *p = popen(cmd.c_str(), "r");
    if (p) {
      while (fgets(buf, 128, p))
        choice += buf;
      pclose(p);
    }
    choice = trim(choice);
#ifdef __APPLE__
    if (choice == "Exit" || choice == "" || choice == "false")
#else
    if (choice == "Exit" || choice == "")
#endif
      break;
    for (auto &btn : gui_buttons)
      if (btn.first == choice)
        runNexus(funcs[btn.second]);
  }
}

void runNexus(vector<string> lines) {
  for (size_t i = 0; i < lines.size(); ++i) {
    string line = trim(lines[i]);
    if (line.empty() || line[0] == '#')
      continue;
    smatch m;
    if (regex_search(line, m, regex(R"(set\s+(\w+)\s*=\s*(.*))")))
      vars[m[1]] = resolve_complex(m[2]);
    else if (regex_search(line, m, regex(R"(out\s+(.*))")))
      cout << "Nexus › " << resolve_complex(m[1]) << endl;
    else if (regex_search(line, m, regex(R"(wait\s+(.*))"))) {
      int sec = stoi(resolve_complex(m[1]));
      this_thread::sleep_for(chrono::seconds(sec));
    } else if (regex_search(line, m, regex(R"(input\s+(\w+)\s+(.*))"))) {
      string pr = resolve_complex(m[2]);
#ifdef __APPLE__
      string cmd = "osascript -e 'text returned of (display dialog \"" + pr + "\" default answer \"\")' 2>/dev/null";
#else
      string cmd = "zenity --entry --text=\"" + pr + "\" 2>/dev/null";
#endif
      char buf[128];
      string r = "";
      FILE *p = popen(cmd.c_str(), "r");
      if (p) {
        while (fgets(buf, 128, p))
          r += buf;
        pclose(p);
      }
      vars[m[1]] = trim(r);
    } else if (line.find("gui.run()") != string::npos)
      show_cpp_dashboard();
    else if (line.find(".") != string::npos && line.find("(") != string::npos) {
      resolve_complex(line);
    } else if (line.substr(0, 2) == "fn") {
      string n = trim(line.substr(3, line.find("(") - 3));
      vector<string> b;
      i++;
      while (i < lines.size() && trim(lines[i]) != "end") {
        b.push_back(lines[i]);
        i++;
      }
      funcs[n] = b;
    } else if (line.find("()") != string::npos) {
      string n = line.substr(0, line.find("("));
      if (funcs.count(n))
        runNexus(funcs[n]);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Nexus Titan Language v3.0-NEXUS (CPP)" << endl;
    return 0;
  }
  ifstream f(argv[1]);
  if (!f.is_open()) {
    cout << "Error: Could not open file " << argv[1] << endl;
    return 1;
  }
  vector<string> l;
  string s;
  while (getline(f, s))
    l.push_back(s);
  runNexus(l);
  return 0;
}