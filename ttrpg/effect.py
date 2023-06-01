class Effect(object):
    def __init__(self, name, duration=1, boni=dict()):
        self.name = name
        self.duration = duration
        self.boni = boni

    def get_boni(self, name):
        return self.boni.get(name, dict())
