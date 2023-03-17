class QuintException(Exception):

    def __init__(self, *message):
        super().__init__(' '.join(message))
        self.message = ' '.join(message)

    def __str__(self):
        return repr(self.message)