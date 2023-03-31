from quint._lib import core
import numpy as np
from quint.utils.exceptions import QuintTypeException
from quint.lang.util import to_numpy_type
from quint.typedefs import primitive_types
from quint.lang import runtime
from quint.lang.common_ops import QuintOperations


class Expr(QuintOperations):

    def __init__(self, *args, tb=None, dtype=None):
        self.tb = tb
        if len(args) == 1:
            if isinstance(args[0], core.Expr):
                self.ptr = args[0]
            elif isinstance(args[0], Expr):
                self.ptr = args[0].ptr
                self.tb = args[0].tb
            else:
                # assume to be constant
                arg = args[0]
                if isinstance(arg, np.ndarray):
                    if arg.shape:
                        raise QuintTypeException(
                            "Only 0-dimensional numpy array can be used to initialize a scalar expression"
                        )
                    arg = arg.dtype.type(arg)
                self.ptr = make_constant_expr(arg, dtype).ptr
        else:
            assert False
        if self.tb:
            self.ptr.set_tb(self.tb)
        self.ptr.type_check(runtime.get_runtime().prog.config())


def check_in_range(npty, val):
    iif = np.iinfo(npty)
    return iif.min <= val <= iif.max


def clamp_unsigned_to_range(npty, val):
    iif = np.iinfo(npty)
    if iif.min <= val <= iif.max:
        return val
    cap = (1 << iif.bits)
    assert 0 <= val < cap
    new_val = val - cap
    return new_val


def make_constant_expr(val, dtype):
    if isinstance(val, bool):
        constant_type = primitive_types.i32
        return Expr(core.make_const_expr_int(constant_type, val))

    if isinstance(val, (float, np.floating)):
        constant_type = primitive_types.f32 if dtype is None else dtype
        if constant_type not in primitive_types.real_types:
            raise QuintTypeException(
                'Floating-point literals must be annotated with a floating-point type. For type casting, '
                'use `quint.cast`.'
            )
        return Expr(core.make_const_expr_fp(constant_type, val))

    if isinstance(val, (int, np.integer)):
        constant_type = primitive_types.i32 if dtype is None else dtype
        if constant_type not in primitive_types.integer_types:
            raise QuintTypeException(
                'Integer literals must be annotated with a integer type. For type casting, use `quint.cast`.'
            )
        if check_in_range(to_numpy_type(constant_type), val):
            return Expr(core.make_const_expr_int(constant_type, clamp_unsigned_to_range(np.int64, val)))
        if dtype is None:
            raise QuintTypeException(
                f'Integer literal {val} exceeded the range of default_ip'
            )
        else:
            raise QuintTypeException(
                f'Integer literal {val} exceeded the range of specified dtype: {dtype}'
            )
    raise QuintTypeException(f'Invalid constant scalar data type: {type(val)}')
