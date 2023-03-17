from quint.lang.expr import Expr


class PyQuint:

    def __init__(self):
        self.symtable_map = {}

    def add_node(self, kernel, ctx):
        self.symtable_map[kernel] = ctx

    def get_node(self, kernel):
        return self.symtable_map[kernel]


pyquint = PyQuint()


def get_runtime():
    return pyquint


def expr_init(rhs):
    if rhs is None:
        return Expr()
    if isinstance(rhs, list):
        return [expr_init(e) for e in rhs]
    if isinstance(rhs, tuple):
        return tuple(expr_init(e) for e in rhs)
    if isinstance(rhs, dict):
        return dict((key, expr_init(val)) for key, val in rhs.items())
    return rhs
