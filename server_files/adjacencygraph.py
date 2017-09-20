class AdjacencyGraph:
    '''Type to represent directed graphs using adjacency storage.

    Attributes:
        _vertices (dict): mapping vertex identifiers to a list of vertices.
        _colours (dict): mapping vertex identifiers to their colour.
    '''

    def __init__(self):
        self._vertices = dict()
        self._colours = dict()

    def add_vertex(self, v):
        ''' Adds a new vertex with identifier v to the graph.

        Args:
            v (hashable type): the vertex identifier to be added.

        Raises:
            RuntimeError: If the vertex was already in the graph.
        '''
        if v in self._vertices:
            raise RuntimeError("Bad argument:"
                               " Vertex {} already in the graph".format(v))
        self._vertices[v] = list()


    def add_colour(self, v, colour):
        ''' Adds a colour (number) to a specific vertex.

        Args:
            v (hashable type): the vertex to be modified
            colour (integer): the colour to be added
        '''
        self._colours[v] = colour

    def colour(self,v):
        ''' checks the colour of a specified vertex.

        Args:
            v (hashable type): the vertex to be checked for its colour.

        Returns:
            0 if v isn't in the colour dict,
            Otherwise returns the vertex's colour.
        '''
        if v not in self._colours:
            return 0
        else:
            return self._colours[v]

    def colours(self):
        ''' Returns the list of colours for a graph.

        Returns:
            A list of the colours in the graph currently
        '''
        return [self.colour(v) for v in self.vertices()]

    def is_vertex(self, v):
        '''Checks whether v is a vertex of the graph.

        Args:
            v (hashable type): the vertex identifier to be added.

        Returns:
            bool: True if v is a vertex of the graph, False otherwise.
        '''
        return v in self._vertices

    def add_edge(self, e, autocreation=False):
        ''' Adds edge e to the graph.

        Args:
            e (tuple of two hashables): The edge to be added as a tuple. The
                edge goes from e[0] to e[1]
            autocreation (bool, default: False): Should the verticies of the
                edge be automatically added to the set of verticies if they are
                not there?

        Raises:
            RuntimeError: When one of the verticies is not a vertex yet
                and autocreation is off
        '''
        if type(autocreation) != bool:
            raise RuntimeError("Autocreation argument is not bool.")
        if autocreation:
            self._vertices.setdefault(e[0], list())
            self._vertices.setdefault(e[1], list())
        else:
            if not self.is_vertex(e[0]):
                raise RuntimeError("Attempt to create an edge with"
                                   "non-existent vertex: {}".format(e[0]))
            if not self.is_vertex(e[1]):
                raise RuntimeError("Attempt to create an edge with"
                                   "non-existent vertex: {}".format(e[1]))

        self._vertices[e[0]].append(e[1])

    def is_edge(self, e):
        ''' Checks whether an edge e exists in self._edges

        Args:
            e (tuple of two hashables): The edge to be added as a tuple. The
                edge goes from e[0] to e[1]
        Returns:
            bool: True if e is an edge of the graph, False otherwise.
        '''
        # @TODO Homework!
        if e[1] in self.neighbours(e[0]):
            return True
        else:
            return False

    def neighbours(self, v):
        '''Returns the list of vertices that are neighbours to v.'''
        return self._vertices[v]

    def vertices(self):
        '''Returns the set of vertices.

        Note that the set returned is a copy of the set of vertices
        in the graph, i.e., modifying the returned set won't change
        the set of vertices.
        '''
        return set(self._vertices.keys())


class UndirectedAdjacencyGraph:
    '''Type to represent directed graphs using adjacency storage.

    Attributes:
        _digraph (AdjacencyGraph): The underlying directed graph that stores
            the data associated with the graph.
    '''

    def __init__(self):
        self._digraph = AdjacencyGraph()
        self._vertices = dict()
        self._colours = dict()

    def add_colour(self, v, colour):
        ''' Adds a colour (number) to a specific vertex.

        Args:
            v (hashable type): the vertex to be modified
            colour (integer): the colour to be added
        '''
        self._colours[v] = colour

    def colour(self,v):
        ''' checks the colour of a specified vertex.

        Args:
            v (hashable type): the vertex to be checked for its colour.

        Returns:
            0 if v isn't in the colour dict,
            Otherwise returns the vertex's colour.
        '''
        if v not in self._colours:
            return 0
        else:
            return self._colours[v]

    def colours(self):
        ''' Returns the list of colours for a graph.

        Returns:
            A list of the colours in the graph currently
        '''
        return [self.colour(v) for v in self.vertices()]
        
    def add_vertex(self, v):
        ''' Adds a new vertex with identifier v to the graph.

        Args:
            v (hashable type): the vertex identifier to be added.

        Raises:
            RuntimeError: If the vertex was already in the graph.
        '''
        self._digraph.add_vertex(v)

    def is_vertex(self, v):
        '''Checks whether v is a vertex of the graph.

        Args:
            v (hashable type): the vertex identifier to be added.

        Returns:
            bool: True if v is a vertex of the graph, False otherwise.
        '''
        return self._digraph.is_vertex(v)

    def add_edge(self, e, autocreation=False):
        ''' Adds edge e to the graph.

        Args:
            e (tuple of two hashables): The edge to be added as a tuple. The
                edge goes from e[0] to e[1]
            autocreation (bool, default: False): Should the verticies of the
                edge be automatically added to the set of verticies if they are
                not there?

        Raises:
            RuntimeError: When one of the verticies is not a vertex yet
                and autocreation is off
        '''
        self._digraph.add_edge(e, autocreation)
        # no need for autocreation on second call:
        self._digraph.add_edge((e[1], e[0]))

    def is_edge(self, e):
        ''' Checks whether an edge e exists in self._edges

        Args:
            e (tuple of two hashables): The edge to be added as a tuple. The
                edge goes from e[0] to e[1]
        Returns:
            bool: True if e is an edge of the graph, False otherwise.
        '''
        return self._digraph.is_edge(e)

    def neighbours(self, v):
        '''Returns the list of vertices that are neighbours to v.'''
        return self._digraph.neighbours(v)

    def vertices(self):
        '''Returns the set of vertices.

        Note that the set returned is a copy of the set of vertices
        in the graph, i.e., modifying the returned set won't change
        the set of vertices.
        '''
        return self._digraph.vertices()
