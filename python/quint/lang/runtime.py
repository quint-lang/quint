from quint.lang.expr import Expr
from quint._lib import core as _quint_core
from quint.lang.struct import Register


class PyQuint:

    def __init__(self, kernels=None):
        self.materialized = False
        self.prog = None
        self.compiled_functions = {}
        self.src_info_stack = []
        self.inside_kernel = False
        self.current_kernel = None
        self.global_vars = []
        self.kernels = kernels or []
        self.symtable_map = {}
        self.create_program()

    def create_program(self):
        if self.prog is None:
            self.prog = _quint_core.Program()

    def add_node(self, kernel, ctx):
        self.symtable_map[kernel] = ctx

    def get_node(self, kernel):
        return self.symtable_map[kernel]

    def materialize(self):
        self.materialized = True
        self.global_vars = []

    def get_current_src_info(self):
        return self.src_info_stack[-1]

    def src_info_guard(self, info):
        return SrcInfoGuard(self.src_info_stack, info)


pyquint = PyQuint()


def get_runtime():
    return pyquint


def expr_init(rhs):
    if rhs is None:
        return Expr(get_runtime().prog.current_ast_builder().expr_alloca())
    if isinstance(rhs, list):
        return [expr_init(e) for e in rhs]
    if isinstance(rhs, tuple):
        return tuple(expr_init(e) for e in rhs)
    if isinstance(rhs, dict):
        return dict((key, expr_init(val)) for key, val in rhs.items())
    if isinstance(rhs, Register):
        return rhs
    return Expr(get_runtime().prog.current_ast_builder().expr_var(
        Expr(rhs).ptr,
        get_runtime().get_current_src_info()
    ))


def begin_frontend_if(ast_builder, cond):
    assert ast_builder is not None
    ast_builder.begin_frontend_if(Expr(cond).ptr)


def quint_format_list_to_content_entries(raw):
    def entry2content(_var):
        if isinstance(_var, str):
            return _var
        return Expr(_var).ptr

    def list_quint_repr(_var):
        yield '['
        for i, v in enumerate(_var):
            if i:
                yield ', '
            yield v
        yield ']'

    def vars2entries(_vars):
        for _var in _vars:
            if hasattr(_var, '__quint_repr__'):
                res = _var.__quint_repr__()
            elif isinstance(_var, (list, tuple)):
                if len(_var) > 0 and isinstance(
                    _var[0], str) and _var[0] == '__quint_format__':
                    res = _var[1:]
                else:
                    res = list_quint_repr(_var)
            else:
                yield _var
                continue

            for v in vars2entries(res):
                yield v

    def fused_string(entries):
        accumated = ''
        for entry in entries:
            if isinstance(entry, str):
                accumated += entry
            else:
                if accumated:
                    yield accumated
                    accumated = ''
                yield entry
        if accumated:
            yield accumated

    entries = vars2entries(raw)
    entries = fused_string(entries)
    return [entry2content(entry) for entry in entries]


class SrcInfoGuard:
    def __init__(self, info_stack, info):
        self.info_stack = info_stack
        self.info = info

    def __enter__(self):
        self.info_stack.append(self.info)

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.info_stack.pop()


def quint_print(*_vars, sep=' ', end='\n'):
    def add_separators(_vars):
        for i, _var in enumerate(_vars):
            if i:
                yield sep
            yield _var
        yield end

    _vars = add_separators(_vars)
    entries = quint_format_list_to_content_entries(_vars)
    get_runtime().prog.current_ast_builder().create_print(entries)


def quint_format(*args, **kwargs):
    content = args[0]
    mixed = args[1:]
    new_mixed = []
    new_mixed_kwargs = {}
    args = []
    for x in mixed:
        if isinstance(x, Expr):
            new_mixed.append('{}')
            args.append(x)
        else:
            new_mixed.append(x)
    for k, v in kwargs.items():
        if isinstance(v, Expr):
            new_mixed_kwargs[k] = '{}'
            args.append(v)
        else:
            new_mixed_kwargs[k] = v
    try:
        content = content.format(*new_mixed, **new_mixed_kwargs)
    except ValueError:
        print("Number formatting is not supported with Quint")
        exit(1)
    res = content.split('{}')
    assert len(res) == len(args) + 1, 'Number of args is different from number of positions provided in string'

    for i, arg in enumerate(args):
        res.insert(i * 2 + 1, arg)
    res.insert(0, '__quint_format__')
    return res


def quint_assert(cond, msg, extra_args):
    get_runtime().prog.current_ast_builder().create_assert_stmt(
        Expr(cond).ptr, msg, extra_args
    )
