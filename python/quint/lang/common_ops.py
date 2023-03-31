import warnings

from quint.lang import ops
from quint.typedefs import primitive_types


class QuintOperations:
    __deprecated_atomic_ops__ = {
        "atomic_add": "_atomic_add",
        "atomic_and": "_atomic_and",
        "atomic_or": "_atomic_or",
        "atomic_sub": "_atomic_sub",
        "atomic_xor": "_atomic_xor",
    }

    def __getattr__(self, item):
        if item in QuintOperations.__deprecated_atomic_ops__:
            warnings.warn(
                f"a.{item}(b) is deprecated. Please use quint.{item}(a, b) instead.",
                DeprecationWarning)
            return getattr(self, QuintOperations.__deprecated_atomic_ops__[item])

        raise AttributeError(
            f"'{type(self).__name__}' object has no attribute '{item}'"
        )

    def __neg__(self):
        return ops.neg(self)

    def __abs__(self):
        return ops.abs(self)

    def __add__(self, other):
        return ops.add(self, other)

    def __radd__(self, other):
        return ops.add(other, self)

    def __sub__(self, other):
        return ops.sub(self, other)

    def __rsub__(self, other):
        return ops.sub(other, self)

    def __mul__(self, other):
        return ops.mul(self, other)

    def __rmul__(self, other):
        return ops.mul(other, self)

    def __truediv__(self, other):
        return ops.truediv(self, other)

    def __rtruediv__(self, other):
        return ops.truediv(other, self)

    def __floordiv__(self, other):
        return ops.floordiv(self, other)

    def __rfloordiv__(self, other):
        return ops.floordiv(other, self)

    def __mod__(self, other):
        return ops.mod(self, other)

    def __rmod__(self, other):
        return ops.mod(other, self)

    def __pow__(self, other, modulo=None):
        return ops.pow(self, other)

    def __rpow__(self, other, modulo=None):
        return ops.pow(other, self)

    def __le__(self, other):
        return ops.cmp_le(self, other)

    def __lt__(self, other):
        return ops.cmp_lt(self, other)

    def __ge__(self, other):
        return ops.cmp_ge(self, other)

    def __gt__(self, other):
        return ops.cmp_gt(self, other)

    def __eq__(self, other):
        return ops.cmp_eq(self, other)

    def __ne__(self, other):
        return ops.cmp_ne(self, other)

    def __and__(self, other):
        return ops.bit_and(self, other)

    def __rand__(self, other):
        return ops.bit_and(other, self)

    def __or__(self, other):
        return ops.bit_or(self, other)

    def __ror__(self, other):
        return ops.bit_or(other, self)

    def __xor__(self, other):
        return ops.bit_xor(self, other)

    def __rxor__(self, other):
        return ops.bit_xor(other, self)

    def __lshift__(self, other):
        return ops.bit_shl(self, other)

    def __rlshift__(self, other):
        return ops.bit_shl(other, self)

    def __rshift__(self, other):
        return ops.bit_sar(self, other)

    def __rrshift__(self, other):
        return ops.bit_sar(other, self)

    def __invert__(self):  # ~a => a.__invert__()
        return ops.bit_not(self)

    def __not__(self):  # not a => a.__not__()
        return ops.logical_not(self)

    def _assign(self, other):
        """Assign the expression of the given operand to self.

        Args:
            other (Any): Given operand.

        Returns:
            :class:`~taichi.lang.expr.Expr`: The expression after assigning."""
        return ops.assign(self, other)

    # pylint: disable=R0201
    def _augassign(self, x, op):
        """Generate the computing expression between self and the given operand of given operator and assigned to self.

        Args:
            x (Any): Given operand.
            op (str): The name of operator."""
        if op == 'Add':
            self += x
        elif op == 'Sub':
            self -= x
        elif op == 'Mult':
            self *= x
        elif op == 'Div':
            self /= x
        elif op == 'FloorDiv':
            self //= x
        elif op == 'Mod':
            self %= x
        elif op == 'BitAnd':
            self &= x
        elif op == 'BitOr':
            self |= x
        elif op == 'BitXor':
            self ^= x
        elif op == 'RShift':
            self >>= x
        elif op == 'LShift':
            self <<= x
        elif op == 'Pow':
            self **= x
        else:
            assert False, op

    def __ti_int__(self):
        return ops.cast(self, int)

    def __ti_bool__(self):
        return ops.cast(
            self, primitive_types.i32)  # TODO[Xiaoyan]: Use i1 in the future

    def __ti_float__(self):
        return ops.cast(self, float)
