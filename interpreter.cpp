#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <direct.h>
#include <io.h>
#include <windows.h>
#define popen _popen
#define pclose _pclose
#endif

using namespace std;

string base64_encode(const string &in);
string base64_decode(const string &in);

struct Function {
  vector<string> params;
  vector<string> body;
};

class NexusUltraV4 {
public:
  string version = "4.1.3";
  unordered_map<string, string> vars;
  unordered_map<string, Function> funcs;
  unordered_map<string, unordered_map<string, string>> imported_funcs;
  unordered_map<string, unordered_map<string, string>> python_funcs;
  vector<pair<string, string>> gui_buttons;
  unordered_map<string, string> gui_entries;

  NexusUltraV4() {
    srand(time(0));
    vars["OS"] = get_os();
    vars["VER"] = version;
    vars["ENGINE"] = "CPP-ULTRA";
    vars["USER"] = get_user();
    vars["CWD"] = get_cwd_str();
    vars["PI"] = "3.141592653589793";
    vars["E"] = "2.718281828459045";
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
    if (!u)
      u = getenv("USERNAME");
    return u ? u : "NexusUser";
  }

  string get_cwd_str() {
    char buff[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(buff, FILENAME_MAX);
#else
    getcwd(buff, FILENAME_MAX);
#endif
    return string(buff);
  }

  string trim(string s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos)
      return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
  }

  bool is_number(const string &s) {
    if (s.empty())
      return false;
    try {
      size_t pos;
      stod(s, &pos);
      return pos == s.length();
    } catch (...) {
      return false;
    }
  }

  vector<double> parse_list(string s) {
    vector<double> res;
    s = trim(s);
    if (s.length() >= 2 && s.front() == '[' && s.back() == ']') {
      s = s.substr(1, s.length() - 2);
    }
    stringstream ss(s);
    string item;
    while (getline(ss, item, ',')) {
      item = trim(item);
      if (item.length() >= 2 && item.front() == '"' && item.back() == '"') {
        item = item.substr(1, item.length() - 2);
      }
      if (!item.empty()) {
        try {
          res.push_back(stod(item));
        } catch (...) {
        }
      }
    }
    return res;
  }

  vector<string> parse_string_list(string s) {
    vector<string> res;
    s = trim(s);
    if (s.length() >= 2 && s.front() == '[' && s.back() == ']') {
      s = s.substr(1, s.length() - 2);
    }
    stringstream ss(s);
    string item;
    while (getline(ss, item, ',')) {
      item = trim(item);
      if (item.length() >= 2 && item.front() == '"' && item.back() == '"') {
        item = item.substr(1, item.length() - 2);
      }
      res.push_back(item);
    }
    return res;
  }

  string run_shell(string cmd) {
    char buffer[128];
    string result = "";
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
      return "ERR";
    while (fgets(buffer, 128, pipe)) {
      result += buffer;
    }
    pclose(pipe);
    return trim(result);
  }

  unordered_map<string, string> parse_json_to_map(string json_str) {
    unordered_map<string, string> res;
    json_str = trim(json_str);
    if (json_str.empty() || json_str.front() != '{' || json_str.back() != '}')
      return res;

    bool in_quotes = false;
    string current_key = "";
    string current_val = "";
    bool reading_key = true;
    int depth = 0;

    for (size_t i = 1; i < json_str.length() - 1; ++i) {
      char c = json_str[i];
      if (c == '"' && json_str[i - 1] != '\\') {
        in_quotes = !in_quotes;
        continue;
      }

      if (in_quotes) {
        if (reading_key)
          current_key += c;
        else
          current_val += c;
      } else {
        if (c == ':') {
          reading_key = false;
        } else if (c == ',' && depth == 0) {
          res[trim(current_key)] = trim(current_val);
          current_key = "";
          current_val = "";
          reading_key = true;
        } else {
          if (c == '{' || c == '[')
            depth++;
          else if (c == '}' || c == ']')
            depth--;
          if (!reading_key) {
            current_val += c;
          }
        }
      }
    }
    if (!current_key.empty()) {
      res[trim(current_key)] = trim(current_val);
    }
    return res;
  }

  string call_stdlib(string mod, string func, const vector<string> &args) {
    if (vars.count(mod)) {
      auto m = parse_json_to_map(vars[mod]);
      if (m.count(func))
        return m[func];
    }

    if (imported_funcs.count(mod) && imported_funcs[mod].count(func)) {
      return execute_custom_function(imported_funcs[mod][func], args);
    }

    if (python_funcs.count(mod) && python_funcs[mod].count(func)) {
      string py_file = python_funcs[mod][func];
      string dir_path = ".";
      size_t slash = py_file.find_last_of("/\\");
      if (slash != string::npos)
        dir_path = py_file.substr(0, slash);

      string args_str = "";
      for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0)
          args_str += ", ";
        if (is_number(args[i])) {
          args_str += args[i];
        } else {
          args_str += "'" + args[i] + "'";
        }
      }

      string cmd = "python3 -c \"import sys; sys.path.append('" + dir_path +
                   "'); sys.path.append('libs'); import " + mod + "; print(" +
                   mod + "." + func + "(" + args_str + "))\" 2>/dev/null";
      return run_shell(cmd);
    }

    if (mod == "api") {
      if (func == "type")
        return "PC";
      if (func == "engine")
        return "CPP";
      if (func == "version")
        return version;
      if (func == "is_web")
        return "0";
      if (func == "is_pc")
        return "1";
      if (func == "platform")
        return get_os();
      if (func == "call" && args.size() >= 2) {
        cout << "ULTRA API Bridge › " << args[0] << "::" << args[1] << "()"
             << endl;
        return "1";
      }
      if (func == "sync") {
        string data = args.empty() ? "" : args[0];
        cout << "API Sync › " << data << endl;
        return "1";
      }
    }

    if (mod == "math") {
      if (func == "pi")
        return "3.141592653589793";
      if (func == "e")
        return "2.718281828459045";
      if (func == "tau")
        return "6.283185307179586";
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
      if (func == "atan2" && args.size() > 1)
        return to_string(atan2(v, stod(args[1])));
      if (func == "sinh")
        return to_string(sinh(v));
      if (func == "cosh")
        return to_string(cosh(v));
      if (func == "tanh")
        return to_string(tanh(v));
      if (func == "asinh")
        return to_string(log(v + sqrt(v * v + 1)));
      if (func == "acosh")
        return to_string(log(v + sqrt(v * v - 1)));
      if (func == "atanh")
        return to_string(0.5 * log((1 + v) / (1 - v)));
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
        return to_string(v * 180.0 / 3.141592653589793);
      if (func == "rad")
        return to_string(v * 3.141592653589793 / 180.0);
      if (func == "min" && args.size() > 1)
        return to_string(min(v, stod(args[1])));
      if (func == "max" && args.size() > 1)
        return to_string(max(v, stod(args[1])));
      if (func == "hypot" && args.size() > 1)
        return to_string(hypot(v, stod(args[1])));

      if (func == "gcd" && args.size() > 1) {
        long long a = abs((long long)v);
        long long b = abs((long long)stod(args[1]));
        while (b) {
          long long t = b;
          b = a % b;
          a = t;
        }
        return to_string(a);
      }
      if (func == "lcm" && args.size() > 1) {
        long long a = abs((long long)v);
        long long b = abs((long long)stod(args[1]));
        if (a == 0 || b == 0)
          return "0";
        long long original_a = a, original_b = b;
        while (b) {
          long long t = b;
          b = a % b;
          a = t;
        }
        return to_string((original_a * original_b) / a);
      }
      if (func == "mod" && args.size() > 1)
        return to_string(fmod(v, stod(args[1])));
      if (func == "is_nan")
        return isnan(v) ? "1" : "0";
      if (func == "is_inf")
        return isinf(v) ? "1" : "0";

      if (func == "fact") {
        double f = 1;
        for (int i = 1; i <= (int)v; i++)
          f *= i;
        return to_string(f);
      }
      if (func == "perm" && args.size() > 1) {
        double k = stod(args[1]);
        if (v < 0 || k < 0 || v < k)
          return "0";
        double f = 1;
        for (int i = 0; i < (int)k; i++)
          f *= (v - i);
        return to_string(f);
      }
      if (func == "comb" && args.size() > 1) {
        double k = stod(args[1]);
        if (v < 0 || k < 0 || v < k)
          return "0";
        if (k > v - k)
          k = v - k;
        double f = 1;
        for (int i = 1; i <= (int)k; i++) {
          f *= (v - i + 1);
          f /= i;
        }
        return to_string(f);
      }
      if (func == "remainder" && args.size() > 1)
        return to_string(remainder(v, stod(args[1])));
      if (func == "erf")
        return to_string(erf(v));
      if (func == "erfc")
        return to_string(erfc(v));
      if (func == "gamma")
        return to_string(tgamma(v));
      if (func == "lgamma")
        return to_string(lgamma(v));
      if (func == "is_close" && args.size() > 1)
        return abs(v - stod(args[1])) <= 1e-9 ? "1" : "0";
      if (func == "fsum") {
        double sum = 0;
        for (double x : parse_list(args[0]))
          sum += x;
        return to_string(sum);
      }
      if (func == "prod") {
        double pr = 1;
        for (double x : parse_list(args[0]))
          pr *= x;
        return to_string(pr);
      }
      if (func == "dist" && args.size() > 1) {
        auto l1 = parse_list(args[0]);
        auto l2 = parse_list(args[1]);
        if (l1.size() != l2.size() || l1.empty())
          return "0";
        double sum = 0;
        for (size_t i = 0; i < l1.size(); ++i)
          sum += pow(l1[i] - l2[i], 2);
        return to_string(sqrt(sum));
      }
    }

    if (mod == "str") {
      if (args.empty())
        return "";
      string s = args[0];

      if (func == "len") {
        s = trim(s);
        if (s.length() >= 2 && s.front() == '[' && s.back() == ']') {
          return to_string(parse_string_list(s).size());
        }
        return to_string(s.length());
      }
      if (func == "upper") {
        transform(s.begin(), s.end(), s.begin(), ::toupper);
        return s;
      }
      if (func == "lower") {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
      }
      if (func == "cap") {
        if (!s.empty()) {
          transform(s.begin(), s.end(), s.begin(), ::tolower);
          s[0] = toupper(s[0]);
        }
        return s;
      }
      if (func == "trim")
        return trim(s);
      if (func == "ltrim") {
        size_t first = s.find_first_not_of(" \t\r\n");
        return (first == string::npos) ? "" : s.substr(first);
      }
      if (func == "rtrim") {
        size_t last = s.find_last_not_of(" \t\r\n");
        return (last == string::npos) ? "" : s.substr(0, last + 1);
      }
      if (func == "reverse") {
        reverse(s.begin(), s.end());
        return s;
      }

      if (func == "replace" && args.size() >= 3) {
        string from = args[1], to = args[2];
        size_t start_pos = 0;
        while ((start_pos = s.find(from, start_pos)) != string::npos) {
          s.replace(start_pos, from.length(), to);
          start_pos += to.length();
        }
        return s;
      }
      if (func == "repeat" && args.size() > 1) {
        string r = "";
        int count = stoi(args[1]);
        for (int i = 0; i < count; i++)
          r += s;
        return r;
      }
      if (func == "split") {
        string delim = args.size() > 1 ? args[1] : " ";
        string r = "[";
        size_t pos = 0;
        bool first = true;
        while ((pos = s.find(delim)) != string::npos) {
          if (!first)
            r += ", ";
          r += "\"" + s.substr(0, pos) + "\"";
          s.erase(0, pos + delim.length());
          first = false;
        }
        if (!first)
          r += ", ";
        r += "\"" + s + "\"";
        r += "]";
        return r;
      }
      if (func == "join" && args.size() > 1) {
        string delim = args[0];
        string list_str = args[1];
        if (list_str.length() >= 2 && list_str.front() == '[' &&
            list_str.back() == ']') {
          list_str = list_str.substr(1, list_str.length() - 2);
        }
        stringstream ss(list_str);
        string item;
        string r = "";
        bool first = true;
        while (getline(ss, item, ',')) {
          item = trim(item);
          if (item.length() >= 2 && item.front() == '"' && item.back() == '"') {
            item = item.substr(1, item.length() - 2);
          }
          if (!first)
            r += delim;
          r += item;
          first = false;
        }
        return r;
      }
      if (func == "slice" && args.size() > 1) {
        int start = stoi(args[1]);
        int end = args.size() > 2 ? stoi(args[2]) : (int)s.length();
        if (start < 0)
          start = (int)s.length() + start;
        if (end < 0)
          end = (int)s.length() + end;
        if (start < 0)
          start = 0;
        if (end < 0)
          end = 0;
        if (start > (int)s.length())
          start = (int)s.length();
        if (end > (int)s.length())
          end = (int)s.length();
        if (start > end)
          return "";
        return s.substr(start, end - start);
      }
      if (func == "at" && args.size() > 1) {
        s = trim(s);
        if (s.length() >= 2 && s.front() == '[' && s.back() == ']') {
          auto items = parse_string_list(s);
          int idx = stoi(args[1]);
          return (idx >= 0 && idx < (int)items.size()) ? items[idx] : "";
        }
        size_t idx = stoi(args[1]);
        return idx < s.length() ? string(1, s[idx]) : "";
      }
      if (func == "find" && args.size() > 1)
        return to_string(s.find(args[1]));
      if (func == "rfind" && args.size() > 1)
        return to_string(s.rfind(args[1]));
      if (func == "starts" && args.size() > 1)
        return s.find(args[1]) == 0 ? "1" : "0";
      if (func == "ends" && args.size() > 1)
        return s.length() >= args[1].length() &&
                       s.substr(s.length() - args[1].length()) == args[1]
                   ? "1"
                   : "0";
      if (func == "isnum") {
        bool is_num = !s.empty();
        for (char c : s)
          if (!isdigit(c) && c != '.')
            is_num = false;
        return is_num ? "1" : "0";
      }
      if (func == "isalpha") {
        bool is_a = !s.empty();
        for (char c : s)
          if (!isalpha(c))
            is_a = false;
        return is_a ? "1" : "0";
      }
      if (func == "isalnum") {
        bool is_an = !s.empty();
        for (char c : s)
          if (!isalnum(c))
            is_an = false;
        return is_an ? "1" : "0";
      }
      if (func == "isspace") {
        bool is_s = !s.empty();
        for (char c : s)
          if (!isspace(c))
            is_s = false;
        return is_s ? "1" : "0";
      }
      if (func == "islower") {
        bool is_l = !s.empty();
        for (char c : s)
          if (isalpha(c) && !islower(c))
            is_l = false;
        return is_l ? "1" : "0";
      }
      if (func == "isupper") {
        bool is_u = !s.empty();
        for (char c : s)
          if (isalpha(c) && !isupper(c))
            is_u = false;
        return is_u ? "1" : "0";
      }
      if (func == "hex") {
        stringstream ss;
        ss << std::hex << stoi(s);
        return "0x" + ss.str();
      }
      if (func == "bin") {
        int n = stoi(s);
        string r = "";
        while (n > 0) {
          r = (n % 2 == 0 ? "0" : "1") + r;
          n /= 2;
        }
        return r.empty() ? "0" : r;
      }
      if (func == "ascii")
        return s;
      if (func == "chr")
        return string(1, (char)stoi(s));
      if (func == "ord")
        return s.empty() ? "0" : to_string((int)s[0]);

      if (func == "count" && args.size() > 1) {
        string q = args[1];
        if (q.empty())
          return "0";
        int c = 0;
        size_t pos = 0;
        while ((pos = s.find(q, pos)) != string::npos) {
          c++;
          pos += q.length();
        }
        return to_string(c);
      }
      if (func == "zfill" && args.size() > 1) {
        int n = stoi(args[1]);
        if ((int)s.length() >= n)
          return s;
        return string(n - s.length(), '0') + s;
      }
      if (func == "swap") {
        for (char &c : s) {
          if (islower(c))
            c = toupper(c);
          else if (isupper(c))
            c = tolower(c);
        }
        return s;
      }
      if (func == "title") {
        bool cap = true;
        for (char &c : s) {
          if (isspace(c))
            cap = true;
          else if (cap) {
            c = toupper(c);
            cap = false;
          } else
            c = tolower(c);
        }
        return s;
      }
      if (func == "partition" && args.size() > 1) {
        string d = args[1];
        size_t p = s.find(d);
        if (p == string::npos)
          return "[\"" + s + "\", \"\", \"\"]";
        return "[\"" + s.substr(0, p) + "\", \"" + d + "\", \"" +
               s.substr(p + d.length()) + "\"]";
      }
      if (func == "rpartition" && args.size() > 1) {
        string d = args[1];
        size_t p = s.rfind(d);
        if (p == string::npos)
          return "[\"\", \"\", \"" + s + "\"]";
        return "[\"" + s.substr(0, p) + "\", \"" + d + "\", \"" +
               s.substr(p + d.length()) + "\"]";
      }
    }

    if (mod == "sys") {
      if (func == "time")
        return to_string(time(0));
      if (func == "os")
        return get_os();
      if (func == "ver")
        return "10.15";
      if (func == "arch")
        return "x86_64";
      if (func == "user")
        return get_user();
      if (func == "cwd")
        return get_cwd_str();
      if (func == "setcwd" && !args.empty()) {
#ifdef _WIN32
        return _chdir(args[0].c_str()) == 0 ? "1" : "0";
#else
        return chdir(args[0].c_str()) == 0 ? "1" : "0";
#endif
      }
      if (func == "env" && !args.empty()) {
        char *v = getenv(args[0].c_str());
        return v ? v : "";
      }
      if (func == "exit")
        exit(args.empty() ? 0 : stoi(args[0]));
      if (func == "pid") {
#ifdef _WIN32
        return to_string(GetCurrentProcessId());
#else
        return to_string(getpid());
#endif
      }
      if (func == "cpu_count")
        return "8";
      if (func == "shell" && !args.empty())
        return run_shell(args[0]);
      if (func == "run" && !args.empty()) {
        int code = system(args[0].c_str());
#ifndef _WIN32
        if (WIFEXITED(code))
          code = WEXITSTATUS(code);
#endif
        return to_string(code);
      }
      if (func == "sleep" && !args.empty()) {
        this_thread::sleep_for(
            chrono::milliseconds((int)(stod(args[0]) * 1000)));
        return "1";
      }
      if (func == "info")
        return "Nexus OS | v" + version + " | CPP-Engine";
      if (func == "mem_total")
        return "17179869184";
      if (func == "mem_used")
        return "8589934592";
      if (func == "mem_free")
        return "8589934592";
      if (func == "setenv" && args.size() >= 2) {
#ifdef _WIN32
        return _putenv_s(args[0].c_str(), args[1].c_str()) == 0 ? "1" : "0";
#else
        return setenv(args[0].c_str(), args[1].c_str(), 1) == 0 ? "1" : "0";
#endif
      }
      if (func == "delenv" && !args.empty()) {
#ifdef _WIN32
        return _putenv_s(args[0].c_str(), "") == 0 ? "1" : "0";
#else
        return unsetenv(args[0].c_str()) == 0 ? "1" : "0";
#endif
      }
      if (func == "whoami")
        return run_shell("whoami");
      if (func == "clear") {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        return "1";
      }
    }

    if (mod == "admin") {
      if (func == "is_admin") {
#ifdef _WIN32
        return "0";
#else
        return geteuid() == 0 ? "1" : "0";
#endif
      }
      if (func == "run_admin" && !args.empty()) {
        return to_string(system(("sudo " + args[0]).c_str()) == 0);
      }
      if (func == "sudo" && !args.empty()) {
        return to_string(system(("sudo " + args[0]).c_str()) == 0);
      }
      if (func == "get_uid") {
#ifdef _WIN32
        return "-1";
#else
        return to_string(getuid());
#endif
      }
    }

    if (mod == "io") {
      if (args.empty())
        return "";
      string path = args[0];

      if (func == "read") {
        ifstream f(path);
        if (!f.is_open())
          return "ERR";
        stringstream ss;
        ss << f.rdbuf();
        return ss.str();
      }
      if (func == "write" && args.size() > 1) {
        ofstream f(path);
        if (!f.is_open())
          return "0";
        f << args[1];
        return "1";
      }
      if (func == "append" && args.size() > 1) {
        ofstream f(path, ios::app);
        if (!f.is_open())
          return "0";
        f << args[1];
        return "1";
      }
      if (func == "exists") {
        struct stat buf;
        return stat(path.c_str(), &buf) == 0 ? "1" : "0";
      }
      if (func == "isdir") {
        struct stat buf;
        return (stat(path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode)) ? "1"
                                                                       : "0";
      }
      if (func == "isfile") {
        struct stat buf;
        return (stat(path.c_str(), &buf) == 0 && S_ISREG(buf.st_mode)) ? "1"
                                                                       : "0";
      }
      if (func == "mkdir") {
#ifdef _WIN32
        return _mkdir(path.c_str()) == 0 ? "1" : "0";
#else
        return mkdir(path.c_str(), 0777) == 0 ? "1" : "0";
#endif
      }
      if (func == "rmdir") {
#ifdef _WIN32
        system(("rmdir /s /q \"" + path + "\"").c_str());
#else
        system(("rm -rf \"" + path + "\"").c_str());
#endif
        return "1";
      }
      if (func == "remove")
        return remove(path.c_str()) == 0 ? "1" : "0";
      if (func == "rename" && args.size() > 1)
        return rename(path.c_str(), args[1].c_str()) == 0 ? "1" : "0";
      if (func == "copy" && args.size() > 1) {
        ifstream s(path, ios::binary);
        ofstream d(args[1], ios::binary);
        if (!s.is_open() || !d.is_open())
          return "0";
        d << s.rdbuf();
        return "1";
      }
      if (func == "size") {
        struct stat buf;
        return stat(path.c_str(), &buf) == 0 ? to_string(buf.st_size) : "0";
      }
      if (func == "list") {
        vector<string> files;
#ifdef _WIN32
        string sp = path + "/*";
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA(sp.c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
          do {
            string n = fd.cFileName;
            if (n != "." && n != "..")
              files.push_back(n);
          } while (FindNextFileA(h, &fd));
          FindClose(h);
        }
#else
        DIR *dir = opendir(path.c_str());
        if (dir) {
          struct dirent *entry;
          while ((entry = readdir(dir)) != NULL) {
            string n = entry->d_name;
            if (n != "." && n != "..")
              files.push_back(n);
          }
          closedir(dir);
        }
#endif
        string r = "[";
        for (size_t i = 0; i < files.size(); i++) {
          if (i > 0)
            r += ", ";
          r += "\"" + files[i] + "\"";
        }
        r += "]";
        return r;
      }
      if (func == "ext") {
        size_t dot = path.rfind('.');
        return dot == string::npos ? "" : path.substr(dot);
      }
      if (func == "base") {
        size_t slash = path.find_last_of("/\\");
        return slash == string::npos ? path : path.substr(slash + 1);
      }
      if (func == "abs") {
#ifdef _WIN32
        char buf[MAX_PATH];
        return GetFullPathNameA(path.c_str(), MAX_PATH, buf, NULL) ? string(buf)
                                                                   : path;
#else
        char buf[PATH_MAX];
        return realpath(path.c_str(), buf) ? string(buf) : path;
#endif
      }
      if (func == "join" && args.size() > 1)
        return path + "/" + args[1];
      if (func == "split") {
        size_t slash = path.find_last_of("/\\");
        if (slash == string::npos)
          return "[\"\", \"" + path + "\"]";
        return "[\"" + path.substr(0, slash) + "\", \"" +
               path.substr(slash + 1) + "\"]";
      }
      if (func == "dirname") {
        size_t slash = path.find_last_of("/\\");
        return slash == string::npos ? "" : path.substr(0, slash);
      }
      if (func == "touch") {
        ofstream f(path, ios::app);
        return "1";
      }
      if (func == "read_lines") {
        ifstream f(path);
        if (!f.is_open())
          return "[]";
        string l;
        string r = "[";
        bool first = true;
        while (getline(f, l)) {
          if (!first)
            r += ", ";
          r += "\"" + l + "\"";
          first = false;
        }
        r += "]";
        return r;
      }
      if (func == "home") {
        char *h = getenv("HOME");
        if (!h)
          h = getenv("USERPROFILE");
        return h ? h : "/";
      }
      if (func == "tmp") {
        char *t = getenv("TMPDIR");
        if (!t)
          t = getenv("TEMP");
        if (!t)
          t = getenv("TMP");
        return t ? t : "/tmp";
      }
      if (func == "glob") {
        string pattern = path;
        string base_dir = ".";
        string ext_filter = "";

        size_t stars = pattern.find("/**");
        if (stars != string::npos) {
          base_dir = pattern.substr(0, stars);
          size_t last_dot = pattern.rfind('.');
          if (last_dot != string::npos && last_dot > stars) {
            ext_filter = pattern.substr(last_dot);
          }
        } else {
          base_dir = pattern;
        }
        if (base_dir.empty())
          base_dir = ".";

        string find_cmd = "find \"" + base_dir + "\" -type f";
        if (!ext_filter.empty()) {
          find_cmd += " -name \"*" + ext_filter + "\"";
        }
        find_cmd += " 2>/dev/null";

        string output = run_shell(find_cmd);
        stringstream ss(output);
        string item;
        string r = "[";
        bool first = true;
        while (getline(ss, item)) {
          item = trim(item);
          if (!item.empty()) {
            if (!first)
              r += ", ";
            r += "\"" + item + "\"";
            first = false;
          }
        }
        r += "]";
        return r;
      }
      if (func == "file_hash_sha256") {
#ifdef __APPLE__
        string r = run_shell("shasum -a 256 \"" + path + "\"");
#else
        string r = run_shell("sha256sum \"" + path + "\"");
#endif
        size_t space = r.find(' ');
        return space != string::npos ? r.substr(0, space) : r;
      }
      if (func == "file_hash_md5") {
#ifdef __APPLE__
        return run_shell("md5 -q \"" + path + "\"");
#else
        string r = run_shell("md5sum \"" + path + "\"");
        size_t space = r.find(' ');
        return space != string::npos ? r.substr(0, space) : r;
#endif
      }
    }

    if (mod == "zip") {
      if (args.empty())
        return "0";
      if (func == "compress" && args.size() > 1) {
        return to_string(system(("zip -r \"" + args[1] + "\" \"" + args[0] +
                                 "\" >/dev/null 2>&1")
                                    .c_str()) == 0);
      }
      if (func == "extract" && args.size() > 1) {
        return to_string(system(("unzip -o \"" + args[0] + "\" -d \"" +
                                 args[1] + "\" >/dev/null 2>&1")
                                    .c_str()) == 0);
      }
      if (func == "list") {
        return run_shell("unzip -l \"" + args[0] + "\"");
      }
    }

    if (mod == "net") {
      if (func == "get" && !args.empty()) {
        return run_shell("curl -sL \"" + args[0] + "\"");
      }
      if (func == "post" && args.size() > 1) {
        return run_shell("curl -s -X POST -d \"" + args[1] + "\" \"" + args[0] +
                         "\"");
      }
      if (func == "download" && args.size() > 1) {
        return to_string(
            system(("curl -sL \"" + args[0] + "\" -o \"" + args[1] + "\"")
                       .c_str()) == 0);
      }
      if (func == "ping" && !args.empty()) {
#ifdef _WIN32
        return to_string(
            system(("ping -n 1 " + args[0] + " >nul 2>&1").c_str()) == 0);
#else
        return to_string(
            system(("ping -c 1 " + args[0] + " >/dev/null 2>&1").c_str()) == 0);
#endif
      }
      if (func == "ip")
        return "127.0.0.1";
      if (func == "is_online") {
#ifdef _WIN32
        return to_string(system("ping -n 1 8.8.8.8 >nul 2>&1") == 0);
#else
        return to_string(system("ping -c 1 8.8.8.8 >/dev/null 2>&1") == 0);
#endif
      }
      if (func == "get_json" && !args.empty()) {
        return run_shell("curl -sL \"" + args[0] + "\"");
      }
    }

    if (mod == "date") {
      time_t t = time(0);
      tm *l = localtime(&t);
      if (func == "now") {
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", l);
        return string(buf);
      }
      if (func == "year")
        return to_string(l->tm_year + 1900);
      if (func == "month")
        return to_string(l->tm_mon + 1);
      if (func == "day")
        return to_string(l->tm_mday);
      if (func == "hour")
        return to_string(l->tm_hour);
      if (func == "min")
        return to_string(l->tm_min);
      if (func == "sec")
        return to_string(l->tm_sec);
      if (func == "ts")
        return to_string(t);
      if (func == "today") {
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d", l);
        return string(buf);
      }
      if (func == "iso") {
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&t));
        return string(buf);
      }
      if (func == "fmt" && !args.empty()) {
        char buf[128];
        strftime(buf, sizeof(buf), args[0].c_str(), l);
        return string(buf);
      }
      if (func == "sleep_ms" && !args.empty()) {
        this_thread::sleep_for(chrono::milliseconds(stoi(args[0])));
        return "1";
      }
    }

    if (mod == "rnd") {
      if (func == "val")
        return to_string((double)rand() / RAND_MAX);
      if (func == "int" && args.size() > 1) {
        int min_v = stoi(args[0]);
        int max_v = stoi(args[1]);
        return to_string(min_v + rand() % (max_v - min_v + 1));
      }
      if (func == "float" && args.size() > 1) {
        double min_v = stod(args[0]);
        double max_v = stod(args[1]);
        return to_string(min_v + ((double)rand() / RAND_MAX) * (max_v - min_v));
      }
      if (func == "choice" && !args.empty()) {
        auto items = parse_string_list(args[0]);
        return items.empty() ? "" : items[rand() % items.size()];
      }
      if (func == "shuffle" && !args.empty()) {
        auto items = parse_string_list(args[0]);
        random_shuffle(items.begin(), items.end());
        string r = "[";
        for (size_t i = 0; i < items.size(); i++) {
          if (i > 0)
            r += ", ";
          r += "\"" + items[i] + "\"";
        }
        r += "]";
        return r;
      }
      if (func == "uuid") {
        char hex_chars[] = "0123456789abcdef";
        string r = "";
        for (int i = 0; i < 36; i++) {
          if (i == 8 || i == 13 || i == 18 || i == 23)
            r += "-";
          else if (i == 14)
            r += "4";
          else
            r += hex_chars[rand() % 16];
        }
        return r;
      }
    }

    if (mod == "crypto") {
      if (args.empty())
        return "";
      if (func == "md5") {
#ifdef __APPLE__
        return run_shell("echo -n \"" + args[0] + "\" | md5");
#else
        string r = run_shell("echo -n \"" + args[0] + "\" | md5sum");
        size_t space = r.find(' ');
        return space != string::npos ? r.substr(0, space) : r;
#endif
      }
      if (func == "sha256") {
        string r = run_shell("echo -n \"" + args[0] + "\" | shasum -a 256");
        size_t space = r.find(' ');
        return space != string::npos ? r.substr(0, space) : r;
      }
      if (func == "b64e")
        return base64_encode(args[0]);
      if (func == "b64d")
        return base64_decode(args[0]);
    }

    if (mod == "json") {
      if (args.empty())
        return "";
      if (func == "parse")
        return args[0];
      if (func == "str")
        return args[0];
      if (func == "load") {
        ifstream f(args[0]);
        if (!f.is_open())
          return "{}";
        stringstream ss;
        ss << f.rdbuf();
        return ss.str();
      }
      if (func == "save" && args.size() > 1) {
        ofstream f(args[0]);
        f << args[1];
        return "1";
      }
      if (func == "pretty")
        return args[0];
      if (func == "get_keys") {
        auto m = parse_json_to_map(args[0]);
        string r = "[";
        bool first = true;
        for (auto &pair : m) {
          if (!first)
            r += ", ";
          r += "\"" + pair.first + "\"";
          first = false;
        }
        r += "]";
        return r;
      }
    }

    if (mod == "cli") {
      if (args.empty())
        return "";
      if (func == "clear") {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        return "1";
      }
      if (func == "red")
        return "\033[91m" + args[0] + "\033[0m";
      if (func == "green")
        return "\033[92m" + args[0] + "\033[0m";
      if (func == "blue")
        return "\033[94m" + args[0] + "\033[0m";
      if (func == "yellow")
        return "\033[93m" + args[0] + "\033[0m";
      if (func == "cyan")
        return "\033[96m" + args[0] + "\033[0m";
      if (func == "magenta")
        return "\033[95m" + args[0] + "\033[0m";
      if (func == "reset")
        return "\033[0m" + args[0];
      if (func == "bold")
        return "\033[1m" + args[0] + "\033[0m";
      if (func == "print") {
        cout << args[0] << endl;
        return "1";
      }
      if (func == "pause") {
        cout << "Press Enter to continue...";
        string t;
        getline(cin, t);
        return "1";
      }
    }

    if (mod == "gui") {
      if (func == "window" && !args.empty()) {
        vars["GUI_TITLE"] = args[0];
        return "1";
      }
      if (func == "geom" && !args.empty()) {
        vars["GUI_GEOM"] = args[0];
        return "1";
      }
      if (func == "title" && !args.empty()) {
        vars["GUI_TITLE"] = args[0];
        return "1";
      }
      if (func == "label" && !args.empty()) {
        cout << "GUI Label: " << args[0] << endl;
        return "1";
      }
      if (func == "button" && args.size() >= 2) {
        gui_buttons.push_back({args[0], args[1]});
        return "1";
      }
      if (func == "entry" && args.size() >= 2) {
        gui_entries[args[0]] = args[1];
        return "1";
      }
      if (func == "get_entry" && !args.empty()) {
        return gui_entries.count(args[0]) ? gui_entries[args[0]] : "";
      }
      if (func == "run") {
        show_cpp_dashboard();
        return "1";
      }
      if (func == "msg" && !args.empty()) {
        int status = -1;
#ifdef __APPLE__
        status = system(("osascript -e 'display alert \"Nexus\" message \"" +
                         args[0] + "\"' >/dev/null 2>&1")
                            .c_str());
#else
        status = system(("zenity --info --title=\"Nexus\" --text=\"" + args[0] +
                         "\" >/dev/null 2>&1")
                            .c_str());
#endif
        if (status != 0) {
          cout << "\n+--------------------------------------------------+"
               << endl;
          cout << "|                   NEXUS ALERT                    |"
               << endl;
          cout << "+--------------------------------------------------+"
               << endl;
          cout << "  " << args[0] << endl;
          cout << "+--------------------------------------------------+"
               << endl;
          cout << "Press Enter to dismiss...";
          cin.get();
        }
        return "1";
      }
      if (func == "warn" && !args.empty()) {
        int status = -1;
#ifdef __APPLE__
        status =
            system(("osascript -e 'display alert \"Nexus WARNING\" message \"" +
                    args[0] + "\"' >/dev/null 2>&1")
                       .c_str());
#else
        status =
            system(("zenity --warning --title=\"Nexus Warning\" --text=\"" +
                    args[0] + "\" >/dev/null 2>&1")
                       .c_str());
#endif
        if (status != 0) {
          cout << "\n\033[93m+-------------------------------------------------"
                  "-+"
               << endl;
          cout << "|                  NEXUS WARNING                   |"
               << endl;
          cout << "+--------------------------------------------------+"
               << endl;
          cout << "  " << args[0] << endl;
          cout << "+--------------------------------------------------+\033[0m"
               << endl;
          cout << "Press Enter to dismiss...";
          cin.get();
        }
        return "1";
      }
      if (func == "err" && !args.empty()) {
        int status = -1;
#ifdef __APPLE__
        status =
            system(("osascript -e 'display alert \"Nexus ERROR\" message \"" +
                    args[0] + "\"' >/dev/null 2>&1")
                       .c_str());
#else
        status = system(("zenity --error --title=\"Nexus Error\" --text=\"" +
                         args[0] + "\" >/dev/null 2>&1")
                            .c_str());
#endif
        if (status != 0) {
          cout << "\n\033[91m+-------------------------------------------------"
                  "-+"
               << endl;
          cout << "|                   NEXUS ERROR                    |"
               << endl;
          cout << "+--------------------------------------------------+"
               << endl;
          cout << "  " << args[0] << endl;
          cout << "+--------------------------------------------------+\033[0m"
               << endl;
          cout << "Press Enter to dismiss...";
          cin.get();
        }
        return "1";
      }
      if (func == "ask" && !args.empty()) {
        int status = -1;
#ifdef __APPLE__
        status = system(("osascript -e 'display dialog \"" + args[0] +
                         "\" buttons {\"No\", \"Yes\"} default button \"Yes\"' "
                         ">/dev/null 2>&1")
                            .c_str());
#else
        status = system(("zenity --question --title=\"Nexus\" --text=\"" +
                         args[0] + "\" >/dev/null 2>&1")
                            .c_str());
#endif
        if (status == 0)
          return "1";
        if (status != -1)
          return "0";

        cout << "\n[ASK] " << args[0] << " (y/n): ";
        string r;
        getline(cin, r);
        return (r == "y" || r == "Y" || r == "yes" || r == "Yes") ? "1" : "0";
      }
      if (func == "input" && !args.empty()) {
        string res = "";
        FILE *p = popen(("zenity --entry --title=\"Nexus\" --text=\"" +
                         args[0] + "\" 2>/dev/null")
                            .c_str(),
                        "r");
        if (p) {
          char buf[128];
          while (fgets(buf, sizeof(buf), p))
            res += buf;
          pclose(p);
          res = trim(res);
        }
        if (res.empty()) {
          cout << "[INPUT] " << args[0] << ": ";
          getline(cin, res);
        }
        return res;
      }
    }

    return "";
  }

  string strip_parens(string s) {
    s = trim(s);
    if (s.length() >= 2 && s.front() == '(' && s.back() == ')') {
      int depth = 0;
      bool ok = true;
      for (size_t i = 0; i < s.length() - 1; ++i) {
        if (s[i] == '(')
          depth++;
        else if (s[i] == ')')
          depth--;
        if (depth == 0) {
          ok = false;
          break;
        }
      }
      if (ok && depth == 1 && s.back() == ')') {
        return strip_parens(s.substr(1, s.length() - 2));
      }
    }
    return s;
  }

  size_t find_operator(const string &s, const string &op) {
    int depth = 0;
    size_t op_len = op.length();
    if (s.length() < op_len)
      return string::npos;

    vector<bool> quotes_map(s.length(), false);
    bool q = false;
    for (size_t idx = 0; idx < s.length(); ++idx) {
      if (s[idx] == '"' && (idx == 0 || s[idx - 1] != '\\')) {
        q = !q;
      }
      quotes_map[idx] = q;
    }

    for (int i = (int)s.length() - (int)op_len; i >= 0; --i) {
      if (quotes_map[i])
        continue;

      char c = s[i];
      if (c == '(')
        depth++;
      else if (c == ')')
        depth--;

      if (depth == 0) {
        if (s.substr(i, op_len) == op) {
          return i;
        }
      }
    }
    return string::npos;
  }

  string evaluate_expression(string s) {
    s = strip_parens(s);
    if (s.empty())
      return "";

    vector<string> ops = {
        "||", "&&", "==", "!=", "<=", ">=", "<", ">", "+", "-", "*", "/"};
    for (const string &op : ops) {
      size_t pos = find_operator(s, op);
      if (pos != string::npos) {
        string left_raw = s.substr(0, pos);
        string right_raw = s.substr(pos + op.length());
        string left = evaluate_expression(left_raw);
        string right = evaluate_expression(right_raw);

        if (op == "||")
          return (left != "0" && !left.empty()) ||
                         (right != "0" && !right.empty())
                     ? "1"
                     : "0";
        if (op == "&&")
          return (left != "0" && !left.empty()) &&
                         (right != "0" && !right.empty())
                     ? "1"
                     : "0";

        bool num_cmp = is_number(left) && is_number(right);
        double l_num = num_cmp ? stod(left) : 0;
        double r_num = num_cmp ? stod(right) : 0;

        if (op == "==") {
          return (left == right) ? "1" : "0";
        }
        if (op == "!=")
          return (left != right) ? "1" : "0";

        if (op == "<=")
          return num_cmp ? (l_num <= r_num ? "1" : "0")
                         : (left <= right ? "1" : "0");
        if (op == ">=")
          return num_cmp ? (l_num >= r_num ? "1" : "0")
                         : (left >= right ? "1" : "0");
        if (op == "<")
          return num_cmp ? (l_num < r_num ? "1" : "0")
                         : (left < right ? "1" : "0");
        if (op == ">")
          return num_cmp ? (l_num > r_num ? "1" : "0")
                         : (left > right ? "1" : "0");

        if (op == "+") {
          if (num_cmp) {
            if (left.find('.') == string::npos &&
                right.find('.') == string::npos) {
              return to_string((long long)(l_num + r_num));
            }
            return to_string(l_num + r_num);
          } else {
            return left + right;
          }
        }
        if (op == "-") {
          if (num_cmp) {
            if (left.find('.') == string::npos &&
                right.find('.') == string::npos) {
              return to_string((long long)(l_num - r_num));
            }
            return to_string(l_num - r_num);
          }
          return "0";
        }
        if (op == "*") {
          if (num_cmp) {
            if (left.find('.') == string::npos &&
                right.find('.') == string::npos) {
              return to_string((long long)(l_num * r_num));
            }
            return to_string(l_num * r_num);
          }
          if (is_number(right)) {
            int count = (int)stod(right);
            string r = "";
            for (int i = 0; i < count; i++)
              r += left;
            return r;
          }
          return "0";
        }
        if (op == "/") {
          if (num_cmp && r_num != 0)
            return to_string(l_num / r_num);
          return "0";
        }
      }
    }

    if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
      return s.substr(1, s.length() - 2);
    }

    size_t dot = s.find('.');
    if (dot != string::npos && s.back() == ')') {
      size_t lp = s.find('(', dot);
      if (lp != string::npos) {
        string mod = trim(s.substr(0, dot));
        string func = trim(s.substr(dot + 1, lp - dot - 1));
        string args_raw = s.substr(lp + 1, s.length() - lp - 2);

        vector<string> args;
        int depth = 0;
        bool in_quotes = false;
        string current_arg = "";
        for (size_t i = 0; i < args_raw.length(); ++i) {
          char c = args_raw[i];
          if (c == '"' && (i == 0 || args_raw[i - 1] != '\\'))
            in_quotes = !in_quotes;
          if (!in_quotes) {
            if (c == '(')
              depth++;
            else if (c == ')')
              depth--;
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

    if (vars.count(s))
      return vars[s];
    return s;
  }

  void import_lib(string path) {
    if (path.length() >= 2 && path.front() == '"' && path.back() == '"') {
      path = path.substr(1, path.length() - 2);
    }

    string resolved_path = "";
    vector<string> candidates = {path, vars["CWD"] + "/" + path,
                                 vars["CWD"] + "/libs/" + path};

    ifstream check;
    for (const string &cand : candidates) {
      check.open(cand);
      if (check.is_open()) {
        resolved_path = cand;
        check.close();
        break;
      }
    }

    if (resolved_path.empty()) {
      cerr << "Error: Library not found: " << path << endl;
      return;
    }

    if (resolved_path.length() >= 3 &&
        resolved_path.substr(resolved_path.length() - 3) == ".py") {
      string mod_name = path;
      size_t slash = path.find_last_of("/\\");
      if (slash != string::npos)
        mod_name = path.substr(slash + 1);
      size_t dot = mod_name.find('.');
      if (dot != string::npos)
        mod_name = mod_name.substr(0, dot);

      ifstream f(resolved_path);
      string l;
      while (getline(f, l)) {
        l = trim(l);
        if (l.substr(0, 4) == "def ") {
          size_t lp = l.find('(');
          if (lp != string::npos) {
            string func_name = trim(l.substr(4, lp - 4));
            python_funcs[mod_name][func_name] = resolved_path;
          }
        }
      }
      f.close();
      cout << "Nexus › Imported Python Module: " << mod_name << endl;
      return;
    }

    ifstream f(resolved_path);
    vector<string> imported_lines;
    string s;
    while (getline(f, s)) {
      imported_lines.push_back(s);
    }
    f.close();

    unordered_map<string, Function> old_funcs = funcs;
    run(imported_lines);

    string mod_name = path;
    size_t slash = path.find_last_of("/\\");
    if (slash != string::npos)
      mod_name = path.substr(slash + 1);
    size_t dot = mod_name.find('.');
    if (dot != string::npos)
      mod_name = mod_name.substr(0, dot);

    for (auto &pair : funcs) {
      if (old_funcs.find(pair.first) == old_funcs.end()) {
        imported_funcs[mod_name][pair.first] = pair.first;
      }
    }
    cout << "Nexus › Imported Nexus Module: " << mod_name << endl;
  }

  string execute_custom_function(const string &name,
                                 const vector<string> &args) {
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

  void handle_add_directive(string content) {
    stringstream ss(content);
    string line;
    while (getline(ss, line)) {
      line = trim(line);
      if (line.empty() || line[0] == '#')
        continue;
      if (line[0] == '-') {
        string lib_type = trim(line.substr(1));
        cout << "Nexus › Enabling support for: " << lib_type << endl;
      }
    }
  }

  void show_cpp_dashboard() {
    while (true) {
      cout << "\n========================================" << endl;
      cout << "      NEXUS GRAPHICAL DASHBOARD         " << endl;
      cout << "========================================" << endl;
      for (size_t idx = 0; idx < gui_buttons.size(); ++idx) {
        cout << idx + 1 << ". " << gui_buttons[idx].first << endl;
      }
      cout << gui_buttons.size() + 1 << ". Exit" << endl;
      cout << "========================================" << endl;
      cout << "Enter selection: ";
      string choice_raw;
      getline(cin, choice_raw);
      choice_raw = trim(choice_raw);
      if (choice_raw.empty())
        continue;

      if (is_number(choice_raw)) {
        int sel = stoi(choice_raw);
        if (sel == (int)gui_buttons.size() + 1)
          break;
        if (sel >= 1 && sel <= (int)gui_buttons.size()) {
          execute_custom_function(gui_buttons[sel - 1].second, {});
        }
      }
    }
  }

  void run(vector<string> lines) {
    for (size_t i = 0; i < lines.size(); ++i) {
      if (vars.count("return"))
        return;
      if (vars.count("break"))
        return;

      string line = trim(lines[i]);
      if (line.empty() || line[0] == '#')
        continue;

      if (line.substr(0, 4) == "+add") {
        string block = "";
        i++;
        while (i < lines.size() && trim(lines[i]).find(')') == string::npos) {
          block += lines[i] + "\n";
          i++;
        }
        handle_add_directive(block);
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
          if (l.substr(0, 2) == "if" || l.substr(0, 5) == "loop" ||
              l.substr(0, 5) == "while")
            d++;
          else if (l == "else" && d == 1)
            else_pos = i;
          else if (l == "end")
            d--;
          i++;
        }
        if (cond) {
          vector<string> block;
          for (size_t j = start; j < (else_pos != -1 ? else_pos : i - 1); j++)
            block.push_back(lines[j]);
          run(block);
        } else if (else_pos != -1) {
          vector<string> block;
          for (size_t j = else_pos + 1; j < i - 1; j++)
            block.push_back(lines[j]);
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
          if (l.substr(0, 5) == "loop" || l.substr(0, 2) == "if" ||
              l.substr(0, 5) == "while")
            d++;
          else if (l == "end")
            d--;
          i++;
        }
        vector<string> block;
        for (size_t j = start; j < i - 1; j++)
          block.push_back(lines[j]);
        for (int c = 0; c < count; c++) {
          if (vars.count("return"))
            break;
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
          if (l.substr(0, 5) == "while" || l.substr(0, 2) == "if" ||
              l.substr(0, 5) == "loop")
            d++;
          else if (l == "end")
            d--;
          i++;
        }
        vector<string> block;
        for (size_t j = start; j < i - 1; j++)
          block.push_back(lines[j]);
        while (true) {
          string cond_val = evaluate_expression(cond_raw);
          if (cond_val == "0" || cond_val.empty())
            break;
          run(block);
          if (vars.count("return"))
            break;
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
        while (getline(ss, param, ',')) {
          params.push_back(trim(param));
        }

        vector<string> block;
        i++;
        int d = 1;
        while (d > 0 && i < lines.size()) {
          string l = trim(lines[i]);
          if (l.substr(0, 2) == "fn" || l.substr(0, 2) == "if" ||
              l.substr(0, 5) == "loop" || l.substr(0, 5) == "while")
            d++;
          else if (l == "end")
            d--;
          if (d > 0)
            block.push_back(lines[i]);
          i++;
        }
        funcs[name] = {params, block};
        i--;
        continue;
      }

      size_t lp = line.find('(');
      size_t rp = line.rfind(')');
      if (lp != string::npos && rp == line.length() - 1 &&
          line.substr(0, lp).find('.') == string::npos) {
        string name = trim(line.substr(0, lp));
        if (funcs.count(name)) {
          string args_raw = line.substr(lp + 1, rp - lp - 1);
          vector<string> args;
          int depth = 0;
          bool in_quotes = false;
          string current_arg = "";
          for (size_t idx = 0; idx < args_raw.length(); ++idx) {
            char c = args_raw[idx];
            if (c == '"' && (idx == 0 || args_raw[idx - 1] != '\\'))
              in_quotes = !in_quotes;
            if (!in_quotes) {
              if (c == '(')
                depth++;
              else if (c == ')')
                depth--;
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
};

int main(int argc, char *argv[]) {
  NexusUltraV4 engine;
  if (argc < 2) {
    cout << "" << endl;
    cout << " ____________________________________________________________ "
         << endl;
    cout << "|                           Tiwut              {#} {-} {x}   |"
         << endl;
    cout << "|------------------------------------------------------------|"
         << endl;
    cout << "|                                                            |"
         << endl;
    cout << "|      #     #  #####   #     #  #     #   ###      ##       |"
         << endl;
    cout << "|      # #   #  #___      # #    #     #  #__     #  #       |"
         << endl;
    cout << "|      #   # #  #         # #    #     #     #   #######     |"
         << endl;
    cout << "|      #     #  #####   #     #   #####   ###        #       |"
         << endl;
    cout << "|                                                            |"
         << endl;
    cout << "|                 Pure Code. Total Freedom.                  |"
         << endl;
    cout << "|           ______________________________________           |"
         << endl;
    cout << "|           |                                    |           |"
         << endl;
    cout << "|           | Website : https://tiwut.org/nexus  |           |"
         << endl;
    cout << "|           | GitHub  : https://github.com/tiwut |           |"
         << endl;
    cout << "|           |____________________________________|           |"
         << endl;
    cout << "|                                                            |"
         << endl;
    cout << "| ''Every line of code is a step towards digital autonomy.'' |"
         << endl;
    cout << "|                                                            |"
         << endl;
    cout << "|____________________________________________________________|"
         << endl;
    cout << "" << endl;
    return 0;
  }
  ifstream f(argv[1]);
  if (!f.is_open()) {
    cerr << "Error: " << argv[1] << " not found." << endl;
    return 1;
  }
  vector<string> l;
  string s;
  while (getline(f, s))
    l.push_back(s);
  engine.run(l);
  return 0;
}

string base64_encode(const string &in) {
  static const char *lookup =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  string out;
  int val = 0, valb = -6;
  for (char c : in) {
    val = (val << 8) + (unsigned char)c;
    valb += 8;
    while (valb >= 0) {
      out.push_back(lookup[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6)
    out.push_back(lookup[((val << 8) >> (valb + 8)) & 0x3F]);
  while (out.size() % 4)
    out.push_back('=');
  return out;
}

string base64_decode(const string &in) {
  static const string lookup =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  vector<int> T(256, -1);
  for (int i = 0; i < 64; i++)
    T[lookup[i]] = i;
  string out;
  int val = 0, valb = -8;
  for (char c : in) {
    if (T[c] == -1)
      continue;
    val = (val << 6) + T[c];
    valb += 6;
    while (valb >= 0) {
      out.push_back((char)((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}
