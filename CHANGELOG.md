# Changelog

## [4.1.3] - 2026-05-25

### Rebuilt
- **Unified Zero-Dependency C++ Engine**: Fully redesigned the C++ interpreter core (`interpreter.cpp`) in standard C++11, compiling out of the box on macOS, Linux, and Windows without any external library dependencies.
- **300+ Standard Library Functions Restored**: Achieved 100% full feature parity with the legacy Python reference engines, restoring complete functionality for `api`, `math`, `str`, `sys`, `admin`, `io`, `zip`, `net`, `date`, `rnd`, `crypto`, `gui`, `json`, and `cli` modules.

### Added
- **Dynamic Python Script Bridge**: Implemented a dynamic sub-process bridge that parses Python scripts (`.py`), maps their functions, and executes them via the system Python environment, supporting all custom arguments natively.
- **Advanced Control Flow nesting**: Re-engineered nesting support for all nested structures (`if-else`, `loop`, and `while` loop conditions) using balanced keyword checks.
- **Native Break Support (`break`)**: Added support for loop breaking in all conditional loops.
- **JSON List & Array Handling**: Implemented standard list support inside the C++ interpreter (supporting JSON lists, dynamic size queries via `str.len`, and index retrievals via `str.at`).
- **Dynamic File Globing & Hashes**: Implemented advanced recursive file listing (`io.glob`) and native cryptographically secured hashing (`io.file_hash_sha256`, `io.file_hash_md5`).
- **Interactive Dual-Mode GUI**: Redesigned the `gui` module to attempt native alert/dialog windows via Zenity/AppleScript first, and gracefully fall back to beautifully formatted interactive CLI terminal dashboards if graphical display servers are not detected.
- **New Downloader & Extractor Sample**: Added `download_and_extract.nx` to demonstrate remote downloading using `net.download` and archive extraction utilizing OS-native `tar` via `sys.run`.

### Fixed
- **String Comparison Quote-Parsing Bug**: Replaced the backwards inline quote-toggling parser in `find_operator` with a robust left-to-right quote state pre-calculation map. This fixes a critical bug where operator boundaries (like `==`) ending in string literals were incorrectly parsed, causing conditions like `os == "Windows"` to evaluate to `true` on non-Windows environments.
- Fixed nested custom function argument parsing where arguments contained module function calls (e.g. `audit_system(sys.cwd())`).
- Fixed parsing termination bugs in `+add(libs)` block directives.

---

## [4.1.1] - Legacy V4 Release

- Standardized cross-engine PC and Web communication bridges.
- Added support for `while` loops in Python and JavaScript.

---

## [3.0.0] - Legacy V3 Release

- Added privilege escalation (`admin`), compression (`zip`), URL browser controls (`browser`), and cryptography (`crypto`).
- Expanded existing networking (`net`) and system monitor (`sys`) libraries.

---

## [2.0.0] - Legacy V2 Release

- Redesigned visual programming blocks in Nexus Studio.
- Implemented standard libraries (200+ math, string, system, and graphic functions).
