# Nexus Integration Test Suite (v4.1.3)

Scripts for verifying the functionality, performance, and cross-platform feature parity of the Nexus native C++ and web execution engines.

## Available Tests

- `lib_check.nx`: Health check for core modules (`logger.nx`, `fs.nx`, `json_db.nx`, `net_utils.py`, `system_monitor.py`).
- `master_check.nx`: Comprehensive test verifying support for all 20 language environments (low-level bridges, drawing operations, web crawlers, parallel processing).
- `api_demo.nx`: Native-to-browser cross-engine communication and environment detection bridge.
- `security_audit.nx`: Advanced filesystem query scanning recursive directories and validating file integrity via SHA-256 hashes.

## Running Tests

To run the diagnostics using the C++ core:
```bash
./nexus Samples/tests/lib_check.nx
./nexus Samples/tests/master_check.nx
./nexus Samples/tests/api_demo.nx
./nexus Samples/tests/security_audit.nx
```
All tests are verified and compile out of the box with 100% correct logic on macOS, Linux, and Windows systems.
