from quint._lib import core

float16 = core.DataType_f16

f16 = float16

float32 = core.DataType_f32

f32 = float32

float64 = core.DataType_f64

f64 = float64

int8 = core.DataType_i8

i8 = int8

int16 = core.DataType_i16

i16 = int16

int32 = core.DataType_i32

i32 = int32

int64 = core.DataType_i64

i64 = int64

uint8 = core.DataType_u8

u8 = uint8

uint16 = core.DataType_u16

u16 = uint16

uint32 = core.DataType_u32

u32 = uint32

uint64 = core.DataType_u64

u64 = uint64

real_types = [f16, f32, f64, float]
real_types_ids = [id(t) for t in real_types]

integer_types = [i8, i16, i32, i64, u8, u16, u32, u64, int, bool]
integer_type_ids = [id(t) for t in integer_types]

all_types = real_types + integer_types
type_ids = [id(t) for t in all_types]

__all__ = [
    'float32',
    'f32',
    'float64',
    'f64',
    'float16',
    'f16',
    'int8',
    'i8',
    'int16',
    'i16',
    'int32',
    'i32',
    'int64',
    'i64',
    'uint8',
    'u8',
    'uint16',
    'u16',
    'uint32',
    'u32',
    'uint64',
    'u64',
]
