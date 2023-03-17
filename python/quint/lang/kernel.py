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


class quantum(object):

    def __init__(self, function, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

        if not callable(function):
            self.convert_python_data_to_kernel()
        else:
            self.function = function

        # func_body = '\n'.join(inspect.getsource(self.function).split('\n')[1:])
        file = inspect.getsourcefile(function)
        src, start_lineno = inspect.getsourcelines(function)
        src = [textwrap.fill(line, tabsize=4, width=9999) for line in src]
        tree = ast.parse(textwrap.dedent("\n".join(src)))
        self.context = ASTTransformerContext(func=function, file=file, src=src, start_lineno=start_lineno)
        get_runtime().add_node(function.__name__, self.context)
        self.extract_arguments(self.context)
        visitor = TransformVisitor()
        visitor.visit(self.context, tree)

    def extract_arguments(self, ctx):
        sig = inspect.signature(self.function)
        params = sig.parameters
        arg_names = params.keys()
        for i, arg_name in enumerate(arg_names):
            param = params[arg_name]
            annotation = param.annotation
            ctx.func_arguments.append(KernelArgument(annotation, param.name, param.default))

    def __call__(self, *args, **kwargs):
        pass

    def convert_python_data_to_kernel(self):
        pass

    def show_dag(self, file):
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
        ptr = ''
        for k, v in self.context.global_reg.items():
            ptr += "%s : %s\n" % (k, v)
        for k, v in self.context.local_reg.items():
            ptr += "%s : %s\n" % (k, v)
        for i in self.context.ops:
            ptr += "%s\n" % i
        return ptr
