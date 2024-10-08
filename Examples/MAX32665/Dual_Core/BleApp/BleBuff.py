import threading

class BleBuff():
    def __init__(self):
        self.buffer = []
        self.lock = threading.Lock()

    def append(self, data):
        self.buffer.append(data)

    def pop(self, index):
        return self.buffer.pop(index)
    
    def clear(self):
        self.buffer.clear()
    
    def len(self):
        return len(self.buffer)