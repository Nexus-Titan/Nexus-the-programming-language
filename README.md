# Nexus Programming Language

Nexus is a cross-platform, multi-engine programming language designed for seamless integration between native PC environments and web-based platforms.

## Install on Mac & Linux
```bash
brew tap tiwut/tab https://github.com/tiwut/homebrew-tap.git
brew update
brew install nexus-titan
```
run with:
```bash
nexus
```

## Project Structure

- v4: The latest version of the Nexus language (Unified V4).
- legacy: Contains previous versions (v1, v2, v3) for backward compatibility.

## Key Features

- Unified API across Python, C++, and JavaScript.
- Support for multiple library ecosystems (Nexus, Python, C++, Rust, Java).
- High-performance execution core.
- Integrated Web IDE support via NEXUS-WA.

## Getting Started

To run a Nexus script on PC:
python3 v4/interpreter.py script.nx

To build the C++ engine:
g++ v4/interpreter.cpp -o nexus
