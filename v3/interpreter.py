#!/usr/bin/env python3
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

class NexusTitanV3:
    def __init__(self):
        self.vars = {
            "OS": platform.system(),
            "VER": "3.0-NEXUS",
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
        
        # --- V1 / V2 Compatibility Math ---
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

        # --- V1 / V2 Compatibility String ---
        self.stdlib['str'] = {
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

        # --- V3 Expanded System Functions ---
        self.stdlib['sys'] = {
            'os': lambda: platform.system(), 'ver': lambda: platform.version(), 'arch': lambda: platform.machine(),
            'user': lambda: os.getenv("USER") or os.getenv("USERNAME") or "NexusUser", 
            'cwd': os.getcwd, 'setcwd': os.chdir, 'env': lambda k: os.getenv(k),
            'exit': sys.exit, 'pid': os.getpid, 'cpu_count': os.cpu_count, 
            'shell': lambda c: subprocess.getoutput(c),
            'run': lambda c: subprocess.run(c, shell=True).returncode, 
            'sleep': time.sleep, 'time': time.time, 'info': self.get_sys_info,
            'argv': lambda: sys.argv, 'path': lambda: sys.path, 'plat': lambda: sys.platform,
            'node': platform.node, 'proc': platform.processor, 'rel': platform.release,
            'mem_total': lambda: shutil.disk_usage(os.getcwd()).total,
            'mem_used': lambda: shutil.disk_usage(os.getcwd()).used,
            'mem_free': lambda: shutil.disk_usage(os.getcwd()).free,
            'setenv': lambda k,v: os.environ.update({str(k): str(v)}),
            'delenv': lambda k: os.environ.pop(str(k), None),
            'getenv': lambda k,d="": os.environ.get(str(k), d),
            'getppid': os.getppid if hasattr(os, 'getppid') else lambda: -1,
            'is_64bit': lambda: sys.maxsize > 2**32,
            'python_ver': platform.python_version,
            'whoami': lambda: subprocess.getoutput('whoami'),
            'clear': lambda: os.system('cls' if os.name == 'nt' else 'clear'),
            'getlogin': os.getlogin if hasattr(os, 'getlogin') else lambda: "unknown",
            'system_alias': platform.system_alias,
            'mac_ver': platform.mac_ver,
            'win32_ver': platform.win32_ver,
            'libc_ver': platform.libc_ver,
            'architecture': platform.architecture,
            'boot_time': lambda: getattr(os, 'stat', lambda x: type('obj', (object,), {'st_ctime': 0})('/'))('/').st_ctime,
            'executable': lambda: sys.executable,
            'byteorder': lambda: sys.byteorder,
            'maxsize': lambda: sys.maxsize,
            'getdefaultencoding': sys.getdefaultencoding,
            'getfilesystemencoding': sys.getfilesystemencoding,
        }

        # --- V3 Sudo / Admin Support ---
        self.stdlib['admin'] = {
            'is_admin': self.is_admin,
            'run_admin': self.run_admin,
            'elevate': self.elevate,
            'sudo': lambda cmd: self.run_admin(cmd),
            'get_euid': lambda: os.geteuid() if hasattr(os, 'geteuid') else -1,
            'get_egid': lambda: os.getegid() if hasattr(os, 'getegid') else -1,
            'get_uid': lambda: os.getuid() if hasattr(os, 'getuid') else -1,
            'get_gid': lambda: os.getgid() if hasattr(os, 'getgid') else -1,
            'set_uid': lambda u: os.setuid(int(u)) if hasattr(os, 'setuid') else False,
            'set_gid': lambda g: os.setgid(int(g)) if hasattr(os, 'setgid') else False,
        }

        # --- V3 File System (IO/FS) Functions ---
        self.stdlib['io'] = {
            'read': lambda f: open(f, 'r', encoding='utf-8').read() if os.path.exists(f) else "Error",
            'write': lambda f,d: open(f, 'w', encoding='utf-8').write(str(d)), 
            'append': lambda f,d: open(f, 'a', encoding='utf-8').write(str(d)),
            'exists': os.path.exists, 'isdir': os.path.isdir, 'isfile': os.path.isfile,
            'mkdir': os.makedirs, 'rmdir': shutil.rmtree, 'remove': os.remove, 'rename': os.rename,
            'copy': shutil.copy, 'move': shutil.move, 'size': os.path.getsize, 'list': os.listdir,
            'ext': lambda p: os.path.splitext(p)[1], 'base': os.path.basename, 'abs': os.path.abspath,
            'atime': os.path.getatime, 'mtime': os.path.getmtime, 'ctime': os.path.getctime,
            'islink': os.path.islink, 'same': os.path.samefile, 'join': os.path.join,
            'split': os.path.split, 'dirname': os.path.dirname, 'realpath': os.path.realpath,
            'touch': lambda f: open(f, 'a').close() or os.utime(f, None),
            'read_lines': lambda f: open(f, 'r', encoding='utf-8').readlines() if os.path.exists(f) else [],
            'write_lines': lambda f, l: open(f, 'w', encoding='utf-8').writelines(l),
            'copytree': shutil.copytree,
            'chown': lambda f, u, g: shutil.chown(f, user=u, group=g) if hasattr(shutil, 'chown') else False,
            'chmod': lambda f, m: os.chmod(f, int(str(m), 8)),
            'symlink': os.symlink if hasattr(os, 'symlink') else lambda s, d: False,
            'readlink': os.readlink if hasattr(os, 'readlink') else lambda p: "",
            'link': os.link if hasattr(os, 'link') else lambda s, d: False,
            'glob': glob.glob,
            'iglob': lambda p: list(glob.iglob(p)),
            'file_hash_md5': lambda f: hashlib.md5(open(f, 'rb').read()).hexdigest() if os.path.exists(f) else "",
            'file_hash_sha256': lambda f: hashlib.sha256(open(f, 'rb').read()).hexdigest() if os.path.exists(f) else "",
            'get_temp_dir': lambda: __import__('tempfile').gettempdir(),
            'get_home_dir': lambda: os.path.expanduser('~'),
            'cwd': os.getcwd,
            'walk': lambda d: list(os.walk(d)),
            'stat': lambda f: str(os.stat(f)),
            'mount_points': self.get_mount_points,
        }

        # --- V3 ZIP / Archive Functions ---
        self.stdlib['zip'] = {
            'compress': self.zip_compress,
            'extract': self.zip_extract,
            'extract_all': self.zip_extract,
            'list': self.zip_list,
            'read_file': self.zip_read_file,
            'add_file': self.zip_add_file,
            'is_zipfile': zipfile.is_zipfile,
            'create_tar': lambda t, s: shutil.make_archive(t, 'tar', s),
            'create_gztar': lambda t, s: shutil.make_archive(t, 'gztar', s),
            'unpack_archive': shutil.unpack_archive,
            'get_archive_formats': lambda: [fmt[0] for fmt in shutil.get_archive_formats()],
        }

        # --- V3 Browser / Web Functions ---
        self.stdlib['browser'] = {
            'open': webbrowser.open,
            'open_new': webbrowser.open_new,
            'open_tab': webbrowser.open_new_tab,
            'get': lambda: str(webbrowser.get()),
        }

        # --- V3 Web & Net Functions ---
        self.stdlib['net'] = {
            'get': lambda u: urllib.request.urlopen(u).read().decode('utf-8'),
            'post': self.http_post,
            'download': lambda u,f: urllib.request.urlretrieve(u, f),
            'ping': lambda h: subprocess.run(f"ping -n 1 {h}" if os.name=='nt' else f"ping -c 1 {h}", shell=True).returncode == 0,
            'host': lambda: platform.node(), 
            'ip': lambda: subprocess.getoutput("hostname -I" if os.name!='nt' else "hostname"),
            'url': lambda u: urllib.parse.quote(u), 
            'unurl': lambda u: urllib.parse.unquote(u),
            'urlencode': lambda d: urllib.parse.urlencode(json.loads(d) if isinstance(d, str) else d),
            'parse_qs': lambda q: json.dumps(urllib.parse.parse_qs(q)),
            'ip_local': lambda: __import__('socket').gethostbyname(__import__('socket').gethostname()),
            'is_connected': self.is_connected,
            'request_headers': self.http_get_headers,
            'get_json': self.http_get_json,
        }

        # --- V3 Date & Time ---
        self.stdlib['date'] = {
            'now': lambda: str(datetime.datetime.now()), 'year': lambda: datetime.datetime.now().year,
            'month': lambda: datetime.datetime.now().month, 'day': lambda: datetime.datetime.now().day,
            'hour': lambda: datetime.datetime.now().hour, 'min': lambda: datetime.datetime.now().minute,
            'sec': lambda: datetime.datetime.now().second, 'ts': time.time, 'today': datetime.date.today,
            'weekday': lambda: datetime.datetime.now().weekday(), 'iso': lambda: datetime.datetime.now().isoformat(),
            'fmt': lambda f: datetime.datetime.now().strftime(str(f)),
            'utc_now': lambda: str(datetime.datetime.now(datetime.timezone.utc).replace(tzinfo=None)),
            'from_ts': lambda ts: str(datetime.datetime.fromtimestamp(float(ts))),
            'sleep_ms': lambda ms: time.sleep(float(ms)/1000.0),
            'perf_counter': time.perf_counter,
            'process_time': time.process_time,
            'timezone': lambda: time.tzname[0],
            'is_leap_year': lambda y: (int(y) % 4 == 0 and int(y) % 100 != 0) or (int(y) % 400 == 0),
        }

        # --- V3 Random ---
        self.stdlib['rnd'] = {
            'val': random.random, 'int': random.randint, 'float': random.uniform,
            'choice': random.choice, 'shuffle': lambda l: (random.shuffle(l), l)[1] if isinstance(l, list) else l, 
            'seed': random.seed, 'sample': random.sample, 'gauss': random.gauss, 
            'tri': random.triangular, 'range': random.randrange,
            'bytes': lambda n: random.randbytes(int(n)).hex(),
            'hex': lambda: "%06x" % random.randint(0, 0xFFFFFF),
            'uuid4': lambda: __import__('uuid').uuid4().hex,
        }

        # --- V3 Crypto / Hashing ---
        self.stdlib['crypto'] = {
            'md5': lambda s: hashlib.md5(str(s).encode()).hexdigest(),
            'sha1': lambda s: hashlib.sha1(str(s).encode()).hexdigest(),
            'sha256': lambda s: hashlib.sha256(str(s).encode()).hexdigest(),
            'sha512': lambda s: hashlib.sha512(str(s).encode()).hexdigest(),
            'b64encode': lambda s: __import__('base64').b64encode(str(s).encode()).decode(),
            'b64decode': lambda s: __import__('base64').b64decode(str(s).encode()).decode(),
            'rot13': lambda s: __import__('codecs').encode(str(s), 'rot_13'),
        }

        # --- V3 GUI ---
        if HAS_TK:
            self.stdlib['gui'] = {
                'window': self.gui_window, 'label': self.gui_label, 'button': self.gui_button,
                'msg': lambda m: messagebox.showinfo("Nexus V3", str(m)),
                'warn': lambda m: messagebox.showwarning("Nexus V3", str(m)),
                'err': lambda m: messagebox.showerror("Nexus V3", str(m)),
                'ask': lambda m: messagebox.askyesno("Nexus V3", str(m)),
                'input': lambda p: simpledialog.askstring("Nexus V3", str(p)),
                'run': lambda: self.root.mainloop() if self.root else None,
                'geom': lambda g: self.root.geometry(str(g)) if self.root else None,
                'title': lambda t: self.root.title(str(t)) if self.root else None,
                'clear': lambda: [c.destroy() for c in self.root.winfo_children()] if self.root else None,
                'file_dialog': lambda: filedialog.askopenfilename(),
                'dir_dialog': lambda: filedialog.askdirectory(),
                'save_dialog': lambda: filedialog.asksaveasfilename(),
                'color_dialog': lambda: __import__('tkinter.colorchooser').colorchooser.askcolor()[1],
                'bgcolor': lambda c: self.root.configure(bg=str(c)) if self.root else None,
                'resizable': lambda x,y: self.root.resizable(bool(x), bool(y)) if self.root else None,
                'destroy': lambda: self.root.destroy() if self.root else None,
                'update': lambda: self.root.update() if self.root else None,
                'entry': self.gui_entry,
                'get_entry': self.gui_get_entry,
            }
        else:
            self.stdlib['gui'] = {}

        # --- V3 JSON ---
        self.stdlib['json'] = {
            'parse': json.loads, 'str': json.dumps, 'load': lambda f: json.load(open(f)),
            'save': lambda f,d: json.dump(d, open(f, 'w')),
            'valid': lambda s: (lambda s: True if json.loads(s) else True)(s),
            'pretty': lambda d: json.dumps(d, indent=4),
            'get_keys': lambda j: list(json.loads(j).keys()) if isinstance(j, str) else list(j.keys()),
        }

        # --- V3 CLI ---
        self.stdlib['cli'] = {
            'clear': lambda: os.system('cls' if os.name == 'nt' else 'clear'),
            'red': lambda s: f"\033[91m{s}\033[0m", 'green': lambda s: f"\033[92m{s}\033[0m",
            'blue': lambda s: f"\033[94m{s}\033[0m", 'reset': lambda s: f"\033[0m{s}",
            'bold': lambda s: f"\033[1m{s}\033[0m", 'under': lambda s: f"\033[4m{s}\033[0m",
            'yellow': lambda s: f"\033[93m{s}\033[0m", 'cyan': lambda s: f"\033[96m{s}\033[0m",
            'magenta': lambda s: f"\033[95m{s}\033[0m", 'white': lambda s: f"\033[97m{s}\033[0m",
            'bg_red': lambda s: f"\033[41m{s}\033[0m", 'bg_green': lambda s: f"\033[42m{s}\033[0m",
            'bg_blue': lambda s: f"\033[44m{s}\033[0m", 'bg_yellow': lambda s: f"\033[43m{s}\033[0m",
            'print': print,
            'pause': lambda: input("Press Enter to continue..."),
            'cursor_up': lambda n: print(f"\033[{n}A", end=""),
            'cursor_down': lambda n: print(f"\033[{n}B", end=""),
            'cursor_right': lambda n: print(f"\033[{n}C", end=""),
            'cursor_left': lambda n: print(f"\033[{n}D", end=""),
        }

    # ==========================
    # --- V3 Admin Functions ---
    # ==========================
    def is_admin(self):
        try:
            if os.name == 'nt':
                return ctypes.windll.shell32.IsUserAnAdmin() != 0
            else:
                return os.geteuid() == 0
        except:
            return False

    def elevate(self):
        if self.is_admin(): return True
        try:
            if os.name == 'nt':
                ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)
                sys.exit()
            else:
                os.execvp("sudo", ["sudo", sys.executable] + sys.argv)
        except Exception as e:
            print("Elevation failed:", e)
            return False

    def run_admin(self, cmd):
        if self.is_admin():
            return subprocess.run(cmd, shell=True).returncode
        try:
            if os.name == 'nt':
                return ctypes.windll.shell32.ShellExecuteW(None, "runas", "cmd.exe", f"/c {cmd}", None, 1)
            else:
                return subprocess.run(f"sudo {cmd}", shell=True).returncode
        except Exception as e:
            return -1

    # ==========================
    # --- V3 ZIP Functions ---
    # ==========================
    def zip_compress(self, src, dst):
        with zipfile.ZipFile(dst, 'w', zipfile.ZIP_DEFLATED) as zipf:
            if os.path.isdir(src):
                for root, dirs, files in os.walk(src):
                    for file in files:
                        zipf.write(os.path.join(root, file), 
                                   os.path.relpath(os.path.join(root, file), 
                                   os.path.join(src, '..')))
            else:
                zipf.write(src, os.path.basename(src))
        return True

    def zip_extract(self, src, dst):
        with zipfile.ZipFile(src, 'r') as zipf:
            zipf.extractall(dst)
        return True

    def zip_list(self, src):
        with zipfile.ZipFile(src, 'r') as zipf:
            return zipf.namelist()

    def zip_read_file(self, src, filename):
        with zipfile.ZipFile(src, 'r') as zipf:
            return zipf.read(filename).decode('utf-8')

    def zip_add_file(self, zip_path, file_path, arcname=None):
        with zipfile.ZipFile(zip_path, 'a') as zipf:
            zipf.write(file_path, arcname or os.path.basename(file_path))
        return True

    # ==========================
    # --- V3 Net Functions ---
    # ==========================
    def http_post(self, url, data):
        if isinstance(data, dict): data = urllib.parse.urlencode(data).encode()
        elif isinstance(data, str): data = data.encode()
        req = urllib.request.Request(url, data=data)
        return urllib.request.urlopen(req).read().decode('utf-8')
        
    def is_connected(self):
        try:
            urllib.request.urlopen('http://8.8.8.8', timeout=1)
            return True
        except:
            return False
            
    def http_get_headers(self, url):
        req = urllib.request.Request(url, method='HEAD')
        return str(urllib.request.urlopen(req).headers)

    def http_get_json(self, url):
        res = urllib.request.urlopen(url).read().decode('utf-8')
        return json.loads(res)

    def get_mount_points(self):
        if os.name == 'nt':
            drives = []
            bitmask = ctypes.windll.kernel32.GetLogicalDrives()
            for letter in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ':
                if bitmask & 1: drives.append(letter + ':\\')
                bitmask >>= 1
            return drives
        else:
            return [line.split()[1] for line in subprocess.getoutput("df -h").split('\n')[1:]]

    # ==========================
    # --- V3 GUI Extension ---
    # ==========================
    def gui_entry(self, name, default=""):
        if self.root:
            if not hasattr(self, 'gui_entries'): self.gui_entries = {}
            e = tk.Entry(self.root)
            e.insert(0, str(default))
            e.pack(pady=5)
            self.gui_entries[name] = e

    def gui_get_entry(self, name):
        if hasattr(self, 'gui_entries') and name in self.gui_entries:
            return self.gui_entries[name].get()
        return ""

    def get_sys_info(self):
        info = f"Nexus OS: {self.vars['OS']} | Version: {self.vars['VER']} | Arch: {self.vars['ARCH']} | User: {self.vars['USER']}"
        print(f"Nexus V3 › {info}")
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
        mod_keys = '|'.join(self.stdlib.keys())
        tokens_def = [
            ('COMMENT', r'#.*'), 
            ('KEYWORD', r'\b(fn|set|out|if|else|loop|end|wait|input|return|break|import|while|for|repeat)\b'),
            ('NUMBER', r'\d+(\.\d+)?'), 
            ('STRING', r'"[^"]*"'),
            ('MODULE_FUNC', rf'\b({mod_keys})\.[a-zA-Z_]\w*'),
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
                if self.root and HAS_TK:
                    self.vars[var_name] = simpledialog.askstring("Nexus V3", str(prompt))
                else:
                    self.vars[var_name] = input(str(prompt)+": ")
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
            engine = NexusTitanV3()
            engine.run(engine.tokenize(f.read()))
    else:
        print("Nexus Titan Language v3.0-NEXUS")
        print("Usage: nexus <script.nx>")
