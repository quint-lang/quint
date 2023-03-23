class SNode:
    """SNode Python wrapper

    Arg:
        ptr (pointer): The C++ side SNode pointer
    """
    def __init__(self, ptr):
        self.ptr = ptr
