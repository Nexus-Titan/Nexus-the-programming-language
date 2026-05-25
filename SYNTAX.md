# Nexus Titan Syntax Guide (v4.1.3)

Nexus is a dynamically typed, procedural, and extremely portable scripting language. Version 4.1.3 provides full execution parity between its native C++ and web engines.

## 1. File Extension
All Nexus source files use the `.nx` extension.

## 2. Variables & Constants

Variables are defined and assigned using the `set` keyword:
```nexus
set name = "Nexus V4"
set version = 4.1.3
set is_native = 1
```

Constants are provided by the `math` module:
- `math.pi()` -> `3.141592653589793`
- `math.e()` -> `2.718281828459045`
- `math.tau()` -> `6.283185307179586`

## 3. Basic I/O

- **Output**: The `out` keyword prints an expression to stdout, automatically prepended with `Nexus › `:
  ```nexus
  out "Welcome to " + name
  ```
- **Input**: The `input` keyword prompts the user for text input and stores it in a variable:
  ```nexus
  input user_name "Enter your name"
  out "Hello, " + user_name
  ```

## 4. Control Flow

### Conditionals (`if` / `else` / `end`)
Conditions support standard comparisons (`==`, `!=`, `<`, `>`, `<=`, `>=`) and logical operators (`&&`, `||`):
```nexus
if age >= 18 && is_authorized == 1
    out "Access granted."
else
    out "Access denied."
end
```

### Loops
Nexus v4.1.3 supports two loop structures:

1. **Fixed-Count Loop (`loop` / `end`)**:
   Iterates a specific number of times:
   ```nexus
   set count = 5
   loop count
       out "Iteration!"
   end
   ```

2. **Conditional Loop (`while` / `end`)**:
   Iterates while a condition evaluates to true (non-zero and non-empty):
   ```nexus
   set index = 0
   while index < 10
       out "Index is: " + index
       set index = index + 1
   end
   ```

3. **Loop Breaking (`break`)**:
   Instantly breaks out of any loop or conditional block:
   ```nexus
   set i = 0
   while i < 100
       if i == 5
           break
       end
       set i = i + 1
   end
   ```

## 5. Functions & Argument Mapping

Functions are declared with `fn` and end with `end`. In version 4.1.3, functions support **dynamic parameters** and **return statements** with local scoping:

```nexus
# Function definition with parameters
fn calculate_area(radius)
    set pi_val = math.pi()
    set area = pi_val * math.pow(radius, 2)
    return area
end

# Calling functions with arguments
set res = calculate_area(10)
out "Area: " + res  # Outputs 314.159
```

## 6. List & Array Mappings
Lists in Nexus are mapped as JSON array strings. Standard list functions handle them seamlessly:
```nexus
set my_list = "[\"apple\", \"banana\", \"cherry\"]"
set total = str.len(my_list)      # Returns 3 (number of items)
set first = str.at(my_list, 0)    # Returns "apple"
```

## 7. Dynamic Imports
You can import other `.nx` scripts or `.py` modules dynamically:
```nexus
import "logger.nx"
logger.info("Diagnostics initialized.")

import "py_suite.py"
set gray = py_suite.grayscale(255, 128, 64)
```

## 8. Standard Library Reference (300+ Functions)

### 8.1. `math.*`
Constants: `pi()`, `e()`, `tau()`, `inf()`, `nan()`
Trigonometry: `sin(x)`, `cos(x)`, `tan(x)`, `asin(x)`, `acos(x)`, `atan(x)`, `atan2(y, x)`, `sinh(x)`, `cosh(x)`, `tanh(x)`, `asinh(x)`, `acosh(x)`, `atanh(x)`
Rounding: `ceil(x)`, `floor(x)`, `round(x)`, `trunc(x)`
Operations: `sqrt(x)`, `cbrt(x)`, `pow(x, y)`, `abs(x)`, `min(a,b)`, `max(a,b)`, `hypot(x,y)`, `gcd(a,b)`, `lcm(a,b)`, `mod(a,b)`, `fact(x)`, `perm(n,k)`, `comb(n,k)`
Utility: `deg(rad)`, `rad(deg)`, `is_nan(x)`, `is_inf(x)`, `remainder(a,b)`, `erf(x)`, `erfc(x)`, `gamma(x)`, `lgamma(x)`, `is_close(a,b)`, `fsum(list)`, `prod(list)`, `dist(p1,p2)`

### 8.2. `str.*`
Formatting: `upper(s)`, `lower(s)`, `cap(s)`, `trim(s)`, `ltrim(s)`, `rtrim(s)`, `reverse(s)`, `repeat(s, n)`, `zfill(s, n)`, `swap(s)`, `title(s)`, `replace(s, old, new)`, `split(s, delim)`, `join(delim, list_str)`
Extraction: `at(s, index)`, `slice(s, start, end)`, `partition(s, delim)`, `rpartition(s, delim)`
Searching: `find(s, query)`, `rfind(s, query)`, `starts(s, query)`, `ends(s, query)`, `count(s, query)`
Checking: `isnum(s)`, `isalpha(s)`, `isalnum(s)`, `isspace(s)`, `islower(s)`, `isupper(s)`, `len(s)`
Casting: `hex(n)`, `bin(n)`, `ascii(s)`, `chr(n)`, `ord(c)`

### 8.3. `sys.*`
Environment: `os()`, `ver()`, `arch()`, `user()`, `cwd()`, `setcwd(dir)`, `env(key)`, `setenv(k,v)`, `delenv(k)`, `whoami()`, `clear()`
Process: `exit(code)`, `pid()`, `cpu_count()`, `sleep(sec)`, `time()`, `info()`
Execution: `shell(cmd)` (captures output), `run(cmd)` (returns status code)
Memory: `mem_total()`, `mem_used()`, `mem_free()`

### 8.4. `io.*`
File Operations: `read(file)`, `write(file, data)`, `append(file, data)`, `read_lines(file)`, `write_lines(file, list)`
Validation: `exists(path)`, `isdir(path)`, `isfile(path)`
CRUD: `mkdir(dir)`, `rmdir(dir)`, `remove(file)`, `rename(old, new)`, `copy(src, dst)`, `move(src, dst)`, `touch(file)`, `chmod(file, mode)`, `glob(pattern)`, `list(dir)`
Path Meta: `size(file)`, `ext(path)`, `base(path)`, `abs(path)`, `join(p1, p2)`, `split(path)`, `dirname(path)`, `home()`, `tmp()`
Hashes: `file_hash_sha256(file)`, `file_hash_md5(file)`

### 8.5. `zip.*`
Compression: `compress(src, dst)`, `extract(src, dst)`, `list(src)`

### 8.6. `net.*`
HTTP Clients: `get(url)`, `post(url, data)`, `download(url, file)`, `get_json(url)`
Utilities: `ping(host)`, `ip()`, `is_online()`

### 8.7. `date.*`
Current: `now()`, `today()`, `iso()`, `ts()`
Parts: `year()`, `month()`, `day()`, `hour()`, `min()`, `sec()`
Format: `fmt(f)`, `sleep_ms(ms)`

### 8.8. `rnd.*`
Values: `val()`, `int(min, max)`, `float(min, max)`
Selection: `choice(list)`, `shuffle(list)`, `uuid()`

### 8.9. `crypto.*`
Hashes: `md5(str)`, `sha256(str)`
Base64: `b64e(str)`, `b64d(str)`

### 8.10. `json.*`
Utility: `parse(str)`, `str(obj)`, `load(file)`, `save(file, data)`, `pretty(data)`, `get_keys(data)`

### 8.11. `gui.*`
Window Setup: `window(title)`, `geom("WxH")`, `title(text)`, `run()`, `clear()`
Widgets: `label(text)`, `button(text, function)`, `entry(name, default)`, `get_entry(name)`
Dialogs: `msg(text)`, `warn(text)`, `err(text)`, `ask(question)`, `input(prompt)`

### 8.12. `cli.*`
Control: `clear()`, `pause()`, `print(text)`
Colors: `red(s)`, `green(s)`, `blue(s)`, `yellow(s)`, `cyan(s)`, `magenta(s)`, `reset(s)`, `bold(s)`
