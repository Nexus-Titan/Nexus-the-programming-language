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

---

## `/v3` — Nexus 3.0 NEXUS

Contains all **Nexus 3.0 NEXUS interpreters** with 300+ total functions (200+ from V2 + 100+ new in V3).

### Interpreters
1. **interpreter.py** — Python reference engine
   - Best for: All platforms, richest feature set, development
   - Requires: Python 3.8+
   - Includes: All 4 new modules + all extended modules

2. **interpreter.cpp** — C++ engine (Linux/Windows)
   - Best for: Native performance on Linux and Windows
   - Requires: C++11 compiler

3. **interpreter_macos.cpp** — C++ engine dedicated to macOS
   - Best for: Native performance on macOS
   - Requires: clang++ / g++ on macOS

4. **interpreter.c** — Pure C engine
   - Best for: Embedded, minimal footprint, maximum portability
   - Requires: C99 compiler

5. **rust-interpreter/** — Rust engine
   - Best for: Safety-critical production deployments
   - Requires: Rust 1.70+ (cargo)

### New V3 Modules
| Module    | Functions | Purpose |
|-----------|:---------:|---------|
| `admin`   | 6         | Sudo/root/admin privilege elevation |
| `zip`     | 10+       | ZIP/TAR archive compress & extract |
| `browser` | 4         | Launch URLs in system browser |
| `crypto`  | 7         | Hashing (MD5/SHA/etc.) and Base64 |

### Extended V3 Modules
| Module | New Functions |
|--------|:-------------:|
| `sys`  | 16 new        |
| `io`   | 12 new        |
| `net`  | 7 new         |
| `date` | 7 new         |
| `rnd`  | 3 new         |
| `gui`  | 10 new        |
| `cli`  | 10 new        |

### All V3 interpreters support:
- ✅ All V1 scripts (100% backward compatible)
- ✅ All V2 scripts (100% backward compatible)
- ✅ 300+ built-in functions total
- ✅ `admin`, `zip`, `browser`, `crypto` modules
- ✅ Cross-platform execution (Linux, macOS, Windows)

---

## Updated Interpreter Comparison

| Feature              | Python | C++ | C | Rust |
|----------------------|:------:|:---:|:-:|:----:|
| **Ease of Use**      | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Performance**      | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Safety**           | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Portability**      | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **V3 Full Support**  | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |

**All 4 V3 interpreters are in `/v3` directory!**

---

## Full Version Compatibility Matrix

| Script Version | V1 Engine | V2 Engine | V3 Engine | V4 Engine |
|----------------|:---------:|:---------:|:---------:|:---------:|
| V1 scripts     | ✅        | ✅        | ✅        | ✅        |
| V2 scripts     | ❌        | ✅        | ✅        | ✅        |
| V3 scripts     | ❌        | ❌        | ✅        | ✅        |
| V4 scripts     | ❌        | ❌        | ❌        | ✅        |

---

## `/v4` — Nexus 4.0 UNIFIED

Contains the **Nexus 4.0 UNIFIED interpreters** with a focus on cross-engine parity and a real API bridge between PC and Web.

### Key Innovations
1. **The `api` Module**: A shared interface across all engines.
   - `api.type()`: Identify environment (PC/Web).
   - `api.engine()`: Identify backend (Python/JS/CPP).
   - `api.call()`: Standardized cross-engine communication.
2. **Unified Core Engine**: The same `.nx` code now runs natively in the browser via `interpreter.js`.
3. **Advanced Control Flow**: Added `while` loop support and improved `if-else` nesting.

### Interpreters
1. **interpreter.py**: Reference implementation for PC.
2. **interpreter.js**: Native Web engine, integrated into NEXUS-WA.
3. **interpreter.cpp**: High-performance native engine for Linux/Windows.

---

**Nexus Titan 4.0 UNIFIED** — One Language, Any Platform.
