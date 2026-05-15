def grayscale(r, g, b):
    return int(0.299*r + 0.587*g + 0.114*b)
def mean(data):
    return sum(data) / len(data) if data else 0
import threading
def run_parallel(target, args):
    t = threading.Thread(target=target, args=args)
    t.start()
    return t
def predict_linear(x, m, c):
    return m * x + c
