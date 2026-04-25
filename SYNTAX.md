# Nexus Titan Language v3.0-NEXUS 

## 1. Core Paradigm & Execution
*   **Type:** Interpreted, dynamically typed, procedural scripting language.
*   **Execution:** Line-by-line.
*   **File Extension:** `.nx`
*   **Case Sensitivity:** Keywords and identifiers are case-sensitive.
*   **Types:** Strings (enclosed in `" "`), Integers, Floats. Booleans do not have explicit keywords; `1` is true, `0` is false.

## 2. Basic Syntax & Keywords

### Comments
Comments start with `#` and ignore the rest of the line.
```nexus
# This is a comment
```

### Variables
Defined and assigned using the `set` keyword. No variable declarations are needed before assignment.
```nexus
set my_var = 10
set my_string = "Hello World"
set computed = my_var + 5
```

### Console I/O
*   **Output:** `out <expression>` (Automatically prepends `Nexus › ` to the console).
*   **Input:** `input <variable_name> "<Prompt text>"`
```nexus
out "Welcome to my program!"
input user_name "What is your name?"
out "Hello, " + user_name
```

### Pausing Execution
*   **Wait:** `wait <seconds>`
```nexus
wait 2.5  # Pauses script for 2.5 seconds
```

## 3. Control Flow

### Conditionals (`if` / `else` / `end`)
Conditions evaluate standard operators (`==`, `!=`, `<`, `>`, `<=`, `>=`).
```nexus
if user_age >= 18
    out "Access granted."
else
    out "Access denied."
end
```

### Loops (`loop` / `end`)
Nexus uses a fixed-count `loop` construct. It evaluates the expression next to it to determine how many times to iterate.
```nexus
set count = 5
loop count
    out "Iteration!"
end
```

## 4. Functions (`fn`)
*   **CRITICAL LIMITATION:** Nexus custom functions **do not take arguments** and **do not explicitly return values**. They act as reusable code blocks. 
*   To pass data in or out of a function, you must use **global variables**.
*   **Definition:** `fn name()`
*   **Invocation:** `name()`

```nexus
# Correct way to use functions:
set arg1 = 10
set arg2 = 20

fn add_numbers()
    set result = arg1 + arg2
end

add_numbers()
out result  # Outputs 30
```

## 5. Expressions & Operators
*   **Math:** `+`, `-`, `*`, `/`
*   **String Concatenation:** `+`
*   **Comparison:** `==`, `!=`, `<`, `>`, `<=`, `>=`
*   **Evaluation:** Standard mathematical precedence applies.

---

## 6. Standard Library (Modules)
Nexus features an absolutely massive built-in standard library. Module functions are called using dot-notation: `module.function(args)`.

### 6.1. `math.*` (Mathematics)
*   **Constants:** `pi()`, `e()`, `tau()`, `inf()`, `nan()`
*   **Trigonometry:** `sin(x)`, `cos(x)`, `tan(x)`, `asin(x)`, `acos(x)`, `atan(x)`, `atan2(y, x)`, `sinh(x)`, `cosh(x)`, `tanh(x)`, `asinh(x)`, `acosh(x)`, `atanh(x)`
*   **Operations:** `sqrt(x)`, `cbrt(x)`, `pow(x, y)`, `exp(x)`, `log(x)`, `log10(x)`, `log2(x)`, `abs(x)`
*   **Rounding:** `ceil(x)`, `floor(x)`, `round(x)`, `trunc(x)`
*   **Utils:** `deg(rad)`, `rad(deg)`, `min(a,b)`, `max(a,b)`, `hypot(x,y)`, `gcd(a,b)`, `lcm(a,b)`, `mod(a,b)`, `fact(x)`, `perm(n,k)`, `comb(n,k)`, `is_nan(x)`, `is_inf(x)`, `is_close(a,b)`, `fsum(list)`, `remainder(a,b)`, `erf(x)`, `erfc(x)`, `gamma(x)`, `lgamma(x)`

### 6.2. `str.*` (Strings)
*   **Transform:** `upper(s)`, `lower(s)`, `cap(s)`, `trim(s)`, `ltrim(s)`, `rtrim(s)`, `replace(s, old, new)`, `reverse(s)`, `repeat(s, n)`, `padl(s, n, char)`, `padr(s, n, char)`, `swap(s)`, `title(s)`, `zfill(s, n)`
*   **Extraction/Lists:** `split(s, delim)`, `join(delim, list)`, `at(s, index)`, `slice(s, start, end)`, `partition(s, delim)`, `rpartition(s, delim)`
*   **Search:** `find(s, query)`, `rfind(s, query)`, `starts(s, query)`, `ends(s, query)`, `count(s, query)`
*   **Validation:** `isnum(s)`, `isalpha(s)`, `isalnum(s)`, `isspace(s)`, `islower(s)`, `isupper(s)`, `istitle(s)`, `isascii(s)`, `isdecimal(s)`, `isident(s)`, `isprint(s)`
*   **Conversion:** `len(s)`, `hex(n)`, `bin(n)`, `ascii(s)`, `chr(n)`, `ord(c)`

### 6.3. `sys.*` (System & Environment)
*   **Environment:** `os()`, `ver()`, `arch()`, `user()`, `cwd()`, `setcwd(dir)`, `env(key)`, `setenv(k,v)`, `delenv(k)`, `getenv(k, default)`, `plat()`, `node()`, `proc()`, `rel()`, `mac_ver()`, `win32_ver()`, `libc_ver()`, `architecture()`
*   **Process:** `exit()`, `pid()`, `sleep(sec)`, `time()`, `cpu_count()`, `getppid()`, `whoami()`, `getlogin()`, `boot_time()`, `executable()`
*   **Execution:** `shell(cmd)` (Returns output string), `run(cmd)` (Returns exit code), `clear()`
*   **Hardware/Memory:** `mem_total()`, `mem_used()`, `mem_free()`, `is_64bit()`
*   **Interpreter Details:** `info()`, `python_ver()`, `byteorder()`, `maxsize()`, `getdefaultencoding()`, `getfilesystemencoding()`

### 6.4. `io.*` (File System)
*   **Read/Write:** `read(file)`, `write(file, data)`, `append(file, data)`, `read_lines(file)`, `write_lines(file, list)`
*   **Validation:** `exists(path)`, `isdir(path)`, `isfile(path)`, `islink(path)`, `same(f1, f2)`
*   **Modifications:** `mkdir(dir)`, `rmdir(dir)`, `remove(file)`, `rename(old, new)`, `copy(src, dst)`, `move(src, dst)`, `touch(file)`, `copytree(src, dst)`, `chmod(file, mode)`, `chown(file, user, group)`
*   **Paths & Meta:** `size(file)`, `list(dir)`, `ext(path)`, `base(path)`, `abs(path)`, `join(p1, p2)`, `split(path)`, `dirname(path)`, `realpath(path)`, `glob(pattern)`, `walk(dir)`, `stat(file)`
*   **Time:** `atime(file)`, `mtime(file)`, `ctime(file)`
*   **System Dirs:** `get_temp_dir()`, `get_home_dir()`, `cwd()`, `mount_points()`
*   **Hashes:** `file_hash_md5(file)`, `file_hash_sha256(file)`

### 6.5. `gui.*` (Graphical User Interfaces)
*   **Setup/Lifecycle:** `window(title)`, `geom("WxH")`, `title(text)`, `run()` (Starts the event loop), `clear()`, `destroy()`, `update()`, `bgcolor(color)`, `resizable(x, y)`
*   **Widgets:** `label(text)`, `button(text, function_name)`, `entry(name, default_value)`
*   **Data Retrieval:** `get_entry(name)`
*   **Dialogs:** `msg(text)`, `warn(text)`, `err(text)`, `ask(question)` (returns 1 or 0), `input(prompt)`
*   **File Dialogs:** `file_dialog()`, `dir_dialog()`, `save_dialog()`, `color_dialog()`

### 6.6. `net.*` (Networking)
*   **HTTP:** `get(url)`, `post(url, data)`, `download(url, file)`, `get_json(url)`, `request_headers(url)`
*   **Status/Utils:** `ping(host)`, `is_connected()`, `host()`, `ip()`, `ip_local()`
*   **Formatting:** `url(str)`, `unurl(str)`, `urlencode(dict_string)`, `parse_qs(query_string)`

### 6.7. `date.*` (Time and Dates)
*   `now()`, `year()`, `month()`, `day()`, `hour()`, `min()`, `sec()`, `ts()`, `today()`, `weekday()`, `iso()`, `utc_now()`, `timezone()`, `is_leap_year(year)`
*   `fmt(format_string)`, `from_ts(timestamp)`, `sleep_ms(ms)`
*   `perf_counter()`, `process_time()`

### 6.8. `rnd.*` (Randomization)
*   `val()`, `int(min, max)`, `float(min, max)`, `choice(list)`, `shuffle(list)`, `seed(x)`, `sample(list, k)`, `gauss(mu, sigma)`, `tri(low, high, mode)`, `range(start, stop)`
*   `bytes(n)`, `hex()`, `uuid4()`

### 6.9. `crypto.*` (Cryptography)
*   `md5(str)`, `sha1(str)`, `sha256(str)`, `sha512(str)`
*   `b64encode(str)`, `b64decode(str)`, `rot13(str)`

### 6.10. `zip.*` (Archives)
*   `compress(src, dst)`, `extract(src, dst)`, `extract_all(src, dst)`, `list(src)`, `read_file(src, filename)`, `add_file(zip_path, file_path)`, `is_zipfile(file)`
*   `create_tar(target, src)`, `create_gztar(target, src)`, `unpack_archive(src, dst)`, `get_archive_formats()`

### 6.11. `json.*` (JSON Handling)
*   `parse(json_str)`, `str(dict_obj)`, `load(file_path)`, `save(file_path, data)`, `valid(json_str)`, `pretty(data)`, `get_keys(data)`

### 6.12. `admin.*` (Privilege Escalation)
*   `is_admin()`, `run_admin(cmd)`, `elevate()`, `sudo(cmd)`
*   `get_euid()`, `get_egid()`, `get_uid()`, `get_gid()`, `set_uid(u)`, `set_gid(g)`

### 6.13. `browser.*`
*   `open(url)`, `open_new(url)`, `open_tab(url)`, `get()`

### 6.14. `cli.*` (Command Line Aesthetics)
*   `clear()`, `pause()`
*   **Colors/Formatting:** `red(s)`, `green(s)`, `blue(s)`, `reset(s)`, `bold(s)`, `under(s)`, `yellow(s)`, `cyan(s)`, `magenta(s)`, `white(s)`, `bg_red(s)`, `bg_green(s)`, `bg_blue(s)`, `bg_yellow(s)`
*   **Cursor:** `cursor_up(n)`, `cursor_down(n)`, `cursor_right(n)`, `cursor_left(n)`

---

## 7. Crucial Programming Patterns in Nexus

### 7.1. Making a GUI Application
Because functions do not take arguments, GUI callbacks (`gui.button`) point to a function name as a string, and you read input states via `gui.get_entry()`.
```nexus
gui.window("Downloader")
gui.geom("400x200")
gui.label("Enter URL:")
gui.entry("url_input", "https://")

fn do_download()
    set target_url = gui.get_entry("url_input")
    set success = net.download(target_url, "file.downloaded")
    gui.msg("Downloaded!")
end

gui.button("Download File", "do_download")
gui.run()
```

### 7.2. JSON and API Fetching
```nexus
set api_url = "https://api.github.com/users/octocat"
set response = net.get(api_url)

if json.valid(response) == 1
    set parsed = json.parse(response)
    out "Data fetched successfully!"
else
    out "Failed to fetch JSON"
end
```

### 7.3. Working with Math and Randoms
```nexus
set random_num = rnd.int(1, 100)
set root = math.sqrt(random_num)
out "The square root of " + random_num + " is " + root
```
