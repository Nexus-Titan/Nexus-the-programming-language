#pragma once
#include "interpreter_core.h"

#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

string show_x11_dialog(const string &type, const string &text);
