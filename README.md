# Nexus Programming Language (v4.1.3)

Nexus is a cross-platform, high-performance, multi-engine programming language designed for seamless integration between native PC environments and web-based platforms. Version 4.1.3 unites all legacy features (300+ standard library functions) into a unified, zero-dependency C++ core engine with lightning-fast execution speed and robust dynamic features.

## Install on Mac & Linux
```bash
brew tap Nexus-Titan/tab https://github.com/Nexus-Titan/homebrew-tap.git
brew update
brew install nexus4
```
Run with:
```bash
nexus4 script.nx
```

## Install with `yay` (Arch Linux)
```bash
yay -S nexus4-git
```

## Key Features

- **Zero-Dependency C++ Core**: Rebuilt completely from scratch using standard C++11 headers, meaning no external C++ packages need to be installed.
- **300+ Built-In Functions**: Complete parity with all previous versions, covering `api`, `math`, `str`, `sys`, `admin`, `io`, `zip`, `net`, `date`, `rnd`, `crypto`, `gui`, `json`, and `cli` modules.
- **Dynamic Module Bridge**: Seamlessly import and execute native `.nx` scripts and python `.py` extensions natively via a smart C++ subprocess bridge.
- **Advanced Control Flow**: Support for nested `if-else` blocks, standard loop counters, dynamic `while` loops, and standard scope-controlled function argument parameters and return statements.
- **Interactive Fallback GUI**: Native OS alert dialogs using Zenity/AppleScript with a highly styled CLI terminal-based fallback for headless/server configurations.

## Project Structure

- `interpreter.cpp`: The unified native C++ execution engine (v4.1.3).
- `download_and_extract.nx`: Demo script downloading a source tarball and extracting it via OS-native `tar` commands.
- `libs/`: Standard library extensions and module components.
  - `logger.nx`, `fs.nx`, `json_db.nx`, `http_client.nx`, `nx_suite.nx`: Native Nexus modules.
  - `ai_helper.py`, `system_monitor.py`, `net_utils.py`, `py_suite.py`: Dynamic Python script modules.
  - `src/`: Java, assembly, C, and Rust bridge components.
- `Samples/`: Demonstration files and test suites.
  - `tests/`: Integration tests (`lib_check.nx`, `master_check.nx`, `api_demo.nx`, `security_audit.nx`).
- `nexus-vscode/`: Official Visual Studio Code syntax highlighter and dark theme.

## Getting Started

To compile the C++ engine from source:
```bash
g++ -std=c++11 -Wall -O3 interpreter.cpp -o nexus
```

To run a script:
```bash
./nexus Samples/tests/lib_check.nx
```
