#include "stdlib.h"
#include <openssl/sha.h>
#include <openssl/md5.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <sys/stat.h>
#include <dirent.h>
#include <glob.h>
#endif

static void recursive_glob(const string &path, const string &ext, vector<string> &res) {
#ifdef _WIN32
#else
  DIR *dir = opendir(path.c_str());
  if (!dir) return;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    string name = entry->d_name;
    if (name == "." || name == "..") continue;
    string full = path + "/" + name;
    struct stat st;
    if (stat(full.c_str(), &st) == 0) {
      if (S_ISDIR(st.st_mode)) {
        recursive_glob(full, ext, res);
      } else {
        if (full.length() >= ext.length() && full.compare(full.length() - ext.length(), ext.length(), ext) == 0) {
          res.push_back(full);
        }
      }
    }
  }
  closedir(dir);
#endif
}

string execute_stdlib_io(const string &mod, const string &func, const vector<string> &args, NexusUltraV4* engine) {
  if (mod == "io") {
    if (func == "read" && !args.empty()) {
      ifstream in(args[0]);
      if (!in.is_open()) return "";
      stringstream ss;
      ss << in.rdbuf();
      return ss.str();
    }
    if (func == "write" && args.size() > 1) {
      ofstream out(args[0]);
      if (!out.is_open()) return "0";
      out.write(args[1].c_str(), args[1].length());
      return "1";
    }
    if (func == "append" && args.size() > 1) {
      ofstream out(args[0], ios::app);
      if (!out.is_open()) return "0";
      out.write(args[1].c_str(), args[1].length());
      return "1";
    }
    if (func == "read_lines" && !args.empty()) {
      ifstream in(args[0]);
      if (!in.is_open()) return "[]";
      string l;
      string res = "[";
      bool first = true;
      while (getline(in, l)) {
        if (!first) res += ", ";
        res += "\"" + escape_json_string(l) + "\"";
        first = false;
      }
      res += "]";
      return res;
    }
    if (func == "exists" && !args.empty()) {
      return to_string(access(args[0].c_str(), 0) == 0);
    }
    if (func == "size" && !args.empty()) {
      ifstream in(args[0], ios::binary | ios::ate);
      return in.is_open() ? to_string(in.tellg()) : "0";
    }
    if (func == "ext" && !args.empty()) {
      size_t dot = args[0].find_last_of('.');
      return dot == string::npos ? "" : args[0].substr(dot);
    }
    if (func == "base" && !args.empty()) {
      size_t slash = args[0].find_last_of("/\\");
      return slash == string::npos ? args[0] : args[0].substr(slash + 1);
    }
    if (func == "abs" && !args.empty()) {
      char buf[PATH_MAX];
#ifdef _WIN32
      _fullpath(buf, args[0].c_str(), sizeof(buf));
#else
      if (!realpath(args[0].c_str(), buf)) return args[0];
#endif
      return string(buf);
    }
    if (func == "mkdir" && !args.empty()) {
#ifdef _WIN32
      return to_string(_mkdir(args[0].c_str()) == 0);
#else
      return to_string(mkdir(args[0].c_str(), 0777) == 0);
#endif
    }
    if (func == "rmdir" && !args.empty()) {
#ifdef _WIN32
      return to_string(_rmdir(args[0].c_str()) == 0);
#else
      return to_string(rmdir(args[0].c_str()) == 0);
#endif
    }
    if (func == "isdir" && !args.empty()) {
      struct stat st;
      if (stat(args[0].c_str(), &st) == 0) return to_string(S_ISDIR(st.st_mode));
      return "0";
    }
    if (func == "isfile" && !args.empty()) {
      struct stat st;
      if (stat(args[0].c_str(), &st) == 0) return to_string(S_ISREG(st.st_mode));
      return "0";
    }
    if (func == "remove" && !args.empty()) return to_string(remove(args[0].c_str()) == 0);
    if (func == "glob" && args.size() >= 1) {
      string pat = args[0];
      size_t recursive = pat.find("/**/*");
      if (recursive != string::npos) {
        string base = pat.substr(0, recursive);
        string ext = pat.substr(recursive + 5);
        vector<string> items;
        recursive_glob(base, ext, items);
        string res = "[";
        for (size_t k = 0; k < items.size(); ++k) {
          if (k > 0) res += ", ";
          res += "\"" + escape_json_string(items[k]) + "\"";
        }
        res += "]";
        return res;
      }
#ifndef _WIN32
      glob_t g;
      if (glob(pat.c_str(), 0, NULL, &g) == 0) {
        string res = "[";
        for (size_t k = 0; k < g.gl_pathc; ++k) {
          if (k > 0) res += ", ";
          res += "\"" + escape_json_string(g.gl_pathv[k]) + "\"";
        }
        res += "]";
        globfree(&g);
        return res;
      }
#endif
      return "[]";
    }
    if (func == "list" && !args.empty()) {
      string dir_path = args[0];
      string res = "[";
#ifndef _WIN32
      DIR *dir = opendir(dir_path.c_str());
      if (dir) {
        struct dirent *entry;
        bool first = true;
        while ((entry = readdir(dir)) != NULL) {
          string name = entry->d_name;
          if (name == "." || name == "..") continue;
          if (!first) res += ", ";
          res += "\"" + escape_json_string(name) + "\"";
          first = false;
        }
        closedir(dir);
      }
#endif
      res += "]";
      return res;
    }
    if (func == "file_hash_sha256" && !args.empty()) {
      ifstream in(args[0], ios::binary);
      if (!in.is_open()) return "";
      SHA256_CTX ctx;
      SHA256_Init(&ctx);
      char buf[4096];
      while (in.read(buf, sizeof(buf))) SHA256_Update(&ctx, buf, in.gcount());
      SHA256_Update(&ctx, buf, in.gcount());
      unsigned char digest[SHA256_DIGEST_LENGTH];
      SHA256_Final(digest, &ctx);
      char out[65];
      for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) sprintf(&out[i*2], "%02x", digest[i]);
      return string(out);
    }
  }

  if (mod == "zip") {
    if (func == "list" && !args.empty()) {
#ifdef _WIN32
      return engine->execute_custom_function("run_shell", {"powershell -Command \"Expand-Archive -LiteralPath '" + args[0] + "' -DestinationPath temp; Get-ChildItem temp | Select-Object -ExpandProperty Name; Remove-Item temp -Recurse\""});
#else
      return engine->execute_custom_function("run_shell", {"unzip -l \"" + args[0] + "\""});
#endif
    }
  }

  if (mod == "json") {
    if (func == "get" && args.size() > 1) {
      string json_str = args[0];
      string key = args[1];
      size_t pos = json_str.find("\"" + key + "\"");
      if (pos != string::npos) {
        size_t colon = json_str.find(':', pos);
        if (colon != string::npos) {
          size_t start = json_str.find_first_not_of(" \t\r\n", colon + 1);
          if (start != string::npos) {
            if (json_str[start] == '"') {
              size_t end = json_str.find('"', start + 1);
              if (end != string::npos) return json_str.substr(start + 1, end - start - 1);
            } else {
              size_t end = json_str.find_first_of(",}", start);
              if (end != string::npos) return trim(json_str.substr(start, end - start));
            }
          }
        }
      }
      return "";
    }
    if (func == "set" && args.size() > 2) {
      string json_str = args[0];
      string key = args[1];
      string val = args[2];
      size_t pos = json_str.find("\"" + key + "\"");
      if (pos != string::npos) {
        size_t colon = json_str.find(':', pos);
        if (colon != string::npos) {
          size_t start = json_str.find_first_not_of(" \t\r\n", colon + 1);
          if (start != string::npos) {
            size_t end;
            if (json_str[start] == '"') {
              end = json_str.find('"', start + 1) + 1;
            } else {
              end = json_str.find_first_of(",}", start);
            }
            string replacement = "\"" + escape_json_string(val) + "\"";
            json_str.replace(start, end - start, replacement);
            return json_str;
          }
        }
      } else {
        size_t last_brace = json_str.find_last_of('}');
        if (last_brace != string::npos) {
          string extra = ", \"" + key + "\": \"" + escape_json_string(val) + "\"";
          if (json_str.find(':') == string::npos) extra = "\"" + key + "\": \"" + escape_json_string(val) + "\"";
          json_str.insert(last_brace, extra);
        }
      }
      return json_str;
    }
    if (func == "delete" && args.size() > 1) {
      string json_str = args[0];
      string key = args[1];
      size_t pos = json_str.find("\"" + key + "\"");
      if (pos != string::npos) {
        size_t end_val = json_str.find_first_of(",}", pos);
        if (end_val != string::npos) {
          if (json_str[end_val] == ',') {
            json_str.erase(pos, end_val - pos + 1);
          } else {
            size_t prev_comma = json_str.find_last_of(',', pos);
            if (prev_comma != string::npos) {
              json_str.erase(prev_comma, end_val - prev_comma);
            } else {
              json_str.erase(pos, end_val - pos);
            }
          }
        }
      }
      return json_str;
    }
    if (func == "str" && !args.empty()) return args[0];
    if (func == "load" && !args.empty()) {
      ifstream in(args[0]);
      if (!in.is_open()) return "{}";
      stringstream ss;
      ss << in.rdbuf();
      return ss.str();
    }
    if (func == "save" && args.size() > 1) {
      ofstream out(args[0]);
      if (!out.is_open()) return "0";
      out.write(args[1].c_str(), args[1].length());
      return "1";
    }
  }

  return "";
}
