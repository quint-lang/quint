from quint.lang.qtype import QInt, QIntType
from quint.lang.kernel import quantum


@quantum
def show(target: QIntType(32)):
    a = QInt(32, 8)
    b = QInt(32, 8)
    if a > b:
        a -= b
    else:
        a += b
    target += a


# print(show)
show.show_dag("base")
show.uncompute()
