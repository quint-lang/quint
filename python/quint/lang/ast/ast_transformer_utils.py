import ast
import builtins
from enum import Enum

from quint.utils.exceptions import QuintException
from sys import version_info
from textwrap import TextWrapper
from quint.lang.qtype import QIntType


class SrcInfoGuard:

    def __init__(self, info_stack, info):
        self.info_stack = info_stack
        self.info = info

    def __enter__(self):
        self.info_stack.append(self.info)

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.info_stack.pop()


class ControlFlowStatus:

    def __init__(self):
        self.control_flow = False


class ControlFlowGuard:

    def __init__(self, status: ControlFlowStatus):
        self.status = status

    def __enter__(self):
        self.prev = self.status.control_flow
        self.status.control_flow = True

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.status.control_flow = self.prev


class LoopStatus(Enum):
    Normal = 0
    Break = 1
    Continue = 2


class LoopScopeAttribute:

    def __init__(self):
        self.status = LoopStatus.Normal
        self.nearest = None


class VariableScopeGuard:

    def __init__(self, scopes):
        self.scopes = scopes

    def __enter__(self):
        self.scopes.append({})

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.scopes.pop()


class IfGuard:

    def __init__(self, if_node: ast.If, loop_attribute: LoopScopeAttribute,
                 status: ControlFlowStatus):
        self.loop_attribute = loop_attribute
        self.if_node = if_node
        self.guard = ControlFlowGuard(status)

    def __enter__(self):
        if self.loop_attribute:
            self.old_if = self.loop_attribute.nearest
            self.loop_attribute.nearest = self.if_node
        self.guard.__enter__()

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.loop_attribute:
            self.loop_attribute.nearest = self.old_if
        self.guard.__exit__(exc_type, exc_val, exc_tb)


class Visitor:

    def visit(self, ctx, node):
        method = getattr(self, 'visit_' + node.__class__.__name__, None)
        try:
            if method is None:
                error_msg = f'Unsupported node "{node.__class__.__name__}"'
                raise QuintException(error_msg)
            info = ctx.get_pos_info(node) if isinstance(node, (ast.stmt, ast.expr)) else ""
            with ctx.src_info_guard(info):
                return method(ctx, node)
        except Exception:
            raise QuintException("Compiler quint kernel error")


class ASTTransformerContext:

    def __init__(self,
                 func=None,
                 file=None,
                 src=None,
                 start_lineno=None,
                 ast_builder=None,
                 global_vars=[]):
        self.func = func
        self.local_scopes = []
        self.loop_scopes = []
        self.file = file
        self.src = src
        self.ident = 0
        for c in self.src[0]:
            if c == ' ':
                self.ident += 1
            else:
                break
        self.lineno_offset = start_lineno - 1
        self.visited_funcdef = False
        self.ast_builder = ast_builder
        self.func_arguments = []
        self.kernel_args = []
        self.src_info_stack = []
        self.ops = []
        self.local_reg = {}
        self.global_reg = {}
        self.regs = {}
        self.global_vars = global_vars
        self.control_flow_status = ControlFlowStatus()
        self.control_scope = []

    def src_info_guard(self, info):
        return SrcInfoGuard(self.src_info_stack, info)

    def variable_scope_guard(self):
        return VariableScopeGuard(self.local_scopes)

    def if_guard(self, if_node: ast.If):
        return IfGuard(if_node,
                       self.current_loop_scope() if len(self.loop_scopes) > 0 else None,
                       self.control_flow_status)

    def enter_if_scope(self, flag):
        control_list = [flag]
        if len(self.control_scope) != 0:
            control_list.extend(self.current_control_scope())
        self.control_scope.append(control_list)

    def exit_if_scope(self):
        self.control_scope.pop()

    def current_scope(self):
        return self.local_scopes[-1]

    def current_loop_scope(self):
        return self.loop_scopes[-1]

    def current_control_scope(self):
        return self.control_scope[-1]

    def is_var_declared(self, name):
        for s in self.local_scopes:
            if name in s:
                return True
            return False

    def create_variable(self, name, var):
        if name in self.current_scope():
            raise QuintException("Recreating variables is not allowed")
        self.current_scope()[name] = var

    def append_op(self, op):
        if len(self.control_scope) > 0:
            op.add_all_controls(self.current_control_scope())
        self.ops.append(op)

    def get_var_by_name(self, name):
        for s in reversed(self.local_scopes):
            if name in s:
                return s[name]
        if name in self.global_vars:
            return self.global_vars[name]
        try:
            if name == 'QInt':
                return QIntType(0)
            return getattr(builtins, name)
        except AttributeError:
            raise QuintException(f'Name "{name}" is not defined')

    def get_pos_info(self, node):
        msg = f'File "{self.file}", line {node.lineno + self.lineno_offset}, in {self.func.__name__}:\n'
        if version_info < (3, 8):
            msg += self.src[node.lineno - 1] + "\n"
            return msg
        col_offset = self.ident + node.col_offset
        end_col_offset = self.ident + node.end_col_offset

        wrapper = TextWrapper(width=80)

        def gen_line(code, hint):
            hint += ' ' * (len(code) - len(hint))
            code = wrapper.wrap(code)
            hint = wrapper.wrap(hint)
            if not len(code):
                return "\n\n"
            return "".join([c + '\n' + h + '\n' for c, h in zip(code, hint)])

        if node.lineno == node.end_lineno:
            hint = ' ' * col_offset + '^' * (end_col_offset - col_offset)
            msg += gen_line(self.src[node.lineno - 1], hint)
        else:
            node_type = node.__class__.__name__

            if node_type in ["For", "While", "FunctionDef", "If"]:
                end_lineno = max(node.body[0].lineno - 1, node.lineno)
            else:
                end_lineno = node.end_lineno

            for i in range(node.lineno - 1, end_lineno):
                last = len(self.src[i])
                while last > 0 and (self.src[i][last - 1].isspace() or
                                    not self.src[i][last - 1].isprintable()):
                    last -= 1
                first = 0
                while first < len(self.src[i]) and (
                        self.src[i][first].isspace()
                        or not self.src[i][first].isprintable()):
                    first += 1
                if i == node.lineno - 1:
                    hint = ' ' * col_offset + '^' * (last - col_offset)
                elif i == node.end_lineno - 1:
                    hint = ' ' * first + '^' * (end_col_offset - first)
                elif first < last:
                    hint = ' ' * first + '^' * (last - first)
                else:
                    hint = ''
                msg += gen_line(self.src[i], hint)
        return msg
