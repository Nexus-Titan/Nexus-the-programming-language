# Nexus Native Source Libraries (v4.1.3)

This directory contains source code for performance-critical and ecosystem-bridging libraries. Version 4.1.3 ensures seamless low-level cross-compilation and native execution integration.

## Languages Included

- **C++ (`native_core.cpp`)**: Physics body simulators and string search logic.
- **C (`native_core.cpp`)**: Data RLE compression and hardware IO mocks.
- **Rust (`rust_engine.rs`)**: Security vault and async XOR-32 encryption.
- **Assembly (`performance_bits.asm`)**: Vector math and bitwise hashing.
- **Java (`EnterpriseLib.java`)**: Enterprise reporting and SQL bridging mocks.

## Integration

These files are intended to be compiled into shared libraries (`.so` or `.dll`) for integration with the Nexus native C++ engine.
