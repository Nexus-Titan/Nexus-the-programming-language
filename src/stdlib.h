#pragma once
#include "interpreter_core.h"

string call_stdlib(const string &mod, const string &func, const vector<string> &args);

string execute_stdlib_basic(const string &mod, const string &func, const vector<string> &args, NexusUltraV4* engine);
string execute_stdlib_io(const string &mod, const string &func, const vector<string> &args, NexusUltraV4* engine);
string execute_stdlib_gui(const string &mod, const string &func, const vector<string> &args, NexusUltraV4* engine);
