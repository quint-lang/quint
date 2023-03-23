import inspect
import ast
import textwrap
import graphviz
import networkx as nx

from quint.lang.ast.ast_transformer_utils import ASTTransformerContext
from quint.lang.ast.transform_visitor import TransformVisitor
from quint.lang.kernel_argument import KernelArgument
from quint.lang.runtime import get_runtime
from quint.lang.graph import DAGNode
from quint.lang.ops import Operation
from quint._lib import core as _qu_core
from quint.utils.exceptions import QuintSyntaxException


class QuintCallableTemplateMapper:

    def __init__(self, arguments):
        self.arguments = arguments
        self.num_args = len(arguments)
        # self.template_slot_locations = template_slot_locations
        self.mapping = {}

    @staticmethod
    def extract_arg(arg, anno):
        # todo handler special type
        return "#"

    def extract(self, args):
        extracted = []
        for arg, kernel_arg in zip(args, self.arguments):
            extracted.append(self.extract_arg(arg, kernel_arg.annotation))
        return tuple(extracted)

    def lookup(self, args):
        if len(args) != self.num_args:
            raise TypeError(
                f'{self.num_args} arguments(s) needed but {len(args)} provided.'
            )

        key = self.extract(args)
        if key not in self.mapping:
            count = len(self.mapping)
            self.mapping[key] = count
        return self.mapping[key], key


def get_tree_and_ctx(function, ast_builder=None):
    file = inspect.getsourcefile(function)
    src, start_lineno = inspect.getsourcelines(function)
    src = [textwrap.fill(line, tabsize=4, width=9999) for line in src]
    tree = ast.parse(textwrap.dedent("\n".join(src)))

    func_body = tree.body[0]
    func_body.decorator_list = []

    # todo global variables
    return tree, ASTTransformerContext(func=function, file=file, src=src, start_lineno=start_lineno,
                                       ast_builder=ast_builder)


def process_args(kernel, args, kwargs):
    ret = [argument.default for argument in kernel.arguments]
    len_args = len(args)

    if len_args > len(ret):
        raise QuintSyntaxException("Too many arguments.")

    for i, arg in enumerate(args):
        ret[i] = arg

    for key, value in kwargs.items():
        found = False
        for i, arg in enumerate(kernel.arguments):
            if key == arg.name:
                if i < len_args:
                    raise QuintSyntaxException(f"Multiple values for argument '{key}'.")
                ret[i] = value
                found = True
                break
        if not found:
            raise QuintSyntaxException(f"Unexpected argument '{key}'.")

    for i, arg in enumerate(ret):
        if arg is inspect.Parameter.empty:
            raise QuintSyntaxException(f"Parameter '{kernel.arguments[i].name}' missing.")

    return ret


class quantum(object):
    counter = 0

    def __init__(self, function, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
        self.kernel_counter = quantum.counter
        quantum.counter += 1

        if not callable(function):
            self.convert_python_data_to_kernel()
        else:
            self.function = function
        self.arguments = []
        self.extract_arguments()
        self.mapper = QuintCallableTemplateMapper(self.arguments)
        self.kernel_cpp = None
        self.compiled_kernels = {}
        self.context = None
        self.compiled = False
        self.runtime = get_runtime()
        self.runtime.kernels.append(self)

    def reset(self):
        self.runtime = get_runtime()

    def compile(self, *args):
        if self.compiled:
            return
        instance_id, arg_features = self.mapper.lookup(args)
        key = (self.function, instance_id)
        self.runtime.materialize()
        if key in self.runtime.compiled_functions:
            return

        kernel_name = f"{self.function.__name__}_c{self.kernel_counter}_{key[1]}"

        tree, self.context = get_tree_and_ctx(self.function)
        self.context.func_arguments = self.arguments

        def quint_ast_generator(kernel_cxx):
            self.runtime.inside_kernel = True
            self.runtime.current_kernel = self
            try:
                self.context.ast_builder = kernel_cxx.ast_builder()
                get_runtime().add_node(self.function.__name__, self.context)
                visitor = TransformVisitor()
                visitor.visit(self.context, tree)
            finally:
                self.runtime.inside_kernel = False
                self.runtime.current_kernel = None

        quint_kernel = get_runtime().prog.create_kernel(quint_ast_generator, kernel_name)
        self.kernel_cpp = quint_kernel

        assert key not in self.runtime.compiled_functions
        self.compiled_kernels[key] = quint_kernel
        self.compiled = True

    def extract_arguments(self):
        sig = inspect.signature(self.function)
        params = sig.parameters
        arg_names = params.keys()
        for i, arg_name in enumerate(arg_names):
            param = params[arg_name]
            annotation = param.annotation
            self.arguments.append(KernelArgument(annotation, param.name, param.default))

    def __call__(self, *args, **kwargs):
        pass

    def convert_python_data_to_kernel(self):
        pass

    def show_dag(self, file, *args, **kwargs):
        args = process_args(self, args, kwargs)
        self.compile(*args)
        regs_map = {}
        graph = nx.MultiDiGraph()
        latest_node = {}
        for name, reg in self.context.global_reg.items():
            regs_map[reg] = name
            node = DAGNode(ty='target', op=Operation("Init", reg, [], []), name=name)
            latest_node[name] = node
            graph.add_node(node)
        for name, reg in self.context.local_reg.items():
            regs_map[reg] = name
            node = DAGNode(ty='temp', op=Operation("Init", reg, [], []), name=name)
            latest_node[name] = node
            graph.add_node(node)

        for op in self.context.ops:
            name = regs_map.get(op.target)
            previous_node = latest_node[name]
            node = DAGNode(ty='op', op=op, name=op.name)
            graph.add_node(node)
            graph.add_edge(previous_node, node, label=name)
            latest_node[name] = node
            for item in op.ancs:
                ancs_name = regs_map.get(item)
                ancs = latest_node[ancs_name]
                graph.add_edge(ancs, node, label=ancs_name, color='green')
            for item in op.controls:
                ctrls_name = regs_map.get(item)
                ctrls = latest_node[ctrls_name]
                graph.add_edge(ctrls, node, label=ctrls_name, color='green')

        for node in graph.nodes:
            n = graph.nodes[node]
            n['label'] = node.name
            if node.type == 'op':
                n['color'] = 'blue'
                n['style'] = 'filled'
                n['fillcolor'] = 'lightblue'
            if node.type == 'target':
                n['color'] = 'black'
                n['style'] = 'filled'
                n['fillcolor'] = 'red'
            if node.type == 'temp':
                n['color'] = 'black'
                n['style'] = 'filled'
                n['fillcolor'] = 'white'
        # for e in graph.edges(data=True):
        #     e[2]['label'] = e[2]['name']
        dot = nx.nx_pydot.to_pydot(graph)
        g = graphviz.Source(str(dot))
        g.render(file, format='png')

    def uncompute(self):
        pass

    def __str__(self):
        return ""

    def dump(self, *args, **kwargs):
        args = process_args(self, args, kwargs)
        self.compile(*args)
        ptr = ''
        for k, v in self.context.global_reg.items():
            ptr += "%s : %s\n" % (k, v)
        for k, v in self.context.local_reg.items():
            ptr += "%s : %s\n" % (k, v)
        for i in self.context.ops:
            ptr += "%s\n" % i
        print(ptr)
