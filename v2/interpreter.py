#!/usr/bin/env python3
import re, sys, os, tkinter as tk, math, time, json, platform, shutil, random, subprocess
from tkinter import messagebox, simpledialog, filedialog
import urllib.request as request
import datetime
class NexusTitan:
    def __init__(self):
        self.vars = {
            "OS": platform.system(),
            "VER": "2.0-ULTRA",
            "ARCH": platform.machine(),
            "USER": os.getenv("USER") or os.getenv("USERNAME") or "NexusUser",
            "CWD": os.getcwd(),
            "PI": math.pi,
            "E": math.e
        }
        self.funcs = {}
        self.root = None
        self.setup_stdlib()
    def setup_stdlib(self):
        self.stdlib = {}
        math_map = {
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
        self.stdlib['math'] = math_map
        str_map = {
            'len': len, 'upper': lambda s: str(s).upper(), 'lower': lambda s: str(s).lower(),
            'cap': lambda s: str(s).capitalize(), 'trim': lambda s: str(s).strip(),
            'ltrim': lambda s: str(s).lstrip(), 'rtrim': lambda s: str(s).rstrip(),
            'replace': lambda s,o,n: str(s).replace(str(o), str(n)),
            'split': lambda s,d=' ': str(s).split(str(d)), 'join': lambda d,l: str(d).join(map(str, l)),
            'at': lambda s,i: str(s)[int(i)], 'find': lambda s,q: str(s).find(str(q)),
            'rfind': lambda s,q: str(s).rfind(str(q)), 'starts': lambda s,q: str(s).startswith(str(q)),
            'ends': lambda s,q: str(s).endswith(str(q)), 'isnum': lambda s: str(s).isdigit(),
            'isalpha': lambda s: str(s).isalpha(), 'isalnum': lambda s: str(s).isalnum(),
            'isspace': lambda s: str(s).isspace(), 'islower': lambda s: str(s).islower(),
            'isupper': lambda s: str(s).isupper(), 'istitle': lambda s: str(s).istitle(),
            'reverse': lambda s: str(s)[::-1], 'repeat': lambda s,n: str(s) * int(n), 
            'padl': lambda s,n,c=' ': str(s).rjust(int(n), str(c)),
            'padr': lambda s,n,c=' ': str(s).ljust(int(n), str(c)), 
            'slice': lambda s,a=0,b=None: str(s)[int(a):int(b) if b is not None else None],
            'hex': hex, 'bin': bin, 'ascii': ascii, 'chr': chr, 'ord': ord, 
            'count': lambda s,q: str(s).count(str(q)), 'zfill': lambda s,n: str(s).zfill(int(n)),
            'format': lambda s,*a: str(s).format(*a), 'swap': lambda s: str(s).swapcase(),
            'title': lambda s: str(s).title(), 'partition': lambda s,d: str(s).partition(str(d)),
            'rpartition': lambda s,d: str(s).rpartition(str(d)), 'expand': lambda s,t=8: str(s).expandtabs(int(t)),
            'isascii': lambda s: str(s).isascii(), 'isdecimal': lambda s: str(s).isdecimal(),
            'isident': lambda s: str(s).isidentifier(), 'isprint': lambda s: str(s).isprintable()
        }
        self.stdlib['str'] = str_map
        sys_map = {
            'os': lambda: platform.system(), 'ver': lambda: platform.version(), 'arch': lambda: platform.machine(),
            'user': lambda: os.getenv("USER") or os.getenv("USERNAME") or "NexusUser", 
            'cwd': os.getcwd, 'setcwd': os.chdir, 'env': lambda k: os.getenv(k),
            'exit': sys.exit, 'pid': os.getpid, 'cpu_count': os.cpu_count, 
            'shell': lambda c: subprocess.getoutput(c),
            'run': lambda c: subprocess.run(c, shell=True).returncode, 
            'sleep': time.sleep, 'time': time.time, 'info': self.get_sys_info,
            'argv': lambda: sys.argv, 'path': lambda: sys.path, 'plat': lambda: sys.platform,
            'node': platform.node, 'proc': platform.processor, 'rel': platform.release,
            'mem': lambda: shutil.disk_usage(os.getcwd())
        }
        self.stdlib['sys'] = sys_map
        io_map = {
            'read': lambda f: open(f, 'r').read() if os.path.exists(f) else "Error",
            'write': lambda f,d: open(f, 'w').write(str(d)), 'append': lambda f,d: open(f, 'a').write(str(d)),
            'exists': os.path.exists, 'isdir': os.path.isdir, 'isfile': os.path.isfile,
            'mkdir': os.makedirs, 'rmdir': shutil.rmtree, 'remove': os.remove, 'rename': os.rename,
            'copy': shutil.copy, 'move': shutil.move, 'size': os.path.getsize, 'list': os.listdir,
            'ext': lambda p: os.path.splitext(p)[1], 'base': os.path.basename, 'abs': os.path.abspath,
            'atime': os.path.getatime, 'mtime': os.path.getmtime, 'ctime': os.path.getctime,
            'islink': os.path.islink, 'same': os.path.samefile, 'join': os.path.join,
            'split': os.path.split, 'dirname': os.path.dirname, 'realpath': os.path.realpath
        }
        self.stdlib['io'] = io_map
        net_map = {
            'get': lambda u: request.urlopen(u).read().decode(),
            'download': lambda u,f: request.urlretrieve(u, f),
            'ping': lambda h: subprocess.run(f"ping -n 1 {h}" if os.name=='nt' else f"ping -c 1 {h}", shell=True).returncode == 0,
            'host': lambda: platform.node(), 'ip': lambda: subprocess.getoutput("hostname -I" if os.name!='nt' else "hostname"),
            'url': lambda u: request.quote(u), 'unurl': lambda u: request.unquote(u)
        }
        self.stdlib['net'] = net_map
        date_map = {
            'now': lambda: str(datetime.datetime.now()), 'year': lambda: datetime.datetime.now().year,
            'month': lambda: datetime.datetime.now().month, 'day': lambda: datetime.datetime.now().day,
            'hour': lambda: datetime.datetime.now().hour, 'min': lambda: datetime.datetime.now().minute,
            'sec': lambda: datetime.datetime.now().second, 'ts': time.time, 'today': datetime.date.today,
            'weekday': lambda: datetime.datetime.now().weekday(), 'iso': lambda: datetime.datetime.now().isoformat(),
            'fmt': lambda f: datetime.datetime.now().strftime(str(f))
        }
        self.stdlib['date'] = date_map
        rnd_map = {
            'val': random.random, 'int': random.randint, 'float': random.uniform,
            'choice': random.choice, 'shuffle': random.shuffle, 'seed': random.seed,
            'sample': random.sample, 'gauss': random.gauss, 'tri': random.triangular,
            'range': random.randrange
        }
        self.stdlib['rnd'] = rnd_map
        gui_map = {
            'window': self.gui_window, 'label': self.gui_label, 'button': self.gui_button,
            'msg': lambda m: messagebox.showinfo("Nexus", str(m)),
            'warn': lambda m: messagebox.showwarning("Nexus", str(m)),
            'err': lambda m: messagebox.showerror("Nexus", str(m)),
            'ask': lambda m: messagebox.askyesno("Nexus", str(m)),
            'input': lambda p: simpledialog.askstring("Nexus", str(p)),
            'run': lambda: self.root.mainloop() if self.root else None,
            'geom': lambda g: self.root.geometry(str(g)) if self.root else None,
            'title': lambda t: self.root.title(str(t)) if self.root else None,
            'clear': lambda: [c.destroy() for c in self.root.winfo_children()] if self.root else None
        }
        self.stdlib['gui'] = gui_map
        json_map = {
            'parse': json.loads, 'str': json.dumps, 'load': lambda f: json.load(open(f)),
            'save': lambda f,d: json.dump(d, open(f, 'w')),
            'valid': lambda s: (lambda s: True if json.loads(s) else True)(s)
        }
        self.stdlib['json'] = json_map
        cli_map = {
            'clear': lambda: os.system('cls' if os.name == 'nt' else 'clear'),
            'red': lambda s: f"\033[91m{s}\033[0m", 'green': lambda s: f"\033[92m{s}\033[0m",
            'blue': lambda s: f"\033[94m{s}\033[0m", 'reset': lambda s: f"\033[0m{s}",
            'bold': lambda s: f"\033[1m{s}\033[0m", 'under': lambda s: f"\033[4m{s}\033[0m",
            'yellow': lambda s: f"\033[93m{s}\033[0m", 'cyan': lambda s: f"\033[96m{s}\033[0m"
        }
        self.stdlib['cli'] = cli_map
    def get_sys_info(self):
        info = f"Nexus OS: {self.vars['OS']} | Version: {self.vars['VER']} | Arch: {self.vars['ARCH']}"
        print(f"Nexus › {info}")
        return info
    def gui_window(self, title, geom="400x300"):
        self.root = tk.Tk()
        self.root.title(title)
        self.root.geometry(geom)
    def gui_label(self, text):
        if self.root: tk.Label(self.root, text=text).pack(pady=10)
    def gui_button(self, text, func_name):
        if self.root: tk.Button(self.root, text=text, command=lambda: self.run(self.funcs[func_name])).pack(pady=5)
    def tokenize(self, code):
        tokens_def = [
            ('COMMENT', r'#.*'), 
            ('KEYWORD', r'\b(fn|set|out|if|else|loop|end|wait|input|return|break|import|while|for|repeat)\b'),
            ('NUMBER', r'\d+(\.\d+)?'), 
            ('STRING', r'"[^"]*"'),
            ('MODULE_FUNC', r'\b(math|str|sys|io|net|date|rnd|gui|json|cli)\.[a-zA-Z_]\w*'),
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
                var_name = tokens[i+1][1]
                prompt = tokens[i+2][1].strip('"') if tokens[i+2][0] == 'STRING' else "Input"
                self.vars[var_name] = simpledialog.askstring("Nexus", str(prompt)) if self.root else input(str(prompt)+": ")
                i += 3
            elif val == 'if':
                i += 1; cond, i = self.expr(tokens, i)
                if tokens[i][1] != 'fn':
                    pass
                start_if = i; d = 1; else_pos = -1
                while d > 0 and i < len(tokens):
                    if tokens[i][1] in ['if', 'fn', 'loop']: d += 1
                    if tokens[i][1] == 'else' and d == 1: else_pos = i
                    if tokens[i][1] == 'end': d -= 1
                    i += 1
                if cond:
                    self.run(tokens[start_if:else_pos if else_pos != -1 else i-1])
                elif else_pos != -1:
                    self.run(tokens[else_pos+1:i-1])
            elif val == 'loop':
                i += 1; count, i = self.expr(tokens, i)
                start_loop = i; d = 1
                while d > 0 and i < len(tokens):
                    if tokens[i][1] in ['if', 'fn', 'loop']: d += 1
                    if tokens[i][1] == 'end': d -= 1
                    i += 1
                for _ in range(int(count)):
                    self.run(tokens[start_loop:i-1])
            elif val == 'fn':
                name = tokens[i+1][1]; i += 4; start = i; d = 1
                while d > 0 and i < len(tokens):
                    if tokens[i][1] == 'fn': d += 1
                    if tokens[i][1] == 'end': d -= 1
                    i += 1
                self.funcs[name] = tokens[start:i-1]
            elif kind == 'MODULE_FUNC':
                _, i = self.parse_module_call(tokens, i)
            elif kind == 'ID' and val in self.funcs:
                self.run(self.funcs[val]); i += 1
            else: i += 1
    def parse_module_call(self, tokens, i):
        full_name = tokens[i][1]
        mod, func = full_name.split('.')
        i += 2
        args = []
        while i < len(tokens) and tokens[i][0] != 'RPAREN':
            if tokens[i][0] == 'COMMA': i += 1; continue
            arg_res, i = self.expr(tokens, i); args.append(arg_res)
        if i < len(tokens): i += 1
        return self.call_stdlib(mod, func, args), i
    def call_stdlib(self, mod, func, args):
        if mod in self.stdlib and func in self.stdlib[mod]:
            f = self.stdlib[mod][func]
            try: return f(*args) if callable(f) else f
            except Exception as e: return f"Error: {e}"
        return ""
    def expr(self, tokens, i):
        if i >= len(tokens): return "", i
        res = ""
        kind, val = tokens[i]
        if kind == 'MODULE_FUNC':
            res, i = self.parse_module_call(tokens, i)
        elif kind == 'NUMBER': res = float(val) if '.' in val else int(val); i += 1
        elif kind == 'STRING': res = val.strip('"'); i += 1
        elif kind == 'ID': res = self.vars.get(val, ""); i += 1
        while i < len(tokens) and tokens[i][0] == 'OP':
            op = tokens[i][1]; i += 1
            right, i = self.expr(tokens, i)
            try:
                if op == '+':
                    if isinstance(res, (int, float)) and isinstance(right, (int, float)): res += right
                    else: res = str(res) + str(right)
                elif op == '-': res -= right
                elif op == '*': res *= right
                elif op == '/': res /= right
                elif op == '==': res = (res == right)
                elif op == '!=': res = (res != right)
                elif op == '>': res = (res > right)
                elif op == '<': res = (res < right)
                elif op == '>=': res = (res >= right)
                elif op == '<=': res = (res <= right)
            except: res = str(res) + str(right)
        return res, i
if __name__ == "__main__":
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r', encoding='utf-8') as f:
            NexusTitan().run(NexusTitan().tokenize(f.read()))
    else:
        print("Nexus Titan Language v2.0-ULTRA")
        print("Usage: nexus <script.nx>")
