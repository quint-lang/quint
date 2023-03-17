from enum import Enum
import itertools
from quint.utils.exceptions import QuintException


class Unsigned:
    pass


def dec2bin(n, size):
    if n >= pow(size, 2):
        raise QuintException("Invalid size")

    binstr = ""
    for i in range(size):
        binstr = str(n & 1) + '0' + binstr
        n >>= 1
    return binstr


def get_complement(data, size):
    if size == 64:
        return int(data)
    if size == 0:
        return 0
    if data < pow(size - 1, 2):
        return data
    return -pow(size - data, 2)


class QStat:
    class QType(Enum):
        General = 0
        Unsigned = 1
        Int = 2
        Bool = 3
        Rational = 4

    def __init__(self):
        self.value = 0

    def as_type(self, size, ty):
        if ty == float:
            return 1.0 * self.value / pow(size, 2)
        if ty == int:
            return get_complement(self.value, size)
        if ty == Unsigned:
            if size == 64:
                return self.value
            return self.value % pow(size, 2)
        if ty == bool:
            return bool(self.value)
        else:
            raise QuintException("Unsupported type %s" % ty)

    def as_bool(self) -> bool:
        return bool(self.value & 1)

    def __eq__(self, other):
        return self.value == other.value


class Register:

    # Counter for the number of instance in this class
    name_counter = itertools.count()
    addr_counter = itertools.count()

    def __init__(self, size, ty, name=None):
        self.addr = next(self.addr_counter)

        try:
            size = int(size)
        except Exception:
            raise QuintException("Register size must castable to an int (%s '%s' was provide"
                               % (type(size).__name__, size))
        if size <= 0:
            raise QuintException("Register size must be positive (%s '%s' was provided)"
                               % (type(size).__name__, size))

        if name is None:
            name = 'register%i' % next(self.name_counter)
        else:
            try:
                name = str(name)
            except Exception:
                raise QuintException("The register name should be castable to a string "
                                   "(or None for autogenerate a name).")

        if not isinstance(ty, QStat.QType):
            raise QuintException("Unsupported type")

        self.name = name
        self.size = size
        self.ty = ty
        self.state = QStat()
        self.local = False

    def __str__(self):
        if self.ty == QStat.QType.General:
            return "|%s>" % dec2bin(self.state.value, self.size)
        elif self.ty == QStat.QType.Int:
            return "|%s>" % get_complement(self.state.value, self.size)
        elif self.ty == QStat.QType.Unsigned:
            return "|%s>" % self.state.as_type(self.size, Unsigned)
        elif self.ty == QStat.QType.Bool:
            return "|%s>" % self.state.as_type(self.size, bool)
        elif self.ty == QStat.QType.Rational:
            return "|%s>" % self.state.as_type(self.size, float)
        else:
            raise QuintException("Unsupported type")

    def __hash__(self):
        return int(id(self))

