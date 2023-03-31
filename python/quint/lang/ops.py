import builtins
import functools
import math
import operator as bt_ops_mod

from quint.lang.struct import Register
from quint.utils.exceptions import QuintException
from quint.lang.util import cook_type
from quint.lang import expr
from quint._lib import core
from quint.lang.util import is_quint_class
from quint.lang import runtime


class Operation:

    def __init__(self, name: str, target: Register, ancs, constants):
        try:
            name = str(name)
        except Exception:
            raise QuintException("Operation Name must be str")

        self.name = name
        self.target = target
        self.ancs = ancs
        self.controls = []
        self.constants = constants

    def add_control(self, reg):
        self.controls.append(reg)

    def add_all_controls(self, regs):
        self.controls.extend(regs)

    def __str__(self):
        ptr = "{ " + self.name + ", target: " + self.target.name + ", ancs: "
        for i in self.ancs:
            ptr += i.name + ", "
        ptr += "controls: "
        for i in self.controls:
            ptr += i.name + ", "
        ptr += "constants: "
        for i in self.constants:
            ptr += str(i) + ", "
        ptr += " }"
        return ptr


def stack_info():
    return runtime.get_runtime().get_current_src_info()


def is_quint_expr(a):
    return isinstance(a, expr.Expr)


def wrap_if_not_expr(a):
    return expr.Expr(a) if not is_quint_expr(a) else a


def unary(foo):
    @functools.wraps(foo)
    def imp_foo(x):
        return foo(x)

    @functools.wraps(foo)
    def wrapped(a):
        if is_quint_class(a):
            return a._element_wise_unary(imp_foo)
        return imp_foo(a)

    return wrapped


binary_ops = []


def binary(foo):
    @functools.wraps(foo)
    def imp_foo(x, y):
        return foo(x, y)

    @functools.wraps(foo)
    def rev_foo(x, y):
        return foo(y, x)

    @functools.wraps(foo)
    def wrapped(a, b):
        # todo handler quantum type
        return imp_foo(a, b)

    binary_ops.append(wrapped)
    return wrapped


ternary_ops = []


def ternary(foo):
    @functools.wraps(foo)
    def abc_foo(a, b, c):
        return foo(a, b, c)

    @functools.wraps(foo)
    def bac_foo(b, a, c):
        return foo(a, b, c)

    @functools.wraps(foo)
    def cab_foo(c, a, b):
        return foo(a, b, c)

    @functools.wraps(foo)
    def wrapped(a, b, c):
        # todo handler quantum type
        return abc_foo(a, b, c)

    ternary_ops.append(wrapped)
    return wrapped


def cast(obj, dt):
    dt = cook_type(dt)
    # todo handler quantum type
    return expr.Expr(core.value_cast(expr.Expr(obj).ptr, dt))


def unary_operation(quint_op, python_op, a):
    if is_quint_expr(a):
        return expr.Expr(quint_op(a.ptr), tb=stack_info())
    return python_op(a)


def binary_operation(quint_op, python_op, a, b):
    if is_quint_expr(a) or is_quint_expr(b):
        a, b = wrap_if_not_expr(a), wrap_if_not_expr(b)
        return expr.Expr(quint_op(a.ptr, b.ptr), tb=stack_info())
    return python_op(a, b)


def ternary_operation(quint_op, python_op, a, b, c):
    if is_quint_expr(a) or is_quint_expr(b) or is_quint_expr(c):
        a, b, c = wrap_if_not_expr(a), wrap_if_not_expr(b), wrap_if_not_expr(c)
        return expr.Expr(quint_op(a.ptr, b.ptr, c.ptr), tb=stack_info())
    return python_op(a, b, c)


@unary
def neg(x):
    return unary_operation(core.expr_neg, bt_ops_mod.neg, x)


@unary
def sin(x):
    return unary_operation(core.expr_sin, math.sin, x)


@unary
def cos(x):
    return unary_operation(core.expr_cos, math.cos, x)


@unary
def asin(x):
    return unary_operation(core.expr_asin, math.asin, x)


@unary
def acos(x):
    return unary_operation(core.expr_acos, math.acos, x)


@unary
def sqrt(x):
    return unary_operation(core.expr_sqrt, math.sqrt, x)


@unary
def rsqrt(x):
    def _rsqrt(x):
        return 1 / math.sqrt(x)

    return unary_operation(core.expr_rsqrt, _rsqrt, x)


@unary
def _round(x):
    return unary_operation(core.expr_round, builtins.round, x)


def round(x, dtype=None):  # pylint: disable=redefined-builtin
    result = _round(x)
    if dtype is not None:
        result = cast(result, dtype)
    return result


@unary
def _floor(x):
    return unary_operation(core.expr_floor, math.floor, x)


def floor(x, dt=None):
    result = _floor(x)
    if dt is not None:
        result = cast(result, dt)
    return result


@unary
def _ceil(x):
    return unary_operation(core.expr_ceil, math.ceil, x)


def ceil(x, dt=None):
    result = _ceil(x)
    if dt is not None:
        result = cast(result, dt)
    return result


@unary
def tan(x):
    return unary_operation(core.expr_tan, math.tan, x)


@unary
def tanh(x):
    return unary_operation(core.expr_tanh, math.tanh, x)


@unary
def exp(x):
    return unary_operation(core.expr_exp, math.exp, x)


@unary
def log(x):
    return unary_operation(core.expr_log, math.log, x)


@unary
def abs(x):
    return unary_operation(core.expr_abs, builtins.abs, x)


@unary
def bit_not(a):
    return unary_operation(core.expr_bit_not, bt_ops_mod.invert, a)


@unary
def logical_not(a):
    return unary_operation(core.expr_logical_not, lambda x: int(not x), a)


def random(dt=float):
    dt = cook_type(dt)
    x = expr.Expr(core.make_rand_expr(dt))
    return runtime.expr_init(x)


@binary
def add(a, b):
    return binary_operation(core.expr_add, bt_ops_mod.add, a, b)


@binary
def sub(a, b):
    return binary_operation(core.expr_sub, bt_ops_mod.sub, a, b)


@binary
def mul(a, b):
    return binary_operation(core.expr_mul, bt_ops_mod.mul, a, b)


@binary
def mod(x1, x2):
    def expr_python_mod(a, b):
        quotient = expr.Expr(core.expr_floordiv(a, b))
        multiply = expr.Expr(core.expr_mul(b, quotient.ptr))
        return core.expr_sub(a, multiply.ptr)

    return binary_operation(expr_python_mod, bt_ops_mod, x1, x2)


@binary
def pow(base, exponent):
    return binary_operation(core.expr_pow, bt_ops_mod.pow, base, exponent)


@binary
def floordiv(a, b):
    return binary_operation(core.expr_floordiv, bt_ops_mod.floordiv, a, b)


@binary
def truediv(a, b):
    return binary_operation(core.expr_truediv, bt_ops_mod.truediv, a, b)


@binary
def max_impl(a, b):
    return binary_operation(core.expr_max, builtins.max, a, b)


@binary
def min_impl(a, b):
    return binary_operation(core.expr_min, builtins.min, a, b)


@binary
def atan2(x1, x2):
    return binary_operation(core.expr_atan2, math.atan2, x1, x2)


@binary
def raw_div(x1, x2):
    def c_div(a, b):
        if isinstance(a, int) and isinstance(b, int):
            return a // b
        return a / b

    return binary_operation(core.expr_div, c_div, x1, x2)


@binary
def raw_mod(x1, x2):
    def c_mod(x, y):
        return x - y * int(float(x) / y)

    return binary_operation(core.expr_mod, c_mod, x1, x2)


@binary
def cmp_lt(a, b):
    return binary_operation(core.expr_cmp_lt, lambda a, b: int(a < b), a, b)


@binary
def cmp_le(a, b):
    return binary_operation(core.expr_cmp_le, lambda a, b: int(a <= b), a, b)


@binary
def cmp_gt(a, b):
    return binary_operation(core.expr_cmp_gt, lambda a, b: int(a > b), a, b)


@binary
def cmp_ge(a, b):
    return binary_operation(core.expr_cmp_ge, lambda a, b: int(a >= b), a, b)


@binary
def cmp_eq(a, b):
    return binary_operation(core.expr_cmp_eq, lambda a, b: int(a == b), a, b)


@binary
def cmp_ne(a, b):
    return binary_operation(core.expr_cmp_ne, lambda a, b: int(a != b), a, b)


@binary
def bit_or(a, b):
    return binary_operation(core.expr_bit_or, bt_ops_mod.or_, a, b)


@binary
def bit_and(a, b):
    return binary_operation(core.expr_bit_and, bt_ops_mod.and_, a, b)


@binary
def bit_xor(a, b):
    return binary_operation(core.expr_bit_xor, bt_ops_mod.xor, a, b)


@binary
def bit_shl(a, b):
    return binary_operation(core.expr_bit_shl, bt_ops_mod.lshift, a, b)


@binary
def bit_sar(a, b):
    return binary_operation(core.expr_bit_sar, bt_ops_mod.rshift, a, b)


@binary
def bit_shr(x1, x2):
    return binary_operation(core.expr_bit_shr, bt_ops_mod.rshift, x1, x2)


@binary
def logical_and(a, b):
    return binary_operation(core.expr_logical_and, lambda a, b: a and b, a, b)


@binary
def logical_or(a, b):
    return binary_operation(core.expr_logical_or, lambda a, b: a or b, a, b)


@ternary
def select(cond, x1, x2):
    cond = logical_not(logical_not(cond))

    def py_select(cond, x1, x2):
        return x1 * cond + x2 * (1 - cond)

    return ternary_operation(core.expr_select, py_select, cond, x1, x2)


@ternary
def ifte(cond, x1, x2):
    cond = logical_not(logical_not(cond))

    def py_ifte(cond, x1, x2):
        return x1 if cond else x2

    return ternary_operation(core.expr_ifte, py_ifte, cond, x1, x2)


def assign(a, b):
    runtime.get_runtime().prog.current_ast_builder().expr_assign(
        a.ptr, b.ptr, stack_info()
    )
    return a
