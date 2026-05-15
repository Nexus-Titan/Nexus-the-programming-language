import socket, subprocess, os
def scan_port(host, port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(0.5)
    try:
        s.connect((host, int(port)))
        return 1
    except:
        return 0
    finally:
        s.close()
def get_public_ip():
    import urllib.request
    try:
        return urllib.request.urlopen('https://api.ipify.org').read().decode('utf8')
    except:
        return "Offline"
def check_service(name):
    if os.name != 'nt':
        try:
            subprocess.check_output(["pgrep", name])
            return 1
        except:
            return 0
    return -1
