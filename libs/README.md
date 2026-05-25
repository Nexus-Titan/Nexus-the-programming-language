# Nexus Extension Libraries (v4.1.3)

This directory contains standard pre-built extension libraries for the Nexus programming language. Version 4.1.3 allows both native `.nx` modules and Python `.py` extensions to be imported and executed natively.

## Available Modules

- `fs.nx`: High-level filesystem queries (file listing, backups, cleanup).
- `logger.nx`: Standardized colored application logging with structured logs written to `app.log`.
- `json_db.nx`: Lightweight local database saving key-values as structured JSON.
- `http_client.nx`: High-level REST API clients wrapping standard `net` and `json` operations.
- `nx_suite.nx`: Graphic rectangle rendering and web content crawlers.
- `py_suite.py`: Data calculations and parallel threading extensions.
- `net_utils.py`: Operating system network port scanners and connection checks.
- `system_monitor.py`: Hardware statistics (CPU load, memory sizes, disk allocations).
- `ai_helper.py`: Local sentiment analytics and chat assistant.

## Usage

Modules are imported dynamically using the standard syntax:
```nexus
import "module_name.nx"
import "module_name.py"
```
Once imported, their functions can be called directly:
```nexus
import "logger.nx"
logger.info("Diagnostics successfully initialized.")
```
