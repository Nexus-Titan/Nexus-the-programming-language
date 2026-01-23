# Nexus Titan 2.0 ULTRA 🚀

The next-generation scripting language with **200+ built-in functions**, powerful control flow, and dual interpreters (Python & C++).

## 🌟 What's New in 2.0 ULTRA

- **200+ Functions**: Comprehensive standard library across 9 modules
- **Control Flow**: Full support for if-else statements and loops
- **Advanced Math**: Trigonometry, logarithms, constants, and more
- **String Manipulation**: 50+ string operations
- **System Integration**: OS info, shell commands, environment variables
- **Date & Time**: Complete date/time handling
- **Random Numbers**: Multiple random generation methods
- **Enhanced GUI**: Warnings, errors, and advanced dialogs
- **Backward Compatible**: All old scripts still work!

## 📦 Installation

Nexus 2.0 ULTRA offers **4 different interpreters** to choose from:

### Python Interpreter (Recommended for Development)
```bash
cd v2
python interpreter.py your_script.nx
```

### C++ Interpreter (For Production)
```bash
cd v2
g++ -o nexus interpreter.cpp -std=c++11
./nexus your_script.nx
```

### C Interpreter (For Embedded Systems)
```bash
cd v2
gcc -o nexus interpreter.c -lm
./nexus your_script.nx
```

### Rust Interpreter (Modern & Safe)
```bash
cd v2/rust-interpreter
cargo build --release
cargo run --release your_script.nx
```

## 📁 Project Structure

```
Nexus-the-programming-language/
├── v1/                    # Original Nexus 1.x (legacy)
├── v2/                    # Nexus 2.0 ULTRA - All interpreters
│   ├── interpreter.py     # Python interpreter
│   ├── interpreter.cpp    # C++ interpreter
│   ├── interpreter.c      # C interpreter
│   ├── rust-interpreter/  # Rust interpreter
│   │   ├── src/
│   │   │   └── main.rs
│   │   ├── Cargo.toml
│   │   └── README.md
│   └── README.md          # Interpreter documentation
├── Samples/               # Example programs
├── Nexus-Studio/          # Visual programming environment
├── index.html             # Project website
└── README.md              # This file
```


## 📚 Language Reference

### Core Commands
- `set variable = value` - Assign variables
- `out text` - Print output
- `input var "prompt"` - Get user input
- `wait seconds` - Pause execution
- `if condition ... else ... end` - Conditional logic
- `loop count ... end` - Repeat code
- `fn name() ... end` - Define function

### Math Module (50+ functions)
```nexus
math.pi()           # Returns π
math.e()            # Returns e
math.sin(x)         # Sine
math.cos(x)         # Cosine
math.tan(x)         # Tangent
math.sqrt(x)        # Square root
math.pow(x, y)      # Power
math.log(x)         # Natural logarithm
math.abs(x)         # Absolute value
math.ceil(x)        # Round up
math.floor(x)       # Round down
math.deg(rad)       # Radians to degrees
math.rad(deg)       # Degrees to radians
math.fact(n)        # Factorial
```

### String Module (50+ functions)
```nexus
str.len(s)          # String length
str.upper(s)        # Convert to uppercase
str.lower(s)        # Convert to lowercase
str.reverse(s)      # Reverse string
str.trim(s)         # Remove whitespace
str.find(s, sub)    # Find substring
str.replace(s, old, new)  # Replace text
str.split(s, delim) # Split into array
str.repeat(s, n)    # Repeat n times
```

### System Module (30+ functions)
```nexus
sys.os()            # Operating system name
sys.user()          # Current username
sys.time()          # Current timestamp
sys.cwd()           # Current directory
sys.shell(cmd)      # Execute shell command
sys.sleep(sec)      # Sleep for seconds
sys.info()          # Display system info
sys.env(key)        # Get environment variable
```

### I/O Module (30+ functions)
```nexus
io.read(file)       # Read file
io.write(file, data)  # Write to file
io.append(file, data) # Append to file
io.exists(path)     # Check if exists
io.size(file)       # Get file size
io.list(dir)        # List directory
io.remove(file)     # Delete file
io.copy(src, dst)   # Copy file
```

### GUI Module (25+ functions)
```nexus
gui.msg(text)       # Info message
gui.warn(text)      # Warning dialog
gui.err(text)       # Error dialog
gui.ask(text)       # Yes/No question
gui.input(prompt)   # Input dialog
gui.window(title)   # Create window
gui.label(text)     # Add label
gui.button(text, func)  # Add button
gui.run()           # Start GUI loop
```

### Date/Time Module (20+ functions)
```nexus
date.now()          # Current date/time
date.year()         # Current year
date.month()        # Current month
date.day()          # Current day
date.hour()         # Current hour
date.min()          # Current minute
date.sec()          # Current second
date.ts()           # Unix timestamp
```

### Random Module (20+ functions)
```nexus
rnd.int(min, max)   # Random integer
rnd.float(min, max) # Random float
rnd.val()           # Random 0-1
rnd.choice(list)    # Random element
rnd.seed(n)         # Set random seed
```

### CLI Module (20 functions)
```nexus
cli.clear()         # Clear terminal
cli.red(text)       # Red colored text
cli.green(text)     # Green colored text
cli.blue(text)      # Blue colored text
cli.yellow(text)    # Yellow colored text
```

## 🎯 Example Programs

### Calculator
```nexus
set radius = input "Enter radius: "
set area = math.pi() * math.pow(radius, 2)
out "Circle area: " + area
gui.msg("Calculation complete!")
```

### File Manager
```nexus
set filename = "data.txt"

if io.exists(filename)
    set content = io.read(filename)
    out "File contents: " + content
else
    out "File not found!"
    io.write(filename, "New file created")
end
```

### System Monitor
```nexus
out "=== System Info ==="
out "OS: " + sys.os()
out "User: " + sys.user()
out "Time: " + date.now()

loop 3
    set rand = rnd.int(1, 100)
    out "Random: " + rand
end
```

### Advanced Logic
```nexus
set x = 10
set y = 5

if x > y
    out "X is greater"
    set result = x * 2
else
    out "Y is greater"
    set result = y * 2
end

out "Result: " + result
```

## 🛠️ Nexus Studio

Visual block-based programming environment for Nexus. Access it at:
https://tiwut.github.io/Nexus-the-programming-language/Nexus-Studio/

Features:
- Drag-and-drop block programming
- Real-time code generation
- Support for all 200+ functions
- Import/Export .nx files
- Save/Load projects

## 📖 Documentation

Full documentation with searchable function reference available at:
https://tiwut.github.io/Nexus-the-programming-language/

## 🎮 Sample Applications

Check the `Samples/` directory for example programs:
- `calculator.nx` - Advanced calculator with trig functions
- `file_manager.nx` - File operations demo
- `system_monitor.nx` - System information display
- `guessing_game.nx` - Interactive number guessing game
- `ultra_test.nx` - Comprehensive feature showcase

## 🔧 Operators

### Arithmetic
- `+` Addition
- `-` Subtraction
- `*` Multiplication
- `/` Division

### Comparison
- `==` Equal
- `!=` Not equal
- `>` Greater than
- `<` Less than
- `>=` Greater or equal
- `<=` Less or equal

## 🚀 Performance

- **Python Interpreter**: Fast development, cross-platform
- **C++ Interpreter**: Maximum performance, native speed
- **Backward Compatible**: All v1.x scripts work in v2.0

## 📝 License

MIT License - See LICENSE file for details

## 👨‍💻 Author

Created by Tiwut
- Website: https://tiwut.de
- GitHub: https://github.com/tiwut

---

**Nexus Titan 2.0 ULTRA** - The bridge between high-speed execution and human-readable scripting.
