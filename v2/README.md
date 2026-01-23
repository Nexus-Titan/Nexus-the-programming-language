# Nexus 2.0 ULTRA Interpreters

This directory contains all interpreters for **Nexus Titan 2.0 ULTRA**.

## Available Interpreters

### 1. Python Interpreter (`interpreter.py`)
**Recommended for development and testing**

```bash
python interpreter.py script.nx
```

**Features:**
- Fast development cycle
- Full stdlib support (200+ functions)
- Cross-platform
- Easy debugging

### 2. C++ Interpreter (`interpreter.cpp`)
**Recommended for production**

```bash
g++ -o nexus interpreter.cpp -std=c++11
./nexus script.nx
```

**Features:**
- High performance
- Native compilation
- Small binary size
- Cross-platform (Windows/Linux/macOS)

### 3. C Interpreter (`interpreter.c`)
**Recommended for embedded systems**

```bash
gcc -o nexus interpreter.c -lm
./nexus script.nx
```

**Features:**
- Maximum portability
- Minimal dependencies
- Pure C99 standard
- Lightweight

### 4. Rust Interpreter (`rust-interpreter/`)
**Recommended for modern, safe applications**

```bash
cd rust-interpreter
cargo build --release
cargo run --release ../script.nx
```

**Features:**
- Memory safety guaranteed
- Thread-safe by design
- Performance comparable to C/C++
- Modern tooling (cargo)
- Zero-cost abstractions

## All Features Supported

✅ **200+ Functions** across 9 modules
✅ **Control Flow**: if-else, loops
✅ **Math**: Trigonometry, logarithms, constants
✅ **Strings**: 50+ manipulation functions
✅ **System**: OS info, shell commands
✅ **I/O**: File operations
✅ **Date/Time**: Complete time handling
✅ **Random**: Number generation
✅ **GUI**: Dialog boxes

## Compatibility

All three interpreters are **100% compatible** and support:
- All Nexus 1.x scripts (backward compatible)
- All Nexus 2.0 ULTRA features
- Same syntax and semantics
- Identical output

Choose the interpreter based on your needs:
- **Development**: Python
- **Production**: C++
- **Embedded**: C
