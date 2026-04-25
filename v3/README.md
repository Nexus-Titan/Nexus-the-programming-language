# Nexus Version 3
Welcome to the Nexus V3 Interpreter!

Nexus V3 is the biggest update yet, adding over 100 new built-in functions while keeping **100% backward compatibility** with V1 and V2 scripts.

## Key Features
* **Multi-Language Engines:** V3 comes with implementations in **Python**, **C**, **C++**, and **Rust**!
* **Cross-Platform:** The engine and all `.nx` scripts will run on Linux, macOS, and Windows exactly the same way without requiring any code changes!
* **100+ System Functions:** Vastly expanded system tools for environment variables, disks, platform info, and runtime contexts.
* **Privilege Elevation (Sudo / Admin):** Added `admin` library to run commands with admin privileges (uses `sudo` on Linux/Mac, `ShellExecute` on Windows).
* **ZIP Archive Support:** Compress, extract, list, and read inside ZIP files right from your script.
* **Web & Browser Functions:** Added `browser` module to manipulate the system web browser, and expanded `net` functions to handle HTTP GET/POST, headers, and connectivity checks.
* **Crypto Library:** Hash strings (MD5, SHA1, SHA256) and handle base64 encoding seamlessly.
* **File System Operations:** Full recursive copy `copytree`, `glob`, file hashing, `symlink`, `chmod`, `chown`, and temporary directories.

## V3 Modules & Built-ins
In addition to the classic `math`, `str`, `date`, `gui`, `json`, `rnd`, and `cli` modules, V3 brings highly requested tools:

### `admin` Module
* `admin.is_admin()` - Returns true if the script is running with elevated privileges.
* `admin.elevate()` - Restarts the script with admin/sudo privileges.
* `admin.run_admin(cmd)` - Runs a bash/cmd string with elevated privileges.
* `admin.get_uid()`, `admin.get_gid()` - Returns Linux/Mac specific user/group ID.

### `zip` Module
* `zip.compress(src, dst)` - Compresses a directory/file to a `.zip`.
* `zip.extract(src, dst)` - Extracts a `.zip` archive.
* `zip.list(src)` - Returns a list of files in a `.zip`.
* `zip.read_file(zip_path, file)` - Reads the content of a file inside the archive.
* `zip.add_file(zip_path, file_path)` - Adds a file to an existing zip.

### `browser` Module
* `browser.open(url)` - Opens URL in default browser.
* `browser.open_new(url)` - Opens URL in a new window.
* `browser.open_tab(url)` - Opens URL in a new tab.

### `net` Module (Extended)
* `net.post(url, data)` - Sends an HTTP POST request.
* `net.request_headers(url)` - Gets headers from a URL.
* `net.get_json(url)` - Returns parsed JSON directly from an endpoint.
* `net.ip_local()` - Returns the local IPv4 address.
* `net.is_connected()` - Returns True if internet is active.

### `io` & `sys` Modules (Extended)
* `io.copytree(src, dst)` - Copies an entire directory recursively.
* `io.file_hash_md5(file)`, `io.file_hash_sha256(file)` - Gets the hash of a file.
* `sys.mem_total()`, `sys.mem_used()`, `sys.mem_free()` - Gets disk memory usage.
* `sys.getenv(k)`, `sys.setenv(k, v)` - Better env var manipulation.
* `sys.whoami()` - Gets the current shell user.
* `sys.boot_time()` - Gets system boot timestamp.

*(See `interpreter.py`, `interpreter.c`, `interpreter.cpp`, or the Rust `src/main.rs` for the full list of all 100+ functions!)*

## Building Platform-Specific Interpreters

### Python Binary
Since Python provides the most comprehensive standard library, we recommend it as the reference engine. To create a standalone binary (`nexus.exe` for Windows, or `nexus` for Linux/Mac) without requiring Python:

```bash
pip install pyinstaller
pyinstaller --onefile --name nexus interpreter.py
```

### C / C++
You can compile the C or C++ interpreters natively on any OS:
```bash
gcc interpreter.c -o nexus -lm
g++ interpreter.cpp -o nexus
```
*(On macOS, compile `interpreter_macos.cpp` instead)*

### Rust
Navigate to `rust-interpreter` and use Cargo to build:
```bash
cd rust-interpreter
cargo build --release
```
