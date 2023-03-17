from quint.lang.struct import Register, QStat, Unsigned
from quint.utils.exceptions import QuintException
from typing import List


class QBool(Register):

    def __init__(self, name=None):
        super(QBool, self).__init__(1, QStat.QType.Bool, name)

    def __bool__(self):
        if self.state.value == 1:
            return True
        else:
            return False


class QInt(Register):

    def __init__(self, size, val=0, name=None):
        super().__init__(size, QStat.QType.Int, name)
        self.state.value = val

    def __iadd__(self, other):
        self.state.value += other.state.value

    def __isub__(self, other):
        self.state.value -= other.state.value

    def __gt__(self, other) -> QBool:
        if self.state.value > other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __ge__(self, other) -> QBool:
        if self.state.value >= other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __lt__(self, other):
        if self.state.value < other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __le__(self, other):
        if self.state.value <= other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __getitem__(self, item):
        return None

    def __setitem__(self, key, value):
        pass


class QFloat(Register):
    def __init__(self, size, val=0, name=""):
        super().__init__(size, QStat.QType.Rational, name)
        self.state.value = val

    def __iadd__(self, other):
        self.state.value += other.state.value

    def __isub__(self, other):
        self.state.value -= other.state.value

    def __gt__(self, other) -> QBool:
        if self.state.value > other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __ge__(self, other) -> QBool:
        if self.state.value >= other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __lt__(self, other):
        if self.state.value < other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __le__(self, other):
        if self.state.value <= other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()


class QUnsigned(Register):
    def __init__(self, size, val=0, name=""):
        super().__init__(size, QStat.QType.Unsigned, name)
        self.state.value = val

    def __iadd__(self, other):
        self.state.value += other.state.value

    def __isub__(self, other):
        self.state.value -= other.state.value

    def __gt__(self, other) -> QBool:
        if self.state.value > other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __ge__(self, other) -> QBool:
        if self.state.value >= other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __lt__(self, other):
        if self.state.value < other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()

    def __le__(self, other):
        if self.state.value <= other.state.value:
            flag = QBool()
            flag.state.value = 1
            return flag
        else:
            return QBool()


class RegisterType:

    def make(self):
        pass


class QIntType(RegisterType):

    def __init__(self, size):
        self.size = size

    def make(self):
        return QInt(self.size)
