from enum import Enum


class EdgeType(Enum):
    A = 0,
    C = 1,
    D = 2,
    U = 3


class Edge:

    def __init__(self, node_from, node_to, edge_type):
        self.node_from = node_from
        self.node_to = node_to
        self.type = edge_type


class Node:

    def __init__(self, register, index, op, value_id=0, copy_id=0):
        self.register = register
        self.index = index
        self.op = op
        self.value_id = value_id
        self.copy_id = copy_id
        self.variable_name = register.name
        self.ctrl_edges_in = []
        self.non_ctrl_edges_in = []
        self.edges_out = []
        self.consume_edge_in = None
        self.consume_edge_out = None


class DepGraph:

    def __init__(self):
        self.nodes = {}
        self.deallocate_nodes = []

    def addNode(self, node):
        list_nodes = self.nodes.get(node.variable_name)
        if list_nodes is not None:
            if len(list_nodes) > node.value_id:
                assert len(list_nodes[node.value_id]) == node.copy_id
                list_nodes[node.value_id].append(node)
            else:
                assert len(list_nodes) == node.value_id and node.copy_id == 0
                list_nodes.append([node])
        else:
            assert node.value_id == 0 and node.copy_id == 0
            self.nodes[node.variable_name] = [[node]]

    def connectAvailabilityNodes(self, node_from, node_to):
        assert node_from.variable_name != node_to.variable_name
        e = Edge(node_from, node_to, EdgeType.A)
        node_from.edges_out.append(e)
        node_to.non_ctrl_edges_in.append(e)
        return e

    def connectConsumeNodes(self, node_from, node_to):
        assert node_from.variable_name == node_to.variable_name
        assert node_from.value_id == node_to.value_id + 1 or node_from.value_id == node_to.value_id - 1

        edge = Edge(node_from, node_to, EdgeType.C)
        node_from.consume_edge_out = edge
        node_to.consume_edge_in = edge
        avail_edges = self.updateAvailabilityEdge(edge)
        avail_edges.append(edge)
        return avail_edges

    def connectDependencyNodes(self, node_from, node_to):
        assert node_from.variable_name != node_to.variable_name
        assert self.nodes[node_from.variable_name][node_from.value_id][node_from.copy_id] == node_from
        assert self.nodes[node_to.variable_name][node_to.value_id][node_to.copy_id] == node_to
        e = Edge(node_from, node_to, EdgeType.D)
        node_from.edges_out.append(e)
        node_to.ctrl_edges_in.append(e)
        added_avail_edges = self.updateAvailabilityEdge(e)
        added_avail_edges.append(e)
        return added_avail_edges

    def updateAvailabilityEdge(self, edge):
        new_edges = []
        if edge.type == EdgeType.C:
            for edge_out in edge.node_from.edges_out:
                if edge_out.type == EdgeType.D:
                    e = self.connectAvailabilityNodes(edge_out.node_to, edge.node_to)
                    new_edges.append(e)
        elif edge.type == EdgeType.D:
            if edge.node_from.consume_edge_out is not None:
                e = self.connectAvailabilityNodes(edge.node_to, edge.node_from.consume_edge_out.node_to, new_edges)
                new_edges.append(e)
        return new_edges

    def aux_topological_sort(self, sorted_nodes, seen_nodes, node):
        seen = seen_nodes.get(node)
        if seen is not None:
            if seen == 1:
                print("has cycle")
                print(node)
                assert False
            else:
                return
        seen_nodes[node] = 1
        for e in node.edges_out:
            self.aux_topological_sort(sorted_nodes, seen_nodes, node.consume_edge_out.node_to)
        seen_nodes[node] = 2
        sorted_nodes.append(node)

    def nodesInTopologicalOrder(self):
        sorted_nodes = []
        seen_nodes = {}
        for variable_name in self.nodes:
            for list_copies in self.nodes[variable_name]:
                for node in list_copies:
                    self.aux_topological_sort(sorted_nodes, seen_nodes, node)
        sorted_nodes.reverse()
        return sorted_nodes

    def latestCopy(self, node):
        assert self.nodes[node.variable_name] is not None and len(self.nodes[node.variable_name]) > node.value_id
        return self.nodes[node.variable_name][node.value_id][-1]

    def hasCycleWith(self, orig_node):
        marked = {}

        def hasCycle(node):
            t = marked.get(node)
            if t is not None:
                if t == 2:
                    return True
                else:
                    return False
            marked[node] = 2
            if node.consume_edge_out and hasCycle(node.consume_edge_out.node_to):
                return True
            for e in node.edges_out:
                if hasCycle(e.node_to):
                    return True
            marked[node] = 1
            return False

        return hasCycle(orig_node)
