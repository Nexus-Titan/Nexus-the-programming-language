# Nexus Project Structure

## Directory Overview

### `/v1` - Legacy Version
Contains the original Nexus 1.x implementation for backward compatibility.

**Files:**
- Original interpreter and examples
- Legacy documentation

**Status:** Maintained for compatibility, not actively developed

---

### `/v2` - Nexus 2.0 ULTRA
Contains **all Nexus 2.0 ULTRA interpreters** with 200+ functions.

**Interpreters:**
1. **interpreter.py** - Python implementation
   - Best for: Development, testing, rapid prototyping
   - Requires: Python 3.7+
   
2. **interpreter.cpp** - C++ implementation
   - Best for: Production, performance-critical applications
   - Requires: C++11 compiler (g++, clang++, MSVC)
   
3. **interpreter.c** - C implementation
   - Best for: Embedded systems, maximum portability
   - Requires: C99 compiler (gcc, clang)

4. **rust-interpreter/** - Rust implementation
   - Best for: Modern applications requiring safety guarantees
   - Requires: Rust 1.70+ (cargo)
   - Structure:
     ```
     rust-interpreter/
     ├── src/
     │   └── main.rs
     ├── Cargo.toml
     └── README.md
     ```

**All interpreters support:**
- ✅ 200+ built-in functions
- ✅ Control flow (if-else, loops)
- ✅ Function definitions
- ✅ All 9 standard library modules
- ✅ 100% backward compatible with v1

---

### `/Samples` - Example Programs
Collection of example Nexus scripts demonstrating features.

**Available Examples:**
- `calculator.nx` - Advanced calculator with trigonometry
- `file_manager.nx` - File I/O operations
- `system_monitor.nx` - System information display
- `guessing_game.nx` - Interactive game
- `text_analyzer.nx` - String manipulation showcase

---

### `/Nexus-Studio` - Visual Programming
Web-based block programming environment.

**Features:**
- Drag-and-drop block programming
- Real-time code generation
- Support for all 200+ functions
- Import/Export .nx files
- Save/Load projects

**Access:** Open `index.html` in browser or visit online version

---

### Root Files

**Documentation:**
- `README.md` - Main project documentation
- `CHANGELOG.md` - Detailed version history
- `LICENSE` - MIT License

**Website:**
- `index.html` - Project website with searchable documentation
- `main.avif` - Background image

**Scripts:**
- `test.nx` - Original test script (v1 compatible)
- `ultra_test.nx` - Comprehensive v2 feature showcase

---

## Quick Start Guide

### For Beginners
1. Start with **Python interpreter** (`v2/interpreter.py`)
2. Try the example scripts in `/Samples`
3. Use **Nexus-Studio** for visual programming

### For Production
1. Use **C++ interpreter** (`v2/interpreter.cpp`) for best performance
2. Or use **Rust interpreter** (`v2/rust-interpreter`) for safety + performance
3. Compile to native binary for distribution

### For Embedded Systems
1. Use **C interpreter** (`v2/interpreter.c`)
2. Minimal dependencies
3. Small binary footprint

---

## Choosing an Interpreter

| Feature | Python | C++ | C | Rust |
|---------|--------|-----|---|------|
| **Ease of Use** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Performance** | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Safety** | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Portability** | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Binary Size** | N/A | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Development Speed** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |

**All 4 interpreters are in `/v2` directory!**

---

## Version Compatibility

- **v1 scripts** → Work on all v2 interpreters ✅
- **v2 scripts** → Work on all v2 interpreters ✅
- **All interpreters** → Produce identical output ✅

---

## Contributing

- `/v2/README.md` - Documentation for all 4 interpreters
- `/v2/rust-interpreter/README.md` - Rust-specific details

---

**Nexus Titan 2.0 ULTRA** - Choose the interpreter that fits your needs!
