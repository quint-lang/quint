from quint.lang.struct import Register
from quint.utils.exceptions import QuintException


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
