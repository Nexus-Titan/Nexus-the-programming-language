#include "interpreter_core.h"

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
  
  char* wayland_display = getenv("WAYLAND_DISPLAY");
  if (wayland_display) {
    cout << "Nexus \u203a Wayland display server detected [" << wayland_display << "]" << endl;
    cout << "Nexus \u203a Booting dynamic client over high-performance XWayland compatibility layer..." << endl;
  }
  
  vector<string> l;
  string s;
  while (getline(f, s)) l.push_back(s);
  engine.run(l);
  return 0;
}
