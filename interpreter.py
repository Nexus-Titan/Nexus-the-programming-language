import re, sys, os, math, time, json, platform, shutil, random, subprocess
import datetime, zipfile, webbrowser, hashlib, glob, urllib.request, urllib.parse, urllib.error
try:
    import tkinter as tk
    from tkinter import messagebox, simpledialog, filedialog
    HAS_TK = True
except ImportError:
    tk = messagebox = simpledialog = filedialog = None
    HAS_TK = False
import ctypes
class NexusUltraV4:
    def __init__(self):
        self.version = "4.0-ULTRA"
        self.vars = {
            "OS": platform.system(),
            "VER": self.version,
            "ARCH": platform.machine(),
            "USER": os.getenv("USER") or os.getenv("USERNAME") or "NexusUser",
            "CWD": os.getcwd(),
            "PI": math.pi,
            "E": math.e,
            "ENGINE": "Python",
            "HAS_GUI": HAS_TK
        }
        self.funcs = {}
        self.root = None
        self.gui_entries = {}
        self.setup_stdlib()
    def setup_stdlib(self):
        self.stdlib = {}
        self.stdlib['api'] = {
            'type': lambda: "PC",
            'engine': lambda: "Python",
            'version': lambda: self.version,
            'is_web': lambda: False,
            'is_pc': lambda: True,
            'call': self.api_call,
            'sync': lambda d: print(f"API Sync › {d}"),
            'platform': platform.system
        }
        self.stdlib['math'] = {
            'sin': math.sin, 'cos': math.cos, 'tan': math.tan, 'asin': math.asin, 'acos': math.acos, 'atan': math.atan,
            'atan2': math.atan2, 'sinh': math.sinh, 'cosh': math.cosh, 'tanh': math.tanh, 'asinh': math.asinh, 
            'acosh': math.acosh, 'atanh': math.atanh, 'sqrt': math.sqrt, 'cbrt': lambda x: x**(1/3), 
            'pow': math.pow, 'exp': math.exp, 'log': math.log, 'log10': math.log10, 'log2': math.log2, 
            'abs': math.fabs, 'ceil': math.ceil, 'floor': math.floor, 'round': round, 'trunc': math.trunc, 
            'pi': lambda: math.pi, 'e': lambda: math.e, 'tau': lambda: math.tau, 'inf': lambda: math.inf,
            'nan': lambda: math.nan, 'deg': math.degrees, 'rad': math.radians, 'min': min, 'max': max, 
            'hypot': math.hypot, 'gcd': math.gcd, 'lcm': lambda a,b: abs(a*b) // math.gcd(a,b) if a*b else 0, 
            'mod': lambda a,b: a % b, 'is_nan': math.isnan, 'is_inf': math.isinf, 'fact': math.factorial,
            'fsum': math.fsum, 'perm': math.perm, 'comb': math.comb, 'remainder': math.remainder,
            'erf': math.erf, 'erfc': math.erfc, 'gamma': math.gamma, 'lgamma': math.lgamma,
            'is_close': math.isclose, 'prod': math.prod, 'dist': math.dist
        }
        self.stdlib['str'] = {
            'len': len, 'upper': lambda s: str(s).upper(), 'lower': lambda s: str(s).lower(),
            'cap': lambda s: str(s).capitalize(), 'trim': lambda s: str(s).strip(),
            'ltrim': lambda s: str(s).lstrip(), 'rtrim': lambda s: str(s).rstrip(),
            'replace': lambda s,o,n: str(s).replace(str(o), str(n)),
            'split': lambda s,d=' ': str(s).split(str(d)), 'join': lambda d,l: str(d).join(map(str, l)),
            'at': lambda s,i: str(s)[int(i)] if 0 <= int(i) < len(str(s)) else "",
            'find': lambda s,q: str(s).find(str(q)), 'rfind': lambda s,q: str(s).rfind(str(q)),
            'starts': lambda s,q: str(s).startswith(str(q)), 'ends': lambda s,q: str(s).endswith(str(q)),
            'isnum': lambda s: str(s).isdigit(), 'isalpha': lambda s: str(s).isalpha(),
            'isalnum': lambda s: str(s).isalnum(), 'isspace': lambda s: str(s).isspace(),
            'reverse': lambda s: str(s)[::-1], 'repeat': lambda s,n: str(s) * int(n),
            'hex': hex, 'bin': bin, 'ascii': ascii, 'chr': chr, 'ord': ord, 'count': lambda s,q: str(s).count(str(q)),
            'zfill': lambda s,n: str(s).zfill(int(n)), 'format': lambda s,*a: str(s).format(*a)
        }
        self.stdlib['sys'] = {
            'os': platform.system, 'ver': platform.version, 'arch': platform.machine,
            'user': lambda: os.getenv("USER") or os.getenv("USERNAME") or "NexusUser", 
            'cwd': os.getcwd, 'setcwd': os.chdir, 'env': lambda k: os.getenv(k),
            'exit': sys.exit, 'pid': os.getpid, 'cpu_count': os.cpu_count, 
            'shell': lambda c: subprocess.getoutput(c),
            'run': lambda c: subprocess.run(c, shell=True).returncode, 
            'sleep': time.sleep, 'time': time.time, 'info': self.get_sys_info,
            'argv': lambda: sys.argv, 'path': lambda: sys.path, 'plat': lambda: sys.platform,
            'mem_total': lambda: shutil.disk_usage(os.getcwd()).total,
            'mem_used': lambda: shutil.disk_usage(os.getcwd()).used,
            'mem_free': lambda: shutil.disk_usage(os.getcwd()).free,
            'setenv': lambda k,v: os.environ.update({str(k): str(v)}),
            'delenv': lambda k: os.environ.pop(str(k), None),
            'whoami': lambda: subprocess.getoutput('whoami'),
            'clear': lambda: os.system('cls' if os.name == 'nt' else 'clear')
        }
        self.stdlib['admin'] = {
            'is_admin': self.is_admin, 'run_admin': self.run_admin, 'elevate': self.elevate,
            'sudo': lambda cmd: self.run_admin(cmd), 'get_uid': lambda: os.getuid() if hasattr(os, 'getuid') else -1
        }
        self.stdlib['io'] = {
            'read': lambda f: open(f, 'r', encoding='utf-8').read() if os.path.exists(f) else "Error",
            'write': lambda f,d: open(f, 'w', encoding='utf-8').write(str(d)), 
            'append': lambda f,d: open(f, 'a', encoding='utf-8').write(str(d)),
            'exists': os.path.exists, 'isdir': os.path.isdir, 'isfile': os.path.isfile,
            'mkdir': os.makedirs, 'rmdir': shutil.rmtree, 'remove': os.remove, 'rename': os.rename,
            'copy': shutil.copy, 'move': shutil.move, 'size': os.path.getsize, 'list': os.listdir,
            'ext': lambda p: os.path.splitext(p)[1], 'base': os.path.basename, 'abs': os.path.abspath,
            'join': os.path.join, 'split': os.path.split, 'dirname': os.path.dirname, 'realpath': os.path.realpath,
            'touch': lambda f: open(f, 'a').close() or os.utime(f, None),
            'read_lines': lambda f: open(f, 'r', encoding='utf-8').readlines() if os.path.exists(f) else [],
            'write_lines': lambda f, l: open(f, 'w', encoding='utf-8').writelines(l),
            'copytree': shutil.copytree, 'chmod': lambda f, m: os.chmod(f, int(str(m), 8)),
            'glob': glob.glob, 'walk': lambda d: list(os.walk(d)), 'stat': lambda f: str(os.stat(f)),
            'home': lambda: os.path.expanduser('~'), 'tmp': lambda: __import__('tempfile').gettempdir()
        }
        self.stdlib['zip'] = {
            'compress': self.zip_compress, 'extract': self.zip_extract, 'list': self.zip_list,
            'read': self.zip_read_file, 'add': self.zip_add_file, 'is_zip': zipfile.is_zipfile
        }
        self.stdlib['net'] = {
            'get': lambda u: urllib.request.urlopen(u).read().decode('utf-8'),
            'post': self.http_post, 'download': lambda u,f: urllib.request.urlretrieve(u, f),
            'ping': lambda h: subprocess.run(f"ping -n 1 {h}" if os.name=='nt' else f"ping -c 1 {h}", shell=True, stdout=subprocess.DEVNULL).returncode == 0,
            'ip': lambda: __import__('socket').gethostbyname(__import__('socket').gethostname()),
            'is_online': self.is_connected, 'get_json': self.http_get_json
        }
        self.stdlib['date'] = {
            'now': lambda: str(datetime.datetime.now()), 'year': lambda: datetime.datetime.now().year,
            'month': lambda: datetime.datetime.now().month, 'day': lambda: datetime.datetime.now().day,
            'hour': lambda: datetime.datetime.now().hour, 'min': lambda: datetime.datetime.now().minute,
            'sec': lambda: datetime.datetime.now().second, 'ts': time.time, 'today': datetime.date.today,
            'iso': lambda: datetime.datetime.now().isoformat(), 'fmt': lambda f: datetime.datetime.now().strftime(str(f)),
            'sleep_ms': lambda ms: time.sleep(float(ms)/1000.0), 'perf': time.perf_counter
        }
        self.stdlib['rnd'] = {
            'val': random.random, 'int': random.randint, 'float': random.uniform,
            'choice': random.choice, 'shuffle': lambda l: (random.shuffle(l), l)[1] if isinstance(l, list) else l, 
            'seed': random.seed, 'sample': random.sample, 'bytes': lambda n: random.randbytes(int(n)).hex(),
            'uuid': lambda: __import__('uuid').uuid4().hex
        }
        self.stdlib['crypto'] = {
            'md5': lambda s: hashlib.md5(str(s).encode()).hexdigest(),
            'sha256': lambda s: hashlib.sha256(str(s).encode()).hexdigest(),
            'b64e': lambda s: __import__('base64').b64encode(str(s).encode()).decode(),
            'b64d': lambda s: __import__('base64').b64decode(str(s).encode()).decode()
        }
        if HAS_TK:
            self.stdlib['gui'] = {
                'window': self.gui_window, 'label': self.gui_label, 'button': self.gui_button,
                'msg': lambda m: messagebox.showinfo("Nexus ULTRA", str(m)),
                'warn': lambda m: messagebox.showwarning("Nexus ULTRA", str(m)),
                'err': lambda m: messagebox.showerror("Nexus ULTRA", str(m)),
                'ask': lambda m: messagebox.askyesno("Nexus ULTRA", str(m)),
                'input': lambda p: simpledialog.askstring("Nexus ULTRA", str(p)),
                'run': lambda: self.root.mainloop() if self.root else None,
                'title': lambda t: self.root.title(str(t)) if self.root else None,
                'clear': lambda: [c.destroy() for c in self.root.winfo_children()] if self.root else None,
                'bgcolor': lambda c: self.root.configure(bg=str(c)) if self.root else None
            }
        else:
            self.stdlib['gui'] = {}
        self.stdlib['json'] = {
            'parse': json.loads, 'str': json.dumps, 'load': lambda f: json.load(open(f)),
            'save': lambda f,d: json.dump(d, open(f, 'w')), 'pretty': lambda d: json.dumps(d, indent=4)
        }
        self.stdlib['cli'] = {
            'clear': lambda: os.system('cls' if os.name == 'nt' else 'clear'),
            'red': lambda s: f"\033[91m{s}\033[0m", 'green': lambda s: f"\033[92m{s}\033[0m",
            'blue': lambda s: f"\033[94m{s}\033[0m", 'yellow': lambda s: f"\033[93m{s}\033[0m",
            'cyan': lambda s: f"\033[96m{s}\033[0m", 'magenta': lambda s: f"\033[95m{s}\033[0m",
            'reset': lambda s: f"\033[0m{s}", 'bold': lambda s: f"\033[1m{s}\033[0m",
            'print': print, 'pause': lambda: input("Press Enter to continue...")
        }
    def is_admin(self):
        try: return ctypes.windll.shell32.IsUserAnAdmin() != 0 if os.name == 'nt' else os.geteuid() == 0
        except: return False
    def elevate(self):
        if self.is_admin(): return True
        try:
            if os.name == 'nt':
                ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)
                sys.exit()
            else: os.execvp("sudo", ["sudo", sys.executable] + sys.argv)
        except: return False
    def run_admin(self, cmd):
        return subprocess.run(f"sudo {cmd}" if os.name != 'nt' else f"runas /user:administrator {cmd}", shell=True).returncode
    def zip_compress(self, src, dst):
        with zipfile.ZipFile(dst, 'w', zipfile.ZIP_DEFLATED) as zipf:
            if os.path.isdir(src):
                for root, _, files in os.walk(src):
                    for file in files: zipf.write(os.path.join(root, file), os.path.relpath(os.path.join(root, file), src))
            else: zipf.write(src, os.path.basename(src))
        return True
    def zip_extract(self, src, dst):
        with zipfile.ZipFile(src, 'r') as zipf: zipf.extractall(dst)
        return True
    def zip_list(self, src):
        with zipfile.ZipFile(src, 'r') as zipf: return zipf.namelist()
    def zip_read_file(self, src, filename):
        with zipfile.ZipFile(src, 'r') as zipf: return zipf.read(filename).decode('utf-8')
    def zip_add_file(self, zp, fp, an=None):
        with zipfile.ZipFile(zp, 'a') as zipf: zipf.write(fp, an or os.path.basename(fp))
        return True
    def http_post(self, url, data):
        if isinstance(data, dict): data = urllib.parse.urlencode(data).encode()
        req = urllib.request.Request(url, data=data)
        return urllib.request.urlopen(req).read().decode('utf-8')
    def is_connected(self):
        try: return urllib.request.urlopen('http://8.8.8.8', timeout=1) and True
        except: return False
    def http_get_json(self, url):
        return json.loads(urllib.request.urlopen(url).read().decode('utf-8'))
    def api_call(self, t, f, a=None):
        print(f"ULTRA API Bridge › {t}::{f}({a})")
        return True
    def get_sys_info(self):
        return f"Nexus ULTRA v{self.version} | {self.vars['OS']} | {self.vars['ARCH']}"
    def gui_window(self, title, geom="400x300"):
        if HAS_TK:
            self.root = tk.Tk()
            self.root.title(title)
            self.root.geometry(geom)
    def gui_label(self, text):
        if self.root: tk.Label(self.root, text=text).pack(pady=10)
    def gui_button(self, text, fn):
        if self.root: tk.Button(self.root, text=text, command=lambda: self.execute_fn(fn)).pack(pady=5)
    def execute_fn(self, name):
        if name in self.funcs: self.run(self.funcs[name]['tokens'])
    def execute_fn_with_args(self, name, *args):
        if name in self.funcs:
            f = self.funcs[name]
            old_vars = self.vars.copy()
            for i, param in enumerate(f['params']):
                if i < len(args): self.vars[param] = args[i]
            self.run(f['tokens'])
            res = self.vars.get("return", None)
            self.vars = old_vars
            return res
    def import_lib(self, path):
        path = str(path).strip('"')
        if not os.path.isabs(path):
            local_path = os.path.join(os.getcwd(), path)
            lib_path = os.path.join(os.path.dirname(__file__), "libs", path)
            if os.path.exists(local_path): path = local_path
            elif os.path.exists(lib_path): path = lib_path
            elif os.path.exists(path): pass
            else: print(f"Error: Library {path} not found"); return
        if path.endswith('.nx'):
            mod_name = os.path.basename(path).replace('.nx', '')
            with open(path, 'r') as f:
                old_funcs = set(self.funcs.keys())
                self.run(self.tokenize(f.read()))
                new_funcs = set(self.funcs.keys()) - old_funcs
                if not mod_name in self.stdlib: self.stdlib[mod_name] = {}
                for fn in new_funcs:
                    self.stdlib[mod_name][fn] = lambda *a, name=fn: self.execute_fn_with_args(name, *a)
                print(f"Nexus › Imported Nexus Module: {mod_name}")
        elif path.endswith('.py'):
            mod_name = os.path.basename(path).replace('.py', '')
            import importlib.util
            spec = importlib.util.spec_from_file_location(mod_name, path)
            foo = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(foo)
            self.stdlib[mod_name] = {k: v for k, v in vars(foo).items() if callable(v)}
            print(f"Nexus › Imported Python Module: {mod_name}")
    def handle_add_directive(self, content):
        lines = content.split('\n')
        for line in lines:
            line = line.strip()
            if not line or line.startswith('#'): continue
            if line.startswith('-'):
                lib_type = line[1:].strip()
                print(f"Nexus › Enabling support for: {lib_type}")
    def tokenize(self, code):
        mod_keys = '|'.join(self.stdlib.keys())
        tokens_def = [
            ('COMMENT', r'#.*'),
            ('KEYWORD', r'\b(fn|set|out|if|else|loop|while|for|end|wait|input|return|break|import)\b|\+add'),
            ('NUMBER', r'\d+(\.\d+)?'), 
            ('STRING', r'"[^"]*"'),
            ('MODULE_FUNC', r'\b[a-zA-Z_]\w*\.[a-zA-Z_]\w*'),
            ('ID', r'[a-zA-Z_]\w*'), 
            ('ASSIGN', r'='), 
            ('LPAREN', r'\('), ('RPAREN', r'\)'), ('COMMA', r','),
            ('OP', r'[+\-*/><=!&|]+'), ('SEP', r';'), ('SKIP', r'[ \t\n\r]+'), ('MISMATCH', r'.'),
        ]
        reg = '|'.join('(?P<%s>%s)' % p for p in tokens_def)
        return [m for m in ((mo.lastgroup, mo.group()) for mo in re.finditer(reg, code)) if m[0] not in ('SKIP', 'COMMENT')]
    def run(self, tokens):
        i = 0
        while i < len(tokens):
            kind, val = tokens[i]
            if val == 'set':
                name = tokens[i+1][1]; i += 3
                res, i = self.expr(tokens, i)
                self.vars[name] = res
            elif val == 'out':
                i += 1; res, i = self.expr(tokens, i); print(f"Nexus › {res}")
            elif val == 'wait':
                i += 1; res, i = self.expr(tokens, i); time.sleep(float(res))
            elif val == 'input':
                var = tokens[i+1][1]; p = tokens[i+2][1].strip('"') if tokens[i+2][0] == 'STRING' else "Input"
                self.vars[var] = input(f"{p}: "); i += 3
            elif val == 'if':
                i += 1; cond, i = self.expr(tokens, i)
                start_if = i; d = 1; else_pos = -1
                while d > 0 and i < len(tokens):
                    if tokens[i][1] in ['if', 'loop', 'while', 'for']: d += 1
                    if tokens[i][1] == 'else' and d == 1: else_pos = i
                    if tokens[i][1] == 'end': d -= 1
                    i += 1
                if cond: self.run(tokens[start_if:else_pos if else_pos != -1 else i-1])
                elif else_pos != -1: self.run(tokens[else_pos+1:i-1])
            elif val == 'loop':
                i += 1; count, i = self.expr(tokens, i)
                start = i; d = 1
                while d > 0 and i < len(tokens):
                    if tokens[i][1] in ['if', 'loop', 'while', 'for']: d += 1
                    if tokens[i][1] == 'end': d -= 1
                    i += 1
                for _ in range(int(count)): self.run(tokens[start:i-1])
            elif val == 'while':
                cond_s = i + 1; i += 1
                _, i = self.expr(tokens, i)
                loop_s = i; d = 1
                while d > 0 and i < len(tokens):
                    if tokens[i][1] in ['if', 'loop', 'while', 'for']: d += 1
                    if tokens[i][1] == 'end': d -= 1
                    i += 1
                loop_e = i - 1
                while True:
                    cond, _ = self.expr(tokens, cond_s)
                    if not cond: break
                    self.run(tokens[loop_s:loop_e])
            elif val == 'fn':
                name = tokens[i+1][1]; i += 2; params = []
                if tokens[i][0] == 'LPAREN':
                    i += 1
                    while i < len(tokens) and tokens[i][0] != 'RPAREN':
                        if tokens[i][0] == 'ID': params.append(tokens[i][1])
                        i += 1
                    i += 1
                start = i; d = 1
                while d > 0 and i < len(tokens):
                    if tokens[i][1] in ['fn', 'if', 'loop', 'while', 'for']: d += 1
                    if tokens[i][1] == 'end': d -= 1
                    i += 1
                self.funcs[name] = {'params': params, 'tokens': tokens[start:i-1]}
            elif val == 'return':
                i += 1; res, i = self.expr(tokens, i)
                self.vars['return'] = res
                return 
            elif val == 'import':
                i += 1; path, i = self.expr(tokens, i)
                self.import_lib(path)
            elif val == '+add':
                i += 1; 
                if tokens[i][0] == 'LPAREN':
                    i += 1
                    if tokens[i][1] == 'libs': i += 1
                    content, i = self.expr(tokens, i)
                    self.handle_add_directive(content)
                    if tokens[i][0] == 'RPAREN': i += 1
            elif kind == 'MODULE_FUNC':
                _, i = self.parse_module_call(tokens, i)
            elif kind == 'ID' and val in self.funcs:
                self.execute_fn(val); i += 1
            else: i += 1
    def parse_module_call(self, tokens, i):
        fn_full = tokens[i][1]; mod, func = fn_full.split('.'); i += 2; args = []
        while i < len(tokens) and tokens[i][0] != 'RPAREN':
            if tokens[i][0] == 'COMMA': i += 1; continue
            res, i = self.expr(tokens, i); args.append(res)
        if i < len(tokens): i += 1
        return self.call_stdlib(mod, func, args), i
    def call_stdlib(self, mod, func, args):
        if mod in self.stdlib and func in self.stdlib[mod]:
            f = self.stdlib[mod][func]
            try:
                return f(*args) if callable(f) else f
            except Exception as e: return f"ERR: {e}"
        return f"MISSING: {mod}.{func}"
    def expr(self, tokens, i):
        if i >= len(tokens): return "", i
        res = ""; kind, val = tokens[i]
        if kind == 'MODULE_FUNC': res, i = self.parse_module_call(tokens, i)
        elif kind == 'NUMBER': res = float(val) if '.' in val else int(val); i += 1
        elif kind == 'STRING': res = val.strip('"'); i += 1
        elif kind == 'ID': res = self.vars.get(val, val); i += 1
        while i < len(tokens) and tokens[i][0] == 'OP':
            op = tokens[i][1]; i += 1; right, i = self.expr(tokens, i)
            try:
                if op == '+': res = res + right if isinstance(res, (int,float)) and isinstance(right, (int,float)) else str(res)+str(right)
                elif op == '-': res -= right
                elif op == '*': res *= right
                elif op == '/': res /= right
                elif op == '==': res = (res == right)
                elif op == '!=': res = (res != right)
                elif op == '>': res = (res > right)
                elif op == '<': res = (res < right)
                elif op == '&&': res = bool(res) and bool(right)
                elif op == '||': res = bool(res) or bool(right)
            except: res = str(res) + str(right)
        return res, i
if __name__ == "__main__":
    engine = NexusUltraV4()
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r', encoding='utf-8') as f:
            engine.run(engine.tokenize(f.read()))
    else:
        print(f"Nexus Titan Language v{engine.version} (Python Engine)")
        print("Usage: nexus4 <script.nx>")
