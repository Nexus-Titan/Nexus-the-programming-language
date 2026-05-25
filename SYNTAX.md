# Nexus Language Syntax Guide (v4.5.0)

Nexus is a dynamically typed, procedural, and highly portable scripting language. Version 4.5.0 integrates a standard zero-dependency C++ core engine with lightning-fast execution, a secure native socket-based HTTP/HTTPS client using POSIX/OpenSSL and Windows WinINet, and a premium native C++ X11/Win32 window drawing suite.

---

## 1. File Extension
All Nexus source files use the `.nx` extension.

---

## 2. Variables & Assignments
Variables are declared and assigned dynamically using the `set` keyword:
```nexus
set name = "Nexus V4"
set version = 4.5.0
set is_native = 1
```

Constants are provided by the `math` module:
- `math.pi()` -> `3.141592653589793`
- `math.e()` -> `2.718281828459045`
- `math.tau()` -> `6.283185307179586`

---

## 3. Basic Input & Output
- **Output**: The `out` keyword prints an expression to standard output, prepended with `Nexus › `:
  ```nexus
  out "Welcome to " + name
  ```
- **Input**: The `input` keyword prompts for terminal input:
  ```nexus
  input user_name "Enter your name"
  out "Hello, " + user_name
  ```

---

## 4. Control Flow

### Conditionals (`if` / `else` / `end`)
Supports standard comparison operators (`==`, `!=`, `<`, `>`, `<=`, `>=`) and logical operators (`&&`, `||`):
```nexus
if age >= 18 && is_authorized == 1
    out "Access granted."
else
    out "Access denied."
end
```

### Loops & Breaking
1. **Fixed-Count Loop (`loop` / `end`)**:
   ```nexus
   set count = 5
   loop count
       out "Iteration!"
   end
   ```
2. **Conditional Loop (`while` / `end`)**:
   ```nexus
   set index = 0
   while index < 10
       out "Index is: " + index
       set index = index + 1
   end
   ```
3. **Loop Breaking (`break`)**:
   ```nexus
   set i = 0
   while i < 100
       if i == 5
           break
       end
       set i = i + 1
   end
   ```

---

## 5. Functions & Return Mappings
Functions are declared using `fn` and end with `end`. They support local scoping, dynamic parameter mapping, and `return` values:
```nexus
fn calculate_area(radius)
    set pi_val = math.pi()
    set area = pi_val * math.pow(radius, 2)
    return area
end

set res = calculate_area(10)
out "Area: " + res
```

---

## 6. List & Array Mappings
Lists are mapped as JSON array strings. Standard list utility functions handle parsing and index retrieval:
```nexus
set my_list = "[\"apple\", \"banana\", \"cherry\"]"
set total = str.len(my_list)      # Returns 3
set first = str.at(my_list, 0)    # Returns "apple"
```

---

## 7. Dynamic Module Imports
Import native `.nx` modules or dynamic `.py` scripts seamlessly:
```nexus
import "logger.nx"
logger.info("System diagnostics initialized.")

import "ssl.nx"
set page = ssl.get("https://tiwut.org/nexus")
```

---

## 8. Complete Standard Library Reference (300+ Functions)

### 8.1. Math (`math.*`)
- **Constants**: `pi()`, `e()`, `tau()`, `inf()`, `nan()`
- **Trigonometry**: `sin(x)`, `cos(x)`, `tan(x)`, `asin(x)`, `acos(x)`, `atan(x)`, `atan2(y, x)`, `sinh(x)`, `cosh(x)`, `tanh(x)`, `asinh(x)`, `acosh(x)`, `atanh(x)`
- **Rounding**: `ceil(x)`, `floor(x)`, `round(x)`, `trunc(x)`
- **Operations**: `sqrt(x)`, `cbrt(x)`, `pow(x, y)`, `abs(x)`, `min(a,b)`, `max(a,b)`, `hypot(x,y)`, `gcd(a,b)`, `lcm(a,b)`, `mod(a,b)`, `fact(x)`, `perm(n,k)`, `comb(n,k)`, `clamp(val,min,max)`, `lerp(a,b,t)`, `sign(x)`
- **Utility**: `deg(rad)`, `rad(deg)`, `is_nan(x)`, `is_inf(x)`, `remainder(a,b)`, `erf(x)`, `erfc(x)`, `gamma(x)`, `lgamma(x)`, `is_close(a,b)`, `fsum(list)`, `prod(list)`, `dist(p1,p2)`

### 8.2. Strings (`str.*`)
- **Formatting**: `upper(s)`, `lower(s)`, `cap(s)`, `trim(s)`, `ltrim(s)`, `rtrim(s)`, `reverse(s)`, `repeat(s, n)`, `zfill(s, n)`, `swap(s)`, `title(s)`, `replace(s, old, new)`, `split(s, delim)`, `join(delim, list_str)`, `pad_left(s, width, pad)`, `pad_right(s, width, pad)`
- **Extraction**: `at(s, index)`, `slice(s, start, end)`, `partition(s, delim)`, `rpartition(s, delim)`
- **Searching**: `find(s, query)`, `rfind(s, query)`, `starts(s, query)`, `ends(s, query)`, `count(s, query)`, `contains(s, query)`
- **Checking**: `isnum(s)`, `isalpha(s)`, `isalnum(s)`, `isspace(s)`, `islower(s)`, `isupper(s)`, `len(s)`
- **Casting**: `hex(n)`, `bin(n)`, `ascii(s)`, `chr(n)`, `ord(c)`

### 8.3. System Environment (`sys.*`)
- **Environment**: `os()`, `ver()`, `arch()`, `user()`, `cwd()`, `setcwd(dir)`, `env(key)`, `setenv(k,v)`, `delenv(k)`, `whoami()`, `clear()`, `hostname()`
- **Process**: `exit(code)`, `pid()`, `cpu_count()`, `sleep(sec)`, `time()`, `info()`
- **Execution**: `shell(cmd)` (captures output), `run(cmd)` (returns exit status code)
- **Memory**: `mem_total()`, `mem_used()`, `mem_free()`

### 8.4. File Input & Output (`io.*`)
- **File Access**: `read(file)`, `write(file, data)`, `append(file, data)`, `read_lines(file)`, `write_lines(file, list)`
- **Validation**: `exists(path)`, `isdir(path)`, `isfile(path)`
- **CRUD Operations**: `mkdir(dir)`, `rmdir(dir)`, `remove(file)`, `rename(old, new)`, `copy(src, dst)`, `move(src, dst)`, `touch(file)`, `chmod(file, mode)`, `glob(pattern)`, `list(dir)`
- **Metadata**: `size(file)`, `ext(path)`, `base(path)`, `abs(path)`, `join(p1, p2)`, `split(path)`, `dirname(path)`, `home()`, `tmp()`
- **Crypto Hashes**: `file_hash_sha256(file)`, `file_hash_md5(file)`

### 8.5. Archives (`zip.*`)
- **Compression**: `compress(src, dst)`, `extract(src, dst)`, `list(src)`

### 8.6. Networking (`net.*`)
- **HTTP Clients**: `get(url)`, `post(url, data)`, `download(url, file)`, `get_json(url)`
- **Diagnostics**: `ping(host)`, `ip()`, `is_online()`

### 8.7. Date & Time (`date.*`)
- **Current Values**: `now()`, `today()`, `iso()`, `ts()`
- **Date Splits**: `year()`, `month()`, `day()`, `hour()`, `min()`, `sec()`
- **Formattings**: `fmt(format_str)`, `sleep_ms(ms)`

### 8.8. Random Numbers (`rnd.*`)
- **Random Values**: `val()`, `int(min, max)`, `float(min, max)`
- **Choice Actions**: `choice(list)`, `shuffle(list)`, `uuid()`

### 8.9. Cryptography (`crypto.*`)
- **Hashes**: `md5(str)`, `sha256(str)`
- **Base64 Codec**: `b64e(str)`, `b64d(str)`

### 8.10. JSON Module (`json.*`)
- **Handling**: `parse(str)`, `str(obj)`, `load(file)`, `save(file, data)`, `pretty(data)`, `get_keys(data)`
- **Storage Modifiers**:
- `get(json_str, key)`: Extracts a field's value from a JSON string.
- `set(json_str, key, value)`: Updates or adds a field's value in a JSON string.
- `delete(json_str, key)`: Deletes a key-value pair from a JSON string.

### 8.11. Secure SSL Sockets Library (`ssl.*`)
*(Requires import "ssl.nx")*
- **Request Handlers**:
- `get(url)`: Performs a native secure HTTP/HTTPS GET query.
- `post(url, data)`: Performs a native secure HTTP/HTTPS POST query.
- `download(url, filepath)`: Synchronously fetches remote data and writes it to a file.

### 8.12. Modern Native C++ GUI Module (`gui.*`)
- **Window Setup**: 
  - `window(title)` or `window(title, geometry)`: Registers main window specs.
  - `geom(width_height_str)`: Configures window size (e.g. `"500x400"`).
  - `color(color_name)`: Configures light/dark mode themes (e.g. `"white"`).
  - `bg_color(hex_color)`: Directly configures the main window background HEX color (e.g. `"#1f2937"`).
  - `canvas_color(hex_color)`: Directly configures the active web engine/drawing canvas background HEX color (e.g. `"#111827"`).
  - `font(family, size)`: Configures default GUI font properties (e.g. `gui.font("helvetica", "14")`).
  - `position(x, y)`: Sets the launch coordinates of the window frame on screen.
  - `resizable(bool)`: Locks (`"0"`) or unlocks (`"1"`) the resizability of the active frame.
  - `run()`: Starts the native synchronous graphical event loop.
  - `clear()`: Wipes active elements/drawings.
- **Widgets**:
  - `label(text)`: Renders flat read-only header or message blocks.
  - `button(text, callback_func)`: Renders custom-padded widgets with dynamic hover states.
  - `entry(name, default_val)`: Registers custom key entry text inputs.
  - `get_entry(name)`: Retrieves the input value corresponding to the name dynamically.
- **Graphics Canvas**:
  - `draw_rect(x, y, w, h, color)`: Paints a filled rectangle.
  - `draw_rect_outline(x, y, w, h, color)`: Paints an unfilled outline rectangle.
  - `draw_rounded_rect(x, y, w, h, radius, color)`: Paints a filled rounded rectangle with custom corner radius.
  - `draw_rounded_rect_outline(x, y, w, h, radius, color)`: Paints an unfilled outline rounded rectangle.
  - `draw_blur(x, y, w, h, intensity)`: Soft software box blur overlaying dynamic canvas pixels in the specified rectangle bounding box.
  - `draw_gradient(x, y, w, h, color1, color2, dir)`: Interpolates colors between `color1` and `color2` dynamically. `dir` sets gradient angle direction (`"0"` = horizontal, `"1"` = vertical).
  - `draw_triangle(x1, y1, x2, y2, x3, y3, color)`: Paints a filled triangle polygon with three coordinates pairs.
  - `draw_triangle_outline(x1, y1, x2, y2, x3, y3, color)`: Paints an unfilled outline triangle.
  - `draw_line(x1, y1, x2, y2, color)`: Paints a line connecting two coordinate pairs.
  - `draw_circle(x, y, r, color)`: Paints a filled circle of radius `r` centered at `(x, y)`.
  - `draw_circle_outline(x, y, r, color)`: Paints an unfilled outline circle.
  - `draw_text(x, y, text, color)`: Paints high-performance transparent text directly on the canvas.
- **Pure Native Dialogs**:
  - `msg(text)`: Displays a native info popup window.
  - `warn(text)`: Displays a native alert warning window.
  - `err(text)`: Displays a native alert error window.
  - `ask(question)`: Displays a yes/no dialog (returns `"1"` or `"0"`).
  - `input(prompt)`: Prompts user inside a native text box dialog (returns string).

### 8.13. Colored Console CLI (`cli.*`)
- **Control**: `clear()`, `pause()`, `print(text)`
- **Ansi Colors**: `red(s)`, `green(s)`, `blue(s)`, `yellow(s)`, `cyan(s)`, `magenta(s)`, `reset(s)`, `bold(s)`

---

## 8.14. Native Zero-Dependency Web Engine & Layout Compiler (`web.*`)

Expose premium HTML5/CSS compilation methods:
- `render(html_string)`: Natively compiles and draws HTML layout strings.
- `load(url)`: Fetches a remote address synchronously and parses/draws its layout elements.

### Supported HTML5 Markup Elements
* **Layout Containers**: `<div>` (blocks), `<p>` (text paragraphs), `<blockquote style='color: #38bdf8;'>` (stylized quote boxes with left borders).
* **Typography**: `<h1>` to `<h4>` (headers), `<strong>`/`<b>` (bold), `<em>`/`<i>` (italic), `<u>` (underline), `<del>`/`<s>`/`<strike>` (strikethroughs), `<mark>` (highlighter background overlays).
* **Monospace Formatters**: `<pre>` & `<code>` (renders a dark colored code editor window box with purple monospace characters).
* **Tabular Grids**: `<table>`, `<tr>`, `<td>`, and `<th>` (renders nested cells tables with border dividers).
* **Load diagnostics Indicators**: `<progress value='0.75'>` & `<meter value='0.45'>` (dynamic loading bars natively drawn inside the canvas).
* **Form Widgets**: `<input type='text' id='...' placeholder='...'>`, `<input type='password' id='...'>` (secure typing masking stars `*`), multiline text boxes `<textarea id='...'>`, dropdown option selectors `<select id='...'>`.
* **Click Interactive Anchors**: `<a href='...'>` (hyperlink references).
* **Embedded Graphics**: `<img src='...' alt='...'>` (draws image placeholders with filenames and alternative descriptors).

### Supported CSS Inline Styling Rules (`style='...'`)
* **Color Schemes**: `color`, `background-color`, `border-color`, `border` (resolves custom border-width, style, and colors dynamically).
* **Dimension bounding box**: `width`, `height`.
* **Margins & Paddings**: Shorthand configurations (`padding: 10px`, `padding: 5px 10px`, `margin: 15px`) are parsed in software to exact top, bottom, left, and right layout spacing.
* **Layout Metrics Units**: Native translation of absolute pixels (`px`), percentages (`%`), and scalable font units (`em`).

### Inline JavaScript Scripting Runtime (`<script>`)
Write native JavaScript functions right inside HTML markup. The C++ parser extracts and populates function definitions dynamically, routing button `onclick` triggers to the JS runtime:
- **`alert(\"...\")`**: Displays standard dialog modals.
- **`console.log(\"...\")`**: Logs to the active terminal with a custom `[JS Console]` tag.
- **`document.getElementById(\"...\").value = \"...\"`**: Dynamically writes value strings to active canvas textbox/textarea input elements in-memory.

