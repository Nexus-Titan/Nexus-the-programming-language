import os, platform, subprocess, shutil, time
def get_cpu_load():
    if os.name == 'nt':
        try:
            out = subprocess.check_output("wmic cpu get loadpercentage", shell=True).decode()
            return out.split('\n')[1].strip() + "%"
        except: return "Unknown"
    else:
        try:
            return os.popen("top -bn1 | grep 'Cpu(s)' | sed 's/.*, *\\([0-9.]*\\)%* id.*/\\1/' | awk '{print 100 - $1\"%\"}'").read().strip()
        except: return "Unknown"
def get_ram_usage():
    total, used, free = shutil.disk_usage("/")
    if os.path.exists("/proc/meminfo"):
        with open("/proc/meminfo", "r") as f:
            lines = f.readlines()
            total_mem = int(lines[0].split()[1])
            free_mem = int(lines[1].split()[1])
            used_mem = total_mem - free_mem
            return f"{(used_mem / total_mem * 100):.1f}%"
    return "N/A"
def list_active_processes():
    if os.name == 'nt':
        return subprocess.check_output("tasklist /FI \"STATUS eq RUNNING\"", shell=True).decode()
    else:
        return subprocess.check_output("ps -eo pid,comm,pcpu --sort=-pcpu | head -n 10", shell=True).decode()
def get_disk_report():
    usage = shutil.disk_usage("/")
    return {
        "total_gb": usage.total // (2**30),
        "used_gb": usage.used // (2**30),
        "free_gb": usage.free // (2**30),
        "percent": f"{(usage.used / usage.total * 100):.1f}%"
    }
