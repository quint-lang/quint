import inspect

from quint.lang.util import cook_type
from quint.lang.runtime import get_runtime
from quint.lang.expr import Expr
from quint._lib import core


class KernelArgument:

    def __init__(self, _annotation, _name, _default=inspect.Parameter.empty):
        self.annotation = _annotation
        self.name = _name
        self.default = _default


def decl_scalar_arg(dtype):
    dtype = cook_type(dtype)
    arg_id = get_runtime().prog.decl_scalar_arg(dtype)
    return Expr(core.make_arg_load_expr(arg_id, dtype, False))
