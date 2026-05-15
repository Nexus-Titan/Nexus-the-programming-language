class NexusUltraV4 {
    constructor() {
        this.version = "4.0-ULTRA";
        this.vars = {
            "OS": "Web",
            "VER": this.version,
            "ARCH": "WASM/JS",
            "USER": "WebUser",
            "CWD": "/",
            "PI": Math.PI,
            "E": Math.E,
            "ENGINE": "JavaScript"
        };
        this.funcs = {};
        this.setupStdlib();
    }
    setupStdlib() {
        this.stdlib = {
            'api': {
                'type': () => "WEB",
                'engine': () => "JavaScript",
                'version': () => this.version,
                'is_web': () => true,
                'is_pc': () => false,
                'call': (t, f, a) => console.log(`API Bridge › ${t}::${f}(${a})`),
                'sync': (d) => console.log("Sync ›", d)
            },
            'math': {
                'sin': Math.sin, 'cos': Math.cos, 'tan': Math.tan, 'asin': Math.asin, 'acos': Math.acos, 'atan': Math.atan,
                'sqrt': Math.sqrt, 'pow': Math.pow, 'abs': Math.abs, 'ceil': Math.ceil, 'floor': Math.floor, 'round': Math.round,
                'pi': () => Math.PI, 'e': () => Math.E, 'rnd': Math.random, 'log': Math.log, 'exp': Math.exp,
                'min': Math.min, 'max': Math.max
            },
            'str': {
                'len': (s) => String(s).length,
                'upper': (s) => String(s).toUpperCase(),
                'lower': (s) => String(s).toLowerCase(),
                'trim': (s) => String(s).trim(),
                'reverse': (s) => String(s).split('').reverse().join(''),
                'at': (s, i) => String(s).charAt(i),
                'split': (s, d) => String(s).split(d),
                'replace': (s, o, n) => String(s).replace(o, n),
                'starts': (s, q) => String(s).startsWith(q),
                'ends': (s, q) => String(s).endsWith(q)
            },
            'sys': {
                'os': () => "Web",
                'time': () => Date.now() / 1000,
                'info': () => `Nexus Ultra Web | v${this.version}`,
                'env': (k) => "Web (No Env)",
                'exit': () => window.close()
            },
            'io': {
                'read': (f) => localStorage.getItem(f) || "ERR: Not Found",
                'write': (f, d) => localStorage.setItem(f, d),
                'exists': (f) => localStorage.getItem(f) !== null,
                'remove': (f) => localStorage.removeItem(f),
                'list': () => Object.keys(localStorage)
            },
            'net': {
                'get': async (u) => { let r = await fetch(u); return await r.text(); },
                'ip': () => "127.0.0.1",
                'is_online': () => navigator.onLine
            },
            'gui': {
                'msg': (m) => alert(m),
                'input': (p) => prompt(p),
                'log': (m) => console.log("Nexus ›", m),
                'title': (t) => document.title = t,
                'bgcolor': (c) => document.body.style.backgroundColor = c
            },
            'date': {
                'now': () => new Date().toLocaleString(),
                'ts': () => Date.now(),
                'iso': () => new Date().toISOString()
            },
            'rnd': {
                'val': Math.random,
                'int': (min, max) => Math.floor(Math.random() * (max - min + 1)) + min,
                'uuid': () => crypto.randomUUID()
            },
            'json': {
                'parse': JSON.parse,
                'str': JSON.stringify,
                'pretty': (d) => JSON.stringify(d, null, 4)
            }
        };
    }
    tokenize(code) {
        const modKeys = Object.keys(this.stdlib).join('|');
        const tokensDef = [
            ['COMMENT', /#.*/],
            ['KEYWORD', /\b(fn|set|out|if|else|loop|while|for|end|wait|input|return|break|import)\b|\+add/],
            ['NUMBER', /\d+(\.\d+)?/],
            ['STRING', /"[^"]*"/],
            ['MODULE_FUNC', /\b[a-zA-Z_]\w*\.[a-zA-Z_]\w*/],
            ['ID', /[a-zA-Z_]\w*/],
            ['OP', /[+\-*/><=!&|]+/],
            ['LPAREN', /\(/], ['RPAREN', /\)/], ['COMMA', /,/],
            ['ASSIGN', /=/], ['SKIP', /[ \t\n\r]+/], ['MISMATCH', /./]
        ];
        let tokens = [];
        let pos = 0;
        while (pos < code.length) {
            let match = null;
            for (let [type, regex] of tokensDef) {
                let m = code.slice(pos).match(new RegExp('^' + regex.source));
                if (m) {
                    match = { type, value: m[0] };
                    break;
                }
            }
            if (!match) break;
            if (match.type !== 'SKIP' && match.type !== 'COMMENT') {
                tokens.push(match);
            }
            pos += match.value.length;
        }
        return tokens;
    }
    async run(tokens) {
        let i = 0;
        while (i < tokens.length) {
            let { type, value } = tokens[i];
            if (value === 'set') {
                let name = tokens[i + 1].value; i += 3;
                let [res, nextI] = await this.expr(tokens, i);
                this.vars[name] = res; i = nextI;
            } else if (value === 'out') {
                i++; let [res, nextI] = await this.expr(tokens, i);
                console.log(`Nexus › ${res}`); i = nextI;
            } else if (value === 'wait') {
                i++; let [res, nextI] = await this.expr(tokens, i);
                await new Promise(r => setTimeout(r, res * 1000)); i = nextI;
            } else if (value === 'if') {
                i++; let [cond, nextI] = await this.expr(tokens, i); i = nextI;
                let startIf = i; let d = 1; let elsePos = -1;
                while (d > 0 && i < tokens.length) {
                    if (['if', 'loop', 'while', 'for'].includes(tokens[i].value)) d++;
                    if (tokens[i].value === 'else' && d === 1) elsePos = i;
                    if (tokens[i].value === 'end') d--;
                    i++;
                }
                if (cond) await this.run(tokens.slice(startIf, elsePos !== -1 ? elsePos : i - 1));
                else if (elsePos !== -1) await this.run(tokens.slice(elsePos + 1, i - 1));
            } else if (value === 'loop') {
                i++; let [count, nextI] = await this.expr(tokens, i); i = nextI;
                let startLoop = i; let d = 1;
                while (d > 0 && i < tokens.length) {
                    if (['if', 'loop', 'while', 'for'].includes(tokens[i].value)) d++;
                    if (tokens[i].value === 'end') d--;
                    i++;
                }
                let loopTokens = tokens.slice(startLoop, i - 1);
                for (let c = 0; c < parseInt(count); c++) await this.run(loopTokens);
            } else if (value === 'while') {
                let condStart = i + 1; i++;
                let [_, nextI] = await this.expr(tokens, i); i = nextI;
                let loopStart = i; let d = 1;
                while (d > 0 && i < tokens.length) {
                    if (['if', 'loop', 'while', 'for'].includes(tokens[i].value)) d++;
                    if (tokens[i].value === 'end') d--;
                    i++;
                }
                let loopEnd = i - 1;
                let loopTokens = tokens.slice(loopStart, loopEnd);
                while (true) {
                    let [cond, _] = await this.expr(tokens, condStart);
                    if (!cond) break;
                    await this.run(loopTokens);
                }
            } else if (value === 'fn') {
                let name = tokens[i + 1].value; i += 4;
                let start = i; let d = 1;
                while (d > 0 && i < tokens.length) {
                    if (tokens[i].value === 'fn') d++;
                    if (tokens[i].value === 'end') d--;
                    i++;
                }
                this.funcs[name] = tokens.slice(start, i - 1);
            } else if (value === 'import') {
                i++; let [path, nextI] = await this.expr(tokens, i); i = nextI;
                await this.importLib(path);
            } else if (value === '+add') {
                i++;
                if (tokens[i].value === '(') {
                    i++;
                    if (tokens[i].value === 'libs') i++;
                    let [content, nextI] = await this.expr(tokens, i); i = nextI;
                    this.handleAddDirective(content);
                    if (tokens[i].value === ')') i++;
                }
            } else if (type === 'ID' && this.funcs[value]) {
                await this.run(this.funcs[value]); i++;
            } else { i++; }
        }
    }
    async parseModuleCall(tokens, i) {
        let fullName = tokens[i].value;
        let [mod, func] = fullName.split('.');
        i += 2;
        let args = [];
        while (i < tokens.length && tokens[i].type !== 'RPAREN') {
            if (tokens[i].type === 'COMMA') { i++; continue; }
            let [res, nextI] = await this.expr(tokens, i);
            args.push(res); i = nextI;
        }
        if (i < tokens.length) i++;
        return [await this.callStdlib(mod, func, args), i];
    }
    async callStdlib(mod, func, args) {
        if (this.stdlib[mod] && this.stdlib[mod][func]) {
            let f = this.stdlib[mod][func];
            return typeof f === 'function' ? await f(...args) : f;
        }
        return `ERR: ${mod}.${func}`;
    }
    async expr(tokens, i) {
        if (i >= tokens.length) return ["", i];
        let res = "";
        let { type, value } = tokens[i];
        if (type === 'MODULE_FUNC') {
            [res, i] = await this.parseModuleCall(tokens, i);
        } else if (type === 'NUMBER') {
            res = parseFloat(value); i++;
        } else if (type === 'STRING') {
            res = value.slice(1, -1); i++;
        } else if (type === 'ID') {
            res = this.vars[value] !== undefined ? this.vars[value] : value; i++;
        }
        while (i < tokens.length && tokens[i].type === 'OP') {
            let op = tokens[i].value; i++;
            let [right, nextI] = await this.expr(tokens, i);
            i = nextI;
            if (op === '+') res = (typeof res === 'number' && typeof right === 'number') ? res + right : String(res) + String(right);
            else if (op === '-') res -= right;
            else if (op === '*') res *= right;
            else if (op === '/') res /= right;
            else if (op === '==') res = (res == right);
            else if (op === '!=') res = (res != right);
            else if (op === '>') res = (res > right);
            else if (op === '<') res = (res < right);
            else if (op === '&&') res = (res && right);
            else if (op === '||') res = (res || right);
        }
        return [res, i];
    }
    async importLib(path) {
        path = String(path).trim();
        if (path.endsWith('.nx')) {
            const res = await fetch(path);
            const code = await res.text();
            await this.run(this.tokenize(code));
        } else if (path.endsWith('.js')) {
            const modName = path.split('/').pop().replace('.js', '');
            const res = await fetch(path);
            const jsCode = await res.text();
            const mod = eval(`(function(){ ${jsCode}; return typeof module !== 'undefined' ? module.exports : {}; })()`);
            this.stdlib[modName] = mod;
            console.log(`Nexus › Imported JS Module: ${modName}`);
        }
    }
    handleAddDirective(content) {
        content.split('\n').forEach(line => {
            line = line.trim();
            if (!line || line.startsWith('#')) return;
            if (line.startsWith('-')) {
                console.log(`Nexus › Enabling support for: ${line.slice(1)}`);
            }
        });
    }
}
if (typeof module !== 'undefined') module.exports = NexusUltraV4;
