use std::collections::HashMap;
use std::env;
use std::fs;
use std::io::{self, Write};
use std::process;
use std::time::{SystemTime, UNIX_EPOCH};
struct NexusInterpreter {
    variables: HashMap<String, String>,
    functions: HashMap<String, Vec<String>>,
}
impl NexusInterpreter {
    fn new() -> Self {
        NexusInterpreter {
            variables: HashMap::new(),
            functions: HashMap::new(),
        }
    }
    fn resolve_value(&self, input: &str) -> String {
        let trimmed = input.trim();
        if trimmed.starts_with('"') && trimmed.ends_with('"') {
            return trimmed[1..trimmed.len() - 1].to_string();
        }
        if trimmed == "math.pi()" {
            return std::f64::consts::PI.to_string();
        }
        if trimmed == "math.e()" {
            return std::f64::consts::E.to_string();
        }
        if trimmed == "math.tau()" {
            return std::f64::consts::TAU.to_string();
        }
        if trimmed.starts_with("math.") {
            if let Some(result) = self.parse_math_function(trimmed) {
                return result;
            }
        }
        if trimmed.starts_with("str.") {
            if let Some(result) = self.parse_str_function(trimmed) {
                return result;
            }
        }
        if trimmed.starts_with("sys.") {
            if let Some(result) = self.parse_sys_function(trimmed) {
                return result;
            }
        }
        if trimmed.starts_with("date.") {
            if let Some(result) = self.parse_date_function(trimmed) {
                return result;
            }
        }
        if trimmed.starts_with("rnd.") {
            if let Some(result) = self.parse_rnd_function(trimmed) {
                return result;
            }
        }
        if trimmed.starts_with("io.") {
            if let Some(result) = self.parse_io_function(trimmed) {
                return result;
            }
        }
        if let Some(value) = self.variables.get(trimmed) {
            return value.clone();
        }
        if let Some(pos) = trimmed.find('+') {
            let left = self.resolve_value(&trimmed[..pos]);
            let right = self.resolve_value(&trimmed[pos + 1..]);
            return format!("{}{}", left, right);
        }
        trimmed.to_string()
    }
    fn parse_math_function(&self, input: &str) -> Option<String> {
        if let Some(paren_pos) = input.find('(') {
            let func_name = &input[5..paren_pos];
            let args_end = input.rfind(')')?;
            let args_str = &input[paren_pos + 1..args_end];
            if func_name == "pow" {
                let parts: Vec<&str> = args_str.split(',').collect();
                if parts.len() == 2 {
                    let x = self.resolve_value(parts[0]).parse::<f64>().ok()?;
                    let y = self.resolve_value(parts[1]).parse::<f64>().ok()?;
                    return Some(x.powf(y).to_string());
                }
            }
            let arg = self.resolve_value(args_str);
            let val = arg.parse::<f64>().ok()?;
            let result = match func_name {
                "sin" => val.sin(),
                "cos" => val.cos(),
                "tan" => val.tan(),
                "asin" => val.asin(),
                "acos" => val.acos(),
                "atan" => val.atan(),
                "sinh" => val.sinh(),
                "cosh" => val.cosh(),
                "tanh" => val.tanh(),
                "sqrt" => val.sqrt(),
                "cbrt" => val.cbrt(),
                "abs" => val.abs(),
                "ceil" => val.ceil(),
                "floor" => val.floor(),
                "round" => val.round(),
                "log" => val.ln(),
                "log10" => val.log10(),
                "log2" => val.log2(),
                "exp" => val.exp(),
                "trunc" => val.trunc(),
                "deg" => val.to_degrees(),
                "rad" => val.to_radians(),
                _ => return None,
            };
            return Some(result.to_string());
        }
        None
    }
    fn parse_str_function(&self, input: &str) -> Option<String> {
        if let Some(paren_pos) = input.find('(') {
            let func_name = &input[4..paren_pos];
            let args_end = input.rfind(')')?;
            let args_str = &input[paren_pos + 1..args_end];
            let arg = self.resolve_value(args_str);
            let result = match func_name {
                "len" => arg.len().to_string(),
                "upper" => arg.to_uppercase(),
                "lower" => arg.to_lowercase(),
                "reverse" => arg.chars().rev().collect(),
                "trim" => arg.trim().to_string(),
                "cap" => {
                    let mut chars = arg.chars();
                    match chars.next() {
                        None => String::new(),
                        Some(first) => first.to_uppercase().collect::<String>() + chars.as_str(),
                    }
                }
                _ => return None,
            };
            return Some(result);
        }
        None
    }
    fn parse_sys_function(&self, input: &str) -> Option<String> {
        match input {
            "sys.os()" => Some(env::consts::OS.to_string()),
            "sys.user()" => env::var("USER")
                .or_else(|_| env::var("USERNAME"))
                .ok(),
            "sys.time()" => {
                let now = SystemTime::now()
                    .duration_since(UNIX_EPOCH)
                    .ok()?;
                Some(now.as_secs().to_string())
            }
            "sys.cwd()" => env::current_dir()
                .ok()
                .and_then(|p| p.to_str().map(|s| s.to_string())),
            "sys.info()" => Some("Nexus OS | v2.0-ULTRA | Rust-Engine".to_string()),
            _ => None,
        }
    }
    fn parse_date_function(&self, input: &str) -> Option<String> {
        use chrono::Local;
        let now = Local::now();
        match input {
            "date.now()" => Some(now.format("%Y-%m-%d %H:%M:%S").to_string()),
            "date.year()" => Some(now.year().to_string()),
            "date.month()" => Some(now.month().to_string()),
            "date.day()" => Some(now.day().to_string()),
            "date.hour()" => Some(now.hour().to_string()),
            "date.min()" => Some(now.minute().to_string()),
            "date.sec()" => Some(now.second().to_string()),
            "date.ts()" => {
                let timestamp = SystemTime::now()
                    .duration_since(UNIX_EPOCH)
                    .ok()?;
                Some(timestamp.as_secs().to_string())
            }
            _ => None,
        }
    }
    fn parse_rnd_function(&self, input: &str) -> Option<String> {
        use rand::Rng;
        if input == "rnd.val()" {
            let val: f64 = rand::thread_rng().gen();
            return Some(format!("{:.6}", val));
        }
        if input.starts_with("rnd.int(") {
            let args_start = input.find('(')? + 1;
            let args_end = input.rfind(')')?;
            let args_str = &input[args_start..args_end];
            let parts: Vec<&str> = args_str.split(',').collect();
            if parts.len() == 2 {
                let min = self.resolve_value(parts[0]).parse::<i32>().ok()?;
                let max = self.resolve_value(parts[1]).parse::<i32>().ok()?;
                let val = rand::thread_rng().gen_range(min..=max);
                return Some(val.to_string());
            }
        }
        None
    }
    fn parse_io_function(&self, input: &str) -> Option<String> {
        if input.starts_with("io.read(") {
            let args_start = input.find('(')? + 1;
            let args_end = input.rfind(')')?;
            let filename = self.resolve_value(&input[args_start..args_end]);
            return fs::read_to_string(&filename)
                .ok()
                .map(|content| content.replace('\n', " | "));
        }
        if input.starts_with("io.exists(") {
            let args_start = input.find('(')? + 1;
            let args_end = input.rfind(')')?;
            let path = self.resolve_value(&input[args_start..args_end]);
            return Some(if std::path::Path::new(&path).exists() {
                "1".to_string()
            } else {
                "0".to_string()
            });
        }
        None
    }
    fn execute_line(&mut self, line: &str) {
        let trimmed = line.trim();
        if trimmed.is_empty() || trimmed.starts_with('#') {
            return;
        }
        if trimmed.starts_with("out ") {
            let value = self.resolve_value(&trimmed[4..]);
            println!("Nexus › {}", value);
            return;
        }
        if trimmed.starts_with("set ") {
            if let Some(eq_pos) = trimmed.find('=') {
                let var_name = trimmed[4..eq_pos].trim().to_string();
                let value = self.resolve_value(&trimmed[eq_pos + 1..]);
                self.variables.insert(var_name, value);
            }
            return;
        }
        if trimmed.starts_with("wait ") {
            let value = self.resolve_value(&trimmed[5..]);
            if let Ok(seconds) = value.parse::<u64>() {
                std::thread::sleep(std::time::Duration::from_secs(seconds));
            }
            return;
        }
        if trimmed.starts_with("input ") {
            let parts: Vec<&str> = trimmed[6..].splitn(2, ' ').collect();
            if !parts.is_empty() {
                let var_name = parts[0];
                let prompt = if parts.len() > 1 {
                    self.resolve_value(parts[1])
                } else {
                    "Input".to_string()
                };
                print!("{}: ", prompt);
                io::stdout().flush().unwrap();
                let mut input = String::new();
                io::stdin().read_line(&mut input).unwrap();
                self.variables.insert(var_name.to_string(), input.trim().to_string());
            }
            return;
        }
        if trimmed.starts_with("gui.msg(") {
            if let Some(args_start) = trimmed.find('(') {
                if let Some(args_end) = trimmed.rfind(')') {
                    let arg = &trimmed[args_start + 1..args_end];
                    let value = self.resolve_value(arg);
                    println!("[GUI MESSAGE] {}", value);
                }
            }
            return;
        }
        if trimmed.starts_with("io.write(") {
            if let Some(args_start) = trimmed.find('(') {
                if let Some(args_end) = trimmed.rfind(')') {
                    let args = &trimmed[args_start + 1..args_end];
                    let parts: Vec<&str> = args.splitn(2, ',').collect();
                    if parts.len() == 2 {
                        let filename = self.resolve_value(parts[0]);
                        let content = self.resolve_value(parts[1]);
                        let _ = fs::write(&filename, format!("{}\n", content));
                    }
                }
            }
            return;
        }
        if trimmed == "sys.info()" {
            println!("Nexus › Nexus OS | v2.0-ULTRA | Rust-Engine");
            return;
        }
    }
    fn run(&mut self, lines: &[String]) {
        let mut i = 0;
        while i < lines.len() {
            let line = lines[i].trim();
            if line.starts_with("fn ") {
                if let Some(paren_pos) = line.find('(') {
                    let func_name = line[3..paren_pos].trim().to_string();
                    let mut func_lines = Vec::new();
                    i += 1;
                    while i < lines.len() && lines[i].trim() != "end" {
                        func_lines.push(lines[i].clone());
                        i += 1;
                    }
                    self.functions.insert(func_name, func_lines);
                }
                i += 1;
                continue;
            }
            if line.starts_with("if ") {
                let condition = self.resolve_value(&line[3..]);
                let cond_true = condition == "1" || condition.parse::<i32>().unwrap_or(0) != 0;
                i += 1;
                let mut depth = 1;
                let mut else_pos = None;
                let start = i;
                while i < lines.len() && depth > 0 {
                    let current = lines[i].trim();
                    if current.starts_with("if ") {
                        depth += 1;
                    } else if current == "else" && depth == 1 {
                        else_pos = Some(i);
                    } else if current == "end" {
                        depth -= 1;
                    }
                    i += 1;
                }
                if cond_true {
                    let end = else_pos.unwrap_or(i - 1);
                    let block: Vec<String> = lines[start..end].to_vec();
                    self.run(&block);
                } else if let Some(else_start) = else_pos {
                    let block: Vec<String> = lines[else_start + 1..i - 1].to_vec();
                    self.run(&block);
                }
                continue;
            }
            if line.starts_with("loop ") {
                let count_str = self.resolve_value(&line[5..]);
                let count = count_str.parse::<usize>().unwrap_or(0);
                i += 1;
                let mut depth = 1;
                let start = i;
                while i < lines.len() && depth > 0 {
                    let current = lines[i].trim();
                    if current.starts_with("loop ") {
                        depth += 1;
                    } else if current == "end" {
                        depth -= 1;
                    }
                    i += 1;
                }
                let block: Vec<String> = lines[start..i - 1].to_vec();
                for _ in 0..count {
                    self.run(&block);
                }
                continue;
            }
            if line.ends_with("()") {
                let func_name = line[..line.len() - 2].trim();
                if let Some(func_lines) = self.functions.get(func_name).cloned() {
                    self.run(&func_lines);
                }
                i += 1;
                continue;
            }
            self.execute_line(line);
            i += 1;
        }
    }
}
fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        println!("Nexus Titan Language v2.0-ULTRA (Rust)");
        println!("Usage: nexus <script.nx>");
        process::exit(0);
    }
    let filename = &args[1];
    let content = fs::read_to_string(filename).unwrap_or_else(|_| {
        eprintln!("Error: Could not open file {}", filename);
        process::exit(1);
    });
    let lines: Vec<String> = content.lines().map(|s| s.to_string()).collect();
    let mut interpreter = NexusInterpreter::new();
    interpreter.run(&lines);
}
