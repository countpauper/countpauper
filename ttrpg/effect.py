class Effect(object):
    def __init__(self, name, duration=1, boni=None):
        self.name = name
        self.duration = duration
        self.boni = boni or dict()

    def get_boni(self, name):
        return self.boni.get(name, dict())

    def turn(self):
        if self.duration is None:
            return True
        self.duration -= 1
        return self.duration > 0

    def __str__(self):
        return self.name
