import numpy as np
from quint._lib import core as _qu_core
from quint.lang import runtime
from quint.typedefs.primitive_types import (f16, f32, f64, i8, i16, i32, i64, u8,
                                            u16, u32, u64)


def cook_type(dtype):
    if isinstance(dtype, _qu_core.DataType):
        return dtype
    if isinstance(dtype, _qu_core.Type):
        return _qu_core.DataType(dtype)
    if dtype is float:
        return f32
    if dtype is int:
        return i32
    if dtype is bool:
        return i32
    raise ValueError(f'Invalid data type {dtype}')


def to_numpy_type(dt):
    if dt == f32:
        return np.float32
    if dt == f64:
        return np.float64
    if dt == i32:
        return np.int32
    if dt == i64:
        return np.int64
    if dt == i8:
        return np.int8
    if dt == i16:
        return np.int16
    if dt == u8:
        return np.uint8
    if dt == u16:
        return np.uint16
    if dt == u32:
        return np.uint32
    if dt == u64:
        return np.uint64
    if dt == f16:
        return np.half
    assert False


def is_quint_class(rhs):
    quint_class = False
    try:
        if rhs._is_quint_class:
            quint_class = True
    except:
        pass
    return quint_class
