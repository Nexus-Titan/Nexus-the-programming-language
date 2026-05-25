# Nexus Project Structure (v4.1.3)

Nexus version 4.1.3 is a highly streamlined, zero-dependency environment. All major modules, libraries, and compilation utilities are centered at the root level.

## Directory Overview

### Root Directory
- `interpreter.cpp`: The unified native C++ compiler and execution engine (v4.1.3).
- `download_and_extract.nx`: Demo script downloading a source tarball and extracting it via OS-native `tar` commands.
- `README.md`: The primary user manual and install guide.
- `SYNTAX.md`: The complete language reference guide.
- `CHANGELOG.md`: The detailed history of changes.
- `PROJECT_STRUCTURE.md`: This file.
- `LICENSE`: MIT License.

### `/libs` - Standard Extension Modules
This folder contains pre-built scripts and extensions that enrich the language capability:
- **Nexus Extensions**:
  - `logger.nx`: Standardized application and database logging.
  - `fs.nx`: Advanced filesystem query and management.
  - `json_db.nx`: High-performance JSON-based local database persistence.
  - `http_client.nx`: Comprehensive REST API client.
  - `nx_suite.nx`: Drawing tools and graphic/web crawlers.
- **Python Extensions** (Executed dynamically via the subprocess bridge):
  - `system_monitor.py`: Operating system and CPU performance stats.
  - `net_utils.py`: Network port scanners and connection diagnostics.
  - `ai_helper.py`: Local natural language assistant.
  - `py_suite.py`: Math modeling and parallel threading wrapper.
- **`/libs/src`** - Cross-Language Bridge Components:
  - `native_core.cpp`: Low-level RLE compression and physics structures.
  - `rust_engine.rs`: Rust encryption keys wrapper.
  - `EnterpriseLib.java`: Enterprise SQL builder mockup.
  - `performance_bits.asm`: Low-level arithmetic speed blocks.

### `/Samples` - Example Applications
- `calculator.nx`: Trigonometric and arithmetic console calculator.
- `file_manager.nx`: Directory scanning and configuration backups.
- `guessing_game.nx`: Random number guessing game with CLI box GUI.
- `text_analyzer.nx`: String manipulation and validation.
- `todo.nx`: Terminal database todo task list.
- **`/Samples/tests`** - Integration Test Suite:
  - `lib_check.nx`: Comprehensive health check of core library modules.
  - `master_check.nx`: Integration validation of all 20 language environments.
  - `api_demo.nx`: Web-to-PC API cross-call test.
  - `security_audit.nx`: Integrity glob checking with SHA-256 validation.

### `/nexus-vscode` - IDE Support
Official syntax highlighting extension and themed developer workspace:
- `syntaxes/nexus.tmLanguage.json`: TextMate grammar for VS Code.
- `themes/nexus-titan-theme.json`: Custom deep dark theme.
- `build.nx`: Dynamic package builder script.

## Getting Started

To build the native binary:
```bash
g++ -std=c++11 -Wall -O3 interpreter.cpp -o nexus
```
To run integration diagnostics:
```bash
./nexus Samples/tests/lib_check.nx
```
