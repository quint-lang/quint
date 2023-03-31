import ast
import collections.abc
import warnings
from collections import ChainMap
from sys import version_info

from quint.utils.exceptions import QuintException, QuintSyntaxException
from quint.lang.ast.ast_transformer_utils import Visitor
from quint.lang.struct import Register
from quint.lang.qtype import RegisterType, QIntType, QInt, QBool
from quint.lang.runtime import expr_init
from quint.lang.ops import Operation, cast
from quint.lang.kernel_argument import decl_scalar_arg
from quint.typedefs import primitive_types
from quint.lang import expr
from quint.lang import ops as quint_ops
from quint.lang import runtime

if version_info < (3, 9):
    from astunparse import unparse
else:
    from ast import unparse


class Pair:

    def __init__(self, first, second):
        self.first = first
        self.second = second


class TransformVisitor(Visitor):

    @staticmethod
    def visit_Name(ctx, node):
        node.ptr = ctx.get_var_by_name(node.id)
        return node.ptr

    @staticmethod
    def visit_FunctionDef(ctx, node):
        args = node.args
        assert args.vararg is None
        assert args.kwonlyargs == []
        assert args.kw_defaults == []
        assert args.kwarg is None

        # handler argument
        for i, arg in enumerate(args.args):
            ctx.kernel_args.append(arg.arg)
            if isinstance(ctx.func_arguments[i].annotation, RegisterType):
                tg = ctx.func_arguments[i].annotation.make()
                ctx.global_reg[arg.arg] = tg
                ctx.create_variable(arg.arg, tg)
            else:
                ctx.create_variable(arg.arg, decl_scalar_arg(ctx.func_arguments[i].annotation))
        # remove original args
        node.args.args = []

        with ctx.variable_scope_guard():
            build_stmts(ctx, node.body)

    @staticmethod
    def visit_Module(ctx, node):
        with ctx.variable_scope_guard():
            for stmt in node.body:
                build_stmt.visit(ctx, stmt)

    @staticmethod
    def visit_Assign(ctx, node):
        build_stmt.visit(ctx, node.value)
        values = expr_init(node.value.ptr)

        for node_target in node.targets:
            TransformVisitor.build_assign_unpack(ctx, node_target, values)

        return None

    @staticmethod
    def visit_Call(ctx, node):
        build_stmt.visit(ctx, node.func)
        build_stmts(ctx, node.args)
        build_stmts(ctx, node.keywords)

        func = node.func.ptr

        args = []
        for arg in node.args:
            if isinstance(arg, ast.Starred):
                arg_list = arg.ptr
                for i in arg_list:
                    args.append(i)
            else:
                args.append(arg.ptr)

        keywords = dict(ChainMap(*[{keyword.ptr.first: keyword.ptr.second} for keyword in node.keywords]))
        if isinstance(func, RegisterType):
            if isinstance(func, QIntType):
                node.ptr = QInt(*args, **keywords)
                node.ptr.local = True
                return node.ptr

        if TransformVisitor.build_call_if_is_builtin(ctx, node, args, keywords):
            return node.ptr

        if hasattr(node.func, 'caller'):
            node.ptr = func(node.func.caller, *args, **keywords)
            return node.ptr

        # todo handler quantum type
        # TransformVisitor.warn_if_is_external_func(ctx, node)

        node.ptr = func(*args, **keywords)
        return node.ptr

    @staticmethod
    def visit_If(ctx, node):
        build_stmt.visit(ctx, node.test)

        control_flag = False
        if isinstance(node.test.ptr, Register):
            control_flag = True
            ctx.local_reg[node.test.ptr.name] = node.test.ptr

        # if False:
        #     if node.test.ptr:
        #         build_stmts(ctx, node.body)
        #     else:
        #         build_stmts(ctx, node.orelse)

        with ctx.if_guard(node):
            if control_flag:
                ctx.enter_if_scope(node.test.ptr)
                build_stmts(ctx, node.body)
                ctx.exit_if_scope()
                if len(node.orelse) > 0:
                    ctx.append_op(Operation("FlipBool", node.test.ptr, [], []))
                    ctx.enter_if_scope(node.test.ptr)
                    build_stmts(ctx, node.orelse)
                    ctx.exit_if_scope()
            else:
                runtime.begin_frontend_if(ctx.ast_builder, node.test.ptr)
                ctx.ast_builder.begin_frontend_if_true()
                build_stmts(ctx, node.body)
                ctx.ast_builder.pop_scope()
                ctx.ast_builder.begin_frontend_if_false()
                build_stmts(ctx, node.orelse)
                ctx.ast_builder.pop_scope()
        return None

    @staticmethod
    def visit_AugAssign(ctx, node):
        build_stmt.visit(ctx, node.target)
        build_stmt.visit(ctx, node.value)
        if isinstance(node.target.ptr, Register):
            op = Operation(type(node.op).__name__, node.target.ptr, [], [])
            if isinstance(node.value.ptr, Register):
                op.ancs = [node.value.ptr]
            else:
                op.constants = [node.value.ptr]
            ctx.append_op(op)
            node.ptr = node.target.ptr
        else:
            # todo handler No Register AugAssign
            node.ptr = None
        return node.ptr

    @staticmethod
    def visit_Constant(ctx, node):
        node.ptr = node.value
        return node.ptr

    @staticmethod
    def visit_keyword(ctx, node):
        build_stmt.visit(ctx, node.value)
        if node.arg is None:
            node.ptr = node.value.ptr
        else:
            node.ptr = Pair(node.arg, node.value.ptr)
        return node.ptr

    @staticmethod
    def build_call_if_is_builtin(ctx, node, args, keywords):
        func = node.func.ptr
        replace_func = {
            id(print): runtime.quint_print,
            id(min): min,
            id(max): max,
            id(int): int,
            id(bool): bool,
            id(float): float,
            id(any): any,
            id(abs): abs,
            id(pow): pow,
            id(range): range,
            id(len): len
        }
        # Builtin 'len' function on Matrix Expr
        if id(func) in replace_func:
            node.ptr = replace_func[id(func)](*args, **keywords)
            if func is min or func is max:
                name = "min" if func is min else "max"
                warnings.warn_explicit(
                    f'Calling builtin function "{name}" in Quint scope is deprecated. '
                    f'Please use "quint.{name}" instead.',
                    DeprecationWarning,
                    ctx.file,
                    node.lineno + ctx.lineno_offset,
                    module="quint")
            return True
        return False

    @staticmethod
    def build_call_if_is_type(ctx, node, args, keywords):
        func = node.func.ptr
        if id(func) in primitive_types.type_ids:
            if len(args) != 1 or keywords:
                raise QuintSyntaxException("A primitive type can only decorate a single expression")
            # todo handler quantum type

            if isinstance(args[0], expr.Expr):
                node.ptr = cast(args[0], func)
            else:
                node.ptr = expr.Expr(args[0], dtype=func)
            return True
        return False

    @staticmethod
    def warn_if_is_external_func(ctx, node):
        func = node.func.ptr
        if hasattr(func, "_is_quint_function") or hasattr(
                func, "_is_wrapped_kernel"):
            return
        if hasattr(
            func, "__module__"
        ) and func.__module__ and func.__module__.startwith("quint."):
            return
        name = unparse(node.func).strip()
        warnings.warn_explicit(
            f'Calling non-quint function "{name}". '
            f'Scope inside the function is not processed by the Quint AST transformer. '
            f'The function may not work as expected. Proceed with caution! '
            f'Maybe you can consider turning it into a @quint.func?',
            UserWarning,
            ctx.file,
            node.lineno + ctx.lineno_offset,
            module="quint"
        )

    @staticmethod
    def build_assign_unpack(ctx, node_target, values):
        if not isinstance(node_target, ast.Tuple):
            return TransformVisitor.build_assign_basic(ctx, node_target, values)

        targets = node_target.elts

        if not isinstance(values, collections.abc.Sequence):
            raise QuintException(f'Cannot unpack type: {type(values)}')

        if len(values) != len(targets):
            raise QuintException("The number of targets is not equal to value length")

        for i, target in enumerate(targets):
            TransformVisitor.build_assign_basic(ctx, target, values[i])

    @staticmethod
    def build_assign_basic(ctx, target, value):
        is_local = isinstance(target, ast.Name)
        # if is_local and target.id in ctx.kernel_args:
        #     raise QuintException(f"Kernel argument \"{target.id}\" is immutable in the kernel. "
        #         f"If you want to change its value, please create a new variable.")
        if is_local and not ctx.is_var_declared(target.id):
            var = expr_init(value)
            if isinstance(var, QInt):
                if var.state.value != 0:
                    ctx.append_op(Operation("Assign", var, [], [var.state.value]))
                ctx.local_reg[target.id] = var
            ctx.create_variable(target.id, value)
        else:
            var = build_stmt.visit(ctx, target)
            try:
                var._assign(value)
            except AttributeError:
                raise QuintSyntaxException(
                    f"Variable '{unparse(target).strip()}' cannot be assigned. May be it is not a Quint Support type"
                )
        return var

    @staticmethod
    def visit_Compare(ctx, node):
        build_stmt.visit(ctx, node.left)
        build_stmts(ctx, node.comparators)
        ops = {
            ast.Eq: Operation("CompareEqual", None, [], []),
            ast.NotEq: Operation("CompareNotEqual", None, [], []),
            ast.Lt: Operation("CompareLess", None, [], []),
            ast.Gt: Operation("CompareGreater", None, [], []),
        }

        special_ops = {
            ast.LtE: [Operation("CompareLess", None, [], []), Operation("CompareEqual", None, [], []),
                      Operation("Or", None, [], [])],
            ast.GtE: [Operation("CompareGreater", None, [], []), Operation("CompareEqual", None, [], []),
                      Operation("Or", None, [], [])]
        }

        ops_normal = {
            ast.Eq: lambda l, r: l == r,
            ast.NotEq: lambda l, r: l != r,
            ast.Lt: lambda l, r: l < r,
            ast.LtE: lambda l, r: l <= r,
            ast.Gt: lambda l, r: l > r,
            ast.GtE: lambda l, r: l >= r,
        }

        ops_static = {
            ast.In: lambda l, r: l in r,
            ast.NotIn: lambda l, r: l not in r,
            ast.Is: lambda l, r: l is r,
            ast.IsNot: lambda l, r: l is not r,
        }

        operands = [node.left.ptr] + [comparator.ptr for comparator in node.comparators]
        val = True
        for i, node_op in enumerate(node.ops):
            left = operands[i]
            right = operands[i + 1]
            if isinstance(node_op, (ast.Is, ast.IsNot)):
                name = "is" if isinstance(node_op, ast.Is) else "is not"
                warnings.warn_explicit(
                    f'Operator "{name}" in Quint scope is deprecated. Please avoid using it.',
                    DeprecationWarning,
                    ctx.file,
                    node.lineo + ctx.lineno_offset,
                    module="quint"
                )
            if isinstance(left, Register):
                val = QBool()
                val.local = True
                if special_ops.get(type(node_op)) is not None:
                    op_list = special_ops.get(type(node_op))
                    bool_list = []
                    if isinstance(right, Register):
                        for index in range(len(op_list) - 1):
                            temp = QBool()
                            op_list[index].target = temp
                            op_list[index].ancs = [left, right]
                            bool_list.append(temp)
                    else:
                        for index in range(len(op_list) - 1):
                            temp = QBool()
                            op_list[index].target = temp
                            op_list[index].ancs = [left]
                            op_list[index].constants = [right]
                            bool_list.append(temp)
                    op_list[-1].target = val
                    op_list[-1].ancs = bool_list
                    for item in op_list:
                        ctx.append_op(item)
                elif ops.get(type(node_op)) is not None:
                    op = ops.get(type(node_op))
                    op.target = val
                    if isinstance(right, Register):
                        op.ancs = [left, right]
                    else:
                        op.ancs = [left]
                        op.constants = [right]
                    ctx.append_op(op)
                else:
                    raise QuintException("Unsupported operation in Quint")
            else:
                op = ops_normal.get(type(node_op))
                if op is None:
                    if type(node_op) in ops_static:
                        raise QuintSyntaxException(
                            f'"{type(node_op).__name__}" is only supported inside `quint.static`'
                        )
                    else:
                        raise QuintSyntaxException(
                            f'"{type(node_op).__name__}" is not supported in Quint Kernels.'
                        )
                val = quint_ops.bit_and(val, op(left, right))
        if not isinstance(val, (QBool, bool)):
            val = quint_ops.cast(val, primitive_types.i32)
        node.ptr = val
        return node.ptr

    @staticmethod
    def visit_List(ctx, node):
        build_stmt.visit(ctx, node.elts)
        node.ptr = [elt.ptr for elt in node.elts]
        return node.ptr

    @staticmethod
    def visit_Num(ctx, node):
        node.ptr = node.n
        return node.ptr

    @staticmethod
    def visit_Str(ctx, node):
        node.ptr = node.s
        return node.ptr

    @staticmethod
    def visit_Bytes(ctx, node):
        node.ptr = node.s
        return node.ptr

    @staticmethod
    def visit_NameConstant(ctx, node):
        node.ptr = node.value
        return node.ptr

    @staticmethod
    def visit_Expr(ctx, node):
        build_stmt.visit(ctx, node.value)
        return None


build_stmt = TransformVisitor()


def build_stmts(ctx, stmts):
    with ctx.variable_scope_guard():
        for stmt in stmts:
            build_stmt.visit(ctx, stmt)
    return stmts
