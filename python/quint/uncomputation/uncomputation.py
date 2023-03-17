from quint.uncomputation.depgraph import DepGraph, Node, Edge, EdgeType


def modConvertToDepGraph(operations, registers):
    graph = DepGraph()
    latest_node = {}

    for reg in registers:
        node = Node(reg, reg.addr, "init")
        latest_node[node.variable_name] = node
        graph.addNode(node)

    for op in operations:
        wire_name = op.target.name
        assert latest_node.get(wire_name) is not None
        previous_node = latest_node[wire_name]
        node = Node(op.target, op.target.addr, op.name, previous_node.value_id+1, 0)
        graph.addNode(node)
        graph.connectConsumeNodes(previous_node, node)
        latest_node[node.variable_name] = node
        for qarg in op.ancs:
            wire_dep_name = qarg.name
            aux_node = latest_node[wire_dep_name]
            graph.connectDependencyNodes(aux_node, node)

        for qarg in op.controls:
            wire_dep_name = qarg.name
            ctrl_node = latest_node[wire_dep_name]
            graph.connectDependencyNodes(ctrl_node, node)

    return graph


def uncomputeAllLocal(operations, registers):
    graph = modConvertToDepGraph(operations, registers)
    uncomp = Uncomputation()
    nodes_to_uncomp = [n for n in graph.nodesInTopologicalOrder() if
                       n.register.local is True and n.value_id > 0]
    uncomp.uncomputeListNodes(graph, nodes_to_uncomp)


class Uncomputation:

    def __init__(self):
        pass

    def uncomputeListNodes(self, dep_graph, nodes):
        while nodes:
            cur_node = nodes.pop()
            uncomp_succeeded = self.uncomputeNode(dep_graph, cur_node)
            if not uncomp_succeeded:
                return False
        return True

    def uncomputeNode(self, dep_graph, node):
        if node.value_id <= 0:
            print("node.value_id <= 0")
            return False
        if dep_graph.nodes[node.variable_name] is None or len(dep_graph.nodes[node.variable_name]) <= node.value_id:
            return False

        latest_node = dep_graph.nodes[node.variable_name][node.value_id][-1]
        if latest_node.consume_edge_out is not None:
            print("Tried to uncompute node with an outgoing consume edge")
            return False

        copy_id = 1
        operation = ""
        if node.op == "Push":
            operation = "Pop"
        else:
            operation = node.op
        uncomputation_node = Node(node.register, node.index, operation, node.value_id - 1, copy_id)
        dep_graph.addNode(uncomputation_node)
        new_edges = dep_graph.connectConsumeNodes(latest_node, uncomputation_node)

        for ctrl_edge in node.ctrl_edges_in:
            ctrl_node = ctrl_edge.node_from
            ctrl_node_latest = dep_graph.latestCopy(ctrl_node)
            dep_graph.connectDependencyNodes(ctrl_node_latest, uncomputation_node)

        return not dep_graph.hasCycleWith(uncomputation_node)
