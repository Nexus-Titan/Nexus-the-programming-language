# Nexus Titan 2.0 ULTRA Support

<div align="center">
  <h1>🚀 Nexus Titan 2.0 ULTRA</h1>
  <p><strong>The official VS Code extension for the Nexus Titan programming language.</strong></p>
</div>

---

Experience the power of **Nexus Titan 2.0 ULTRA** directly in Visual Studio Code. This extension provides full syntax highlighting, language support, and a custom Cyberpunk theme designed specifically for the `.nx` file format.

## ✨ Features

### 🎨 Syntax Highlighting
Full colorization for the Nexus 2.0 syntax, including:
- **200+ Built-in Functions** (`math.*`, `str.*`, `sys.*`, `gui.*`, etc.)
- **Control Flow Keywords** (`if`, `else`, `loop`, `fn`, `end`)
- **Strings & Variables**
- **Comments**

### 🌑 Nexus Titan Theme
Includes the exclusive **"Nexus Titan Dark"** color theme.
- High-contrast Cyberpunk/Neon aesthetics.
- Optimized for long coding sessions.
- Matches the official Nexus Titan branding.

## 📦 Installation

1. Download the `.vsix` file.
2. Open VS Code.
3. Go to the **Extensions** view (`Ctrl+Shift+X`).
4. Click the `...` menu in the top-right corner.
5. Select **"Install from VSIX..."**.
6. Choose the `nexus-titan-support-2.0.0.vsix` file.

## 📝 Usage

Simply open any file with the **`.nx`** extension. The syntax highlighting will activate automatically.

To use the theme:
1. Press `Ctrl+K` then `Ctrl+T`.
2. Select **"Nexus Titan Dark"**.

## 🔧 Example Code

```nexus
# Nexus Titan 2.0 Example
fn main()
    cli.clear()
    cli.cyan("Welcome to Nexus Titan 2.0 ULTRA")
    
    set radius = 10
    set area = math.pi() * math.pow(radius, 2)
    
    if area > 100
        out "Area is massive: " + area
        gui.msg("Calculation complete!")
    else
        out "Area is small."
    end
end
👨‍💻 Author

Tiwut

Website: tiwut.de

GitHub: github.com/tiwut

Powered by the Nexus Titan Engine.

---