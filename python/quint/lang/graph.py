class DAGNode:

    def __init__(self, ty=None, op=None, name=None, condition=None):
        self.type = ty
        self.op = op
        self.name = name

    def __hash__(self):
        """Needed for ancestors function, which returns a set.
        To be in a set requires the object to be hashable
        """
        return hash(id(self))

    def __str__(self):
        # TODO is this used anywhere other than in DAG drawing?
        # needs to be unique as it is what pydot uses to distinguish nodes
        return str(id(self))
