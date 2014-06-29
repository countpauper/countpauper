class FormatException(Exception):
    def __init__(self, text, *args):
        Exception.__init__(self, *args)
        self.message = text

    def __str__(self):
        return self.message.format(*self.args)

