# Nexus Titan IDE Support (v4.1.3)

<div align="center">
  <h1>🚀 Nexus VS Code Support (v4.1.3)</h1>
  <p><strong>The official VS Code extension for the Nexus Titan programming language.</strong></p>
</div>

---

Experience the power of **Nexus Titan v4.1.3** directly in Visual Studio Code. This extension provides full syntax highlighting, language support, and a custom Cyberpunk theme designed specifically for the `.nx` file format.

## ✨ Features

### 🎨 Syntax Highlighting
Full colorization for the Nexus v4.1.3 syntax, including:
- **300+ Built-in Functions** (`math.*`, `str.*`, `sys.*`, `gui.*`, `zip.*`, `net.*`, `crypto.*`, `json.*`, `admin.*`, `cli.*`, etc.)
- **Control Flow Keywords** (`if`, `else`, `loop`, `while`, `fn`, `end`, `break`, `return`, `import`)
- **Strings, Numbers & Variable Assignments**
- **Unified Comments** (`#`)

### 🌑 Nexus Titan Theme
Includes the exclusive **"Nexus Titan Dark"** color theme.
- High-contrast Cyberpunk/Neon aesthetics.
- Optimized for long coding sessions.
- Matches the official Nexus Titan branding.

## 📦 Installation

1. Download the `.vsix` package.
2. Open VS Code.
3. Go to the **Extensions** view (`Ctrl+Shift+X`).
4. Click the `...` menu in the top-right corner.
5. Select **"Install from VSIX..."**.
6. Choose the extension file `nexus-titan-support-2.0.0.vsix` (upgraded for v4.1.3 parsing compatibility).

## 📝 Usage

Simply open any file with the **`.nx`** extension. The syntax highlighting will activate automatically.

To use the theme:
1. Press `Ctrl+K` then `Ctrl+T`.
2. Select **"Nexus Titan Dark"**.

## 🔧 Example Code

```nexus
# Nexus Titan v4.1.3 Example
fn calculate_statistics(radius)
    cli.clear()
    cli.cyan("Welcome to Nexus Titan v4.1.3")
    
    set area = math.pi() * math.pow(radius, 2)
    
    if area > 100
        out "Area is massive: " + area
        gui.msg("Calculation complete!")
    else
        out "Area is small."
    end
    return area
end

set res = calculate_statistics(10)
```

👨‍💻 **Author**: Tiwut
- **Website**: [tiwut.de](https://tiwut.de)
- **GitHub**: [github.com/tiwut](https://github.com/tiwut)

Powered by the Nexus Titan C++ Engine.