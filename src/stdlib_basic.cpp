#include "stdlib.h"
#include <cmath>
#include <iomanip>
#include <openssl/md5.h>
#include <openssl/sha.h>

string execute_stdlib_basic(const string &mod, const string &func, const vector<string> &args, NexusUltraV4* engine) {
  if (mod == "math") {
    if (func == "pi") return "3.141592653589793";
    if (func == "e") return "2.718281828459045";
    if (func == "tau") return "6.283185307179586";
    if (func == "inf") return "inf";
    if (func == "nan") return "nan";
    if (func == "sin" && !args.empty()) return to_string(sin(stod(args[0])));
    if (func == "cos" && !args.empty()) return to_string(cos(stod(args[0])));
    if (func == "tan" && !args.empty()) return to_string(tan(stod(args[0])));
    if (func == "asin" && !args.empty()) return to_string(asin(stod(args[0])));
    if (func == "acos" && !args.empty()) return to_string(acos(stod(args[0])));
    if (func == "atan" && !args.empty()) return to_string(atan(stod(args[0])));
    if (func == "sqrt" && !args.empty()) return to_string(sqrt(stod(args[0])));
    if (func == "pow" && args.size() > 1) return to_string(pow(stod(args[0]), stod(args[1])));
    if (func == "abs" && !args.empty()) return to_string(abs(stod(args[0])));
    if (func == "min" && args.size() > 1) return to_string(min(stod(args[0]), stod(args[1])));
    if (func == "max" && args.size() > 1) return to_string(max(stod(args[0]), stod(args[1])));
    if (func == "ceil" && !args.empty()) return to_string(ceil(stod(args[0])));
    if (func == "floor" && !args.empty()) return to_string(floor(stod(args[0])));
    if (func == "round" && !args.empty()) return to_string(round(stod(args[0])));
    if (func == "trunc" && !args.empty()) return to_string(trunc(stod(args[0])));
    if (func == "clamp" && args.size() >= 3) {
      double v = stod(args[0]);
      double lo = stod(args[1]);
      double hi = stod(args[2]);
      return to_string(v < lo ? lo : (v > hi ? hi : v));
    }
    if (func == "lerp" && args.size() >= 3) {
      double a = stod(args[0]);
      double b = stod(args[1]);
      double t = stod(args[2]);
      return to_string(a + t * (b - a));
    }
    if (func == "sign" && !args.empty()) {
      double v = stod(args[0]);
      return to_string(v < 0 ? -1 : (v > 0 ? 1 : 0));
    }
  }

  if (mod == "str") {
    if (func == "len" && !args.empty()) return to_string(args[0].length());
    if (func == "upper" && !args.empty()) {
      string r = args[0];
      for (char &c : r) c = toupper(c);
      return r;
    }
    if (func == "lower" && !args.empty()) {
      string r = args[0];
      for (char &c : r) c = tolower(c);
      return r;
    }
    if (func == "trim" && !args.empty()) return trim(args[0]);
    if (func == "reverse" && !args.empty()) {
      string r = args[0];
      int n = r.length();
      for (int i = 0; i < n / 2; i++) swap(r[i], r[n - i - 1]);
      return r;
    }
    if (func == "repeat" && args.size() > 1) {
      string r = "";
      int count = stoi(args[1]);
      for (int i = 0; i < count; i++) r += args[0];
      return r;
    }
    if (func == "replace" && args.size() > 2) {
      string r = args[0];
      string old_s = args[1];
      string new_s = args[2];
      size_t pos = 0;
      while ((pos = r.find(old_s, pos)) != string::npos) {
        r.replace(pos, old_s.length(), new_s);
        pos += new_s.length();
      }
      return r;
    }
    if (func == "slice" && args.size() > 2) {
      int s = stoi(args[1]);
      int e = stoi(args[2]);
      if (s < 0) s = 0;
      if (e > (int)args[0].length()) e = args[0].length();
      return args[0].substr(s, e - s);
    }
    if (func == "at" && args.size() > 1) {
      string lst = args[0];
      int idx = stoi(args[1]);
      size_t first = lst.find('[');
      size_t last = lst.find(']');
      if (first != string::npos && last != string::npos) {
        string inner = lst.substr(first + 1, last - first - 1);
        stringstream ss(inner);
        string el;
        vector<string> items;
        while (getline(ss, el, ',')) {
          string t = trim(el);
          if (t.front() == '"' && t.back() == '"') {
            t = t.substr(1, t.length() - 2);
          }
          items.push_back(t);
        }
        if (idx >= 0 && idx < (int)items.size()) return items[idx];
      }
      return "";
    }
    if (func == "find" && args.size() > 1) {
      size_t pos = args[0].find(args[1]);
      return pos == string::npos ? "-1" : to_string(pos);
    }
    if (func == "starts" && args.size() > 1) return to_string(args[0].compare(0, args[1].length(), args[1]) == 0);
    if (func == "ends" && args.size() > 1) {
      if (args[0].length() >= args[1].length()) {
        return to_string(args[0].compare(args[0].length() - args[1].length(), args[1].length(), args[1]) == 0);
      }
      return "0";
    }
    if (func == "isnum" && !args.empty()) {
      try {
        stod(args[0]);
        return "1";
      } catch (...) {
        return "0";
      }
    }
    if (func == "isalpha" && !args.empty()) {
      for (char c : args[0]) {
        if (!isalpha(c) && c != ' ') return "0";
      }
      return "1";
    }
    if (func == "chr" && !args.empty()) {
      string r = "";
      r += (char)stoi(args[0]);
      return r;
    }
    if (func == "ord" && !args.empty()) return to_string((int)args[0].front());
    if (func == "contains" && args.size() >= 2) return to_string(args[0].find(args[1]) != string::npos);
    if (func == "pad_left" && args.size() >= 3) {
      string s = args[0];
      int w = stoi(args[1]);
      string p = args[2];
      while ((int)s.length() < w) s = p + s;
      return s;
    }
    if (func == "pad_right" && args.size() >= 3) {
      string s = args[0];
      int w = stoi(args[1]);
      string p = args[2];
      while ((int)s.length() < w) s = s + p;
      return s;
    }
  }

  if (mod == "sys") {
    if (func == "os") return get_os();
    if (func == "ver") return "10.15";
    if (func == "arch") return "x86_64";
    if (func == "user") return get_user();
    if (func == "cwd") {
      char buf[512];
#ifdef _WIN32
      GetCurrentDirectoryA(sizeof(buf), buf);
#else
      if (!getcwd(buf, sizeof(buf))) return "";
#endif
      return string(buf);
    }
    if (func == "hostname") {
      char buf[256];
#ifdef _WIN32
      DWORD size = sizeof(buf);
      GetComputerNameA(buf, &size);
#else
      gethostname(buf, sizeof(buf));
#endif
      return string(buf);
    }
    if (func == "info") return "Nexus OS | v4.5.0 | CPP-Engine";
  }

  if (mod == "date") {
    time_t t = time(0);
    tm *l = localtime(&t);
    if (func == "now") {
      char buf[64];
      strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", l);
      return string(buf);
    }
    if (func == "year") return to_string(l->tm_year + 1900);
    if (func == "month") return to_string(l->tm_mon + 1);
    if (func == "day") return to_string(l->tm_mday);
    if (func == "hour") return to_string(l->tm_hour);
    if (func == "min") return to_string(l->tm_min);
    if (func == "sec") return to_string(l->tm_sec);
    if (func == "ts") return to_string(t);
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
    if (func == "val") return to_string((double)rand() / RAND_MAX);
    if (func == "int" && args.size() > 1) {
      int low = stoi(args[0]);
      int high = stoi(args[1]);
      return to_string(low + rand() % (high - low + 1));
    }
    if (func == "float" && args.size() > 1) {
      double low = stod(args[0]);
      double high = stod(args[1]);
      double factor = (double)rand() / RAND_MAX;
      return to_string(low + factor * (high - low));
    }
    if (func == "choice" && !args.empty()) {
      string lst = args[0];
      size_t first = lst.find('[');
      size_t last = lst.find(']');
      if (first != string::npos && last != string::npos) {
        string inner = lst.substr(first + 1, last - first - 1);
        stringstream ss(inner);
        string el;
        vector<string> items;
        while (getline(ss, el, ',')) {
          string t = trim(el);
          if (t.front() == '"' && t.back() == '"') {
            t = t.substr(1, t.length() - 2);
          }
          items.push_back(t);
        }
        if (!items.empty()) return items[rand() % items.size()];
      }
      return "";
    }
    if (func == "uuid") {
      char buf[64];
      sprintf(buf, "%04x%04x-%04x-%04x-%04x-%04x%04x%04x", 
              rand() % 0xffff, rand() % 0xffff, rand() % 0xffff, 
              (rand() % 0x0fff) | 0x4000, (rand() % 0x3fff) | 0x8000, 
              rand() % 0xffff, rand() % 0xffff, rand() % 0xffff);
      return string(buf);
    }
  }

  if (mod == "crypto") {
    if (func == "md5" && !args.empty()) {
      unsigned char digest[MD5_DIGEST_LENGTH];
      MD5((const unsigned char*)args[0].c_str(), args[0].length(), digest);
      char buf[33];
      for (int i = 0; i < MD5_DIGEST_LENGTH; i++) sprintf(&buf[i*2], "%02x", digest[i]);
      return string(buf);
    }
    if (func == "sha256" && !args.empty()) {
      unsigned char digest[SHA256_DIGEST_LENGTH];
      SHA256((const unsigned char*)args[0].c_str(), args[0].length(), digest);
      char buf[65];
      for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) sprintf(&buf[i*2], "%02x", digest[i]);
      return string(buf);
    }
    if (func == "b64e" && !args.empty()) return base64_encode(args[0]);
    if (func == "b64d" && !args.empty()) return base64_decode(args[0]);
  }

  if (mod == "cli") {
    if (func == "clear") {
#ifdef _WIN32
      system("cls");
#else
      system("clear");
#endif
      return "1";
    }
    if (func == "pause") {
#ifdef _WIN32
      system("pause");
#else
      cout << "Press Enter to continue..." << endl;
      cin.get();
#endif
      return "1";
    }
    if (func == "print" && !args.empty()) {
      cout << args[0] << endl;
      return "1";
    }
    if (func == "red" && !args.empty()) return "\033[1;31m" + args[0] + "\033[0m";
    if (func == "green" && !args.empty()) return "\033[1;32m" + args[0] + "\033[0m";
    if (func == "blue" && !args.empty()) return "\033[1;34m" + args[0] + "\033[0m";
    if (func == "yellow" && !args.empty()) return "\033[1;33m" + args[0] + "\033[0m";
    if (func == "cyan" && !args.empty()) return "\033[1;36m" + args[0] + "\033[0m";
    if (func == "magenta" && !args.empty()) return "\033[1;35m" + args[0] + "\033[0m";
    if (func == "bold" && !args.empty()) return "\033[1m" + args[0] + "\033[0m";
  }

  return "";
}
