from quint.lang.expr import Expr
from quint._lib import core as _quint_core


class PyQuint:

    def __init__(self, kernels=None):
        self.materialized = False
        self.prog = None
        self.compiled_functions = {}
        self.src_info_stack = []
        self.inside_kernel = False
        self.current_kernel = None
        self.global_vars = []
        self.kernels = kernels or []
        self.symtable_map = {}
        self.create_program()

    def create_program(self):
        if self.prog is None:
            self.prog = _quint_core.Program()

    def add_node(self, kernel, ctx):
        self.symtable_map[kernel] = ctx

    def get_node(self, kernel):
        return self.symtable_map[kernel]

    def materialize(self):
        self.materialized = True
        self.global_vars = []


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
