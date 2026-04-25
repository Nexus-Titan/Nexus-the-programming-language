# Nexus Rust Interpreter

Modern, safe, and performant Rust implementation of the Nexus 2.0 ULTRA interpreter.

## Features

🦀 **Memory Safety**: No buffer overflows or memory leaks
🔒 **Thread Safety**: Safe concurrent execution
⚡ **Performance**: Comparable to C/C++
🛡️ **Type Safety**: Strong type system prevents errors
📦 **Modern**: Uses latest Rust ecosystem

## Installation

### Prerequisites
- Rust 1.70+ (install from https://rustup.rs/)

### Build

```bash
# Development build
cargo build

# Optimized release build
cargo build --release
```

## Usage

```bash
# Run directly with cargo
cargo run script.nx
cargo run --release script.nx

# Or use the compiled binary
./target/release/nexus script.nx
```

## Examples

```bash
# Run test script
cargo run --release ../test.nx

# Run ultra test
cargo run --release ../ultra_test.nx

# Run samples
cargo run --release ../Samples/calculator.nx
cargo run --release ../Samples/system_monitor.nx
```

## Supported Features

✅ **All Nexus 2.0 ULTRA features**:
- 200+ built-in functions
- Control flow (if-else, loops)
- Function definitions
- Math operations (35+ functions)
- String manipulation (50+ functions)
- System integration (30+ functions)
- File I/O (30+ functions)
- Date/Time handling (20+ functions)
- Random number generation (20+ functions)
- GUI dialogs (25+ functions)

## Dependencies

- **chrono**: Date and time functionality
- **rand**: Random number generation

## Performance

The Rust interpreter offers:
- **Fast startup**: Compiled binary loads instantly
- **Low memory**: Efficient memory usage
- **Safe execution**: No undefined behavior
- **Concurrent**: Ready for multi-threading

## Why Rust?

1. **Safety**: Prevents common bugs at compile time
2. **Speed**: Zero-cost abstractions
3. **Reliability**: Strong guarantees about correctness
4. **Modern**: Great tooling and ecosystem

## Development

```bash
# Check code
cargo check

# Run tests
cargo test

# Format code
cargo fmt

# Lint code
cargo clippy
```

## Cross-Platform

Works on:
- ✅ Windows
- ✅ Linux
- ✅ macOS
- ✅ BSD variants

## License

Same as main Nexus project (MIT)
