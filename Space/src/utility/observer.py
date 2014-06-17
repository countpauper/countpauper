class Observation(object):
    def __init__(self):
        self.observers = []

    # TODO: just call a provided callback, observers is dict
    def notify(self, event):
        for observer in self.observers:
            observer.notify(self, event)

    def register(self, observer):
        self.observers.append(observer)

    def unregister(self, observer):
        self.observers.remove(observer)

class Observer(object):
    def notify(self, observation, function):
        pass
