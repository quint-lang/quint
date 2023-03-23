from quint._lib import core


class QuintException(Exception):

    def __init__(self, *message):
        super().__init__(' '.join(message))
        self.message = ' '.join(message)

    def __str__(self):
        return repr(self.message)


class QuintSyntaxException(QuintException, SyntaxError):
    pass


class QuintNameException(QuintException, NameError):
    pass


class QuintIndexException(QuintException, IndexError):
    pass


class QuintTypeException(QuintException, TypeError):
    pass


class QuintRuntimeException(QuintException, RuntimeError):
    pass


class QuintAssertionException(QuintRuntimeException, AssertionError):
    pass


def handle_exception_from_cpp(exc):
    if isinstance(exc, core.QuintTypeError):
        return QuintTypeException(str(exc))
    if isinstance(exc, core.QuintSyntaxError):
        return QuintSyntaxException(str(exc))
    if isinstance(exc, core.QuintIndexError):
        return QuintIndexException(str(exc))
    if isinstance(exc, core.QuintAssertionError):
        return QuintAssertionException(str(exc))
    return exc


__all__ = [
    'QuintSyntaxException', 'QuintTypeException', 'QuintException',
    'QuintNameException', 'QuintRuntimeException', 'QuintAssertionException'
]
