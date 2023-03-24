import inspect


class KernelArgument:

    def __init__(self, _annotation, _name, _default=inspect.Parameter.empty):
        self.annotation = _annotation
        self.name = _name
        self.default = _default


def decl_scalar_arg(dtype):
    pass
