"""
    NetworkX interface to Mycelia.
"""

import warnings

from UserDict import IterableUserDict
import collections

import xmlrpclib

import networkx as nx


__all__ = ('MyceliaGraph', 'MyceliaDiGraph')

def ensure_nxversion():
    nx_min_major = 1
    nx_min_minor = 0
    nx_min_rev = 1364

    nx_max_major = 1
    nx_max_minor = 1
    nx_max_rev = ''

    nx_min = '%s.%s.dev%s' % (nx_min_major, nx_min_minor, nx_min_rev)
    nx_max = '%s.%s.dev%s' % (nx_max_major, nx_max_minor, nx_max_rev)

    msg = 'Mycelia Python bindings require NetworkX between %s and %s. '
    msg += 'NetworkX %s was detected.'
    msg = msg % (nx_min, nx_max, nx.__version__)
    from pkg_resources import parse_version as V  # setuptools
    if not V(nx_min) <= V(nx.__version__) < V(nx_max):
        raise Exception(msg)

ensure_nxversion()

##########

class InvertibleDict(IterableUserDict):
    """A reversible dictionary, providing an index() method.

    For use when there is a 1-1 correspondence between the keys and values.

    Examples
    --------
    >>> x = InvertibleDict()
    >>> x[1] = 2
    >>> x[3] = 4
    >>> x.index(2)
    1
    
    If a new key is assigned to a previously assigned value, the original key 
    of the value is deleted from the dictionary.    

    >>> x[5] = 2
    >>> 1 in x
    False
    >>> x.index(2)
    5

    """
    def __init__(self, dict=None, **kwargs):
        IterableUserDict.__init__(self, dict, **kwargs)
        self.revmap = {}

    def __delitem__(self, key):
        value = self[key]
        IterableUserDict.__delitem__(self, key)
        del self.revmap[value]

    def __setitem__(self, key, value):
        IterableUserDict.__setitem__(self, key, value)
        if value in self.revmap:
            oldkey = self.revmap[value]
            del self[oldkey]
        self.revmap[value] = key

    def index(self, value, *default):
        """Return set of keys mapping to value.

        If no additional arguments are passed, a ValueError is raised
        if the value does not exist in the dictionary.
        
        If an additional argument is passed, then it is returned if 
        the value does not exist in the dictionary.

        """
        assert(len(default) < 2)
        if value not in self.revmap:
            if len(default) == 0:
                msg = "%s is an invalid value, it has no key" % (repr(value),)
                raise ValueError(msg)
            else:
                return default[0]
        else:
            return self.revmap[value]

class MyceliaBaseGraph(object):

    count = 0

    layout_types = {'static': 0,
                    'dynamic' : 1}

    def __init__(self, address):
        if MyceliaGraph.count == 1:
            msg = "Multiple MyceliaBaseGraph instances are not allowed."
            raise Exception(msg)
        MyceliaBaseGraph.count += 1

        self.mycelia_nodes = InvertibleDict()
        self.mycelia_edges = InvertibleDict()
        self.connect(address)

    def __del__(self):
        MyceliaBaseGraph.count -= 1

    def center(self):
        self.server.center()

    def connect(self, address):
        self.address = address
        self.server = xmlrpclib.Server(address)

    def reset_layout(self):
        self.server.layout()

    def setup_callback_server(self, address='127.0.0.1', port=20999):
        self.callback_server = SimpleXMLRPCServer((address, port))
        self.callback_server.register_introspection_functions()

    def set_layout_type(self, layout_type='static'):
        """Sets the Mycelia layout type.

        Valid arguments are:
            'static'    - static layout
            'dynamic'   - dynamic layout

        """
        layout = self.layout_types[layout_type]
        self.server.set_layout_type(layout)        

    def set_node_callback(self, address, callback, port=20999):
        """Sets the node callback function.

        You must setup the callback server before calling this method.
        Otherwise, an exception is raised.  See setup_callback_server().

        Once this method is called, the RPC server will be started
        and further input is blocked.  A keyboard interrupt will stop
        the server.        

        Examples
        --------
        >>> g.set_node_callback('http://localhost', myFunction, port=20999)

        """
        if not hasattr(self, 'callback_server'):
            raise Exception('run setup_callback_server() first')

        self.callback = callback
        callback_server = ''.join([address,':',str(port)])
        self.server.set_callback(callback_server, self.callback.__name__)
        self.callback_server.register_function(self.callback)
        self.callback_server.serve_forever() 

    def set_status(self, status):
        self.server.set_status(str(status))

    def start_layout(self):
        self.server.start_layout()

    def stop_layout(self):
        self.server.stop_layout()

    def sync_edge_attr(self, u, v):
        """Updates Mycelia edge attributes to match NetworkX edge attributes."""
        if 'label' in self.edge[u][v]:
            label = self.edge[u][v]['label']
            edge = self.mycelia_edges[(u,v)]
            self.server.set_edge_label(edge, str(label))

    def sync_node_attr(self, n):
        """Updates Mycelia node attributes to match NetworkX node attributes."""
        if 'color' in self.node[n]:
            color = self.node[n]['color']
            if len(color) == 4:
                r, g, b, a = map(float,color)
            else:
                r, g, b = map(float, color)
                a = 1.0
        
            node = self.mycelia_nodes[n]
            self.server.set_vertex_color(node, r, g, b, a)
        
        if 'size' in self.node[n]:
            size = self.node[n]['size']
            node = self.mycelia_nodes[n]
            self.server.set_vertex_size(node, float(size))

        if 'label' in self.node[n]:
            label = self.node[n]['label']
            node = self.mycelia_nodes[n]
            self.server.set_vertex_label(node, str(label))

def mycelia_add_node(baseclass):
    def add_node(self, n, attr_dict=None, **attr):
        if n not in self:
            mycelia_node = self.server.new_vertex()
            self.mycelia_nodes[n] = mycelia_node
            
        baseclass.add_node(self, n, attr_dict, **attr)
        self.sync_node_attr(n)

    add_node.__doc__ = baseclass.add_node.__doc__
    return add_node

def mycelia_add_nodes_from(baseclass):
    def add_nodes_from(self, nodes, **attr):
        for node in nodes:
            self.add_node(node, **attr)

    add_nodes_from.__doc__ = baseclass.add_nodes_from.__doc__
    return add_nodes_from

def mycelia_remove_node(baseclass):
    def remove_node(self, n):
        baseclass.remove_node(self, n)

        nn = self.mycelia_nodes[n]
        del self.mycelia_nodes[n]
        self.server.delete_vertex(nn)

    remove_node.__doc__ = baseclass.remove_node.__doc__
    return remove_node

def mycelia_remove_nodes_from(baseclass):
    def remove_nodes_from(self, nodes):
        for n in nodes:
            self.remove_node()

    remove_nodes_from.__doc__ = baseclass.remove_nodes_from.__doc__
    return remove_nodes_from

def mycelia_add_edge(baseclass):
    def add_edge(self, u, v, attr_dict=None, **attr):
        if not self.has_edge(u,v):
            self.add_nodes_from([u,v])
            uu, vv = self.mycelia_nodes[u], self.mycelia_nodes[v]
            mycelia_edge = self.server.new_edge(uu, vv)
            self.mycelia_edges[(u,v)] = mycelia_edge

        baseclass.add_edge(self, u, v, attr_dict, **attr)
        self.sync_edge_attr(u,v)

    add_edge.__doc__ = baseclass.add_edge.__doc__
    return add_edge

def mycelia_add_edges_from(baseclass):
    def add_edges_from(self, ebunch, attr_dict=None, **attr):
        # set up attribute dict
        if attr_dict is None:
            attr_dict=attr
        else:
            try:
                attr_dict.update(attr)
            except AttributeError:
                raise NetworkXError(\
                    "The attr_dict argument must be a dict.")

        for e in ebunch:
            ne = len(e)
            if ne==3:
                u,v,dd = e
            elif ne==2:
                u,v = e
                dd = {}
            else:
                msg = "Edge tuple %s must be a 2-tuple or 3-tuple." %(e,)
                raise NetworkXError(msg)

            self.add_edge(u,v,attr_dict,**dd)
            uu, vv = self.mycelia_nodes[u], self.mycelia_nodes[v]
            mycelia_edge = self.server.new_edge(uu, vv)
            self.mycelia_edges[(u,v)] = mycelia_edge

            self.sync_edge_attr(u,v)

    add_edges_from.__doc__ = baseclass.add_edges_from.__doc__
    return add_edges_from        
    
def mycelia_remove_edge(baseclass):
    def remove_edge(self, u, v):
        baseclass.remove_edge(self, u,v)
        uv = self.mycelia_edges[(u,v)]
        del self.mycelia_edges[(u,v)]
        self.server.delete_edge(uv)
    remove_edge.__doc__ = baseclass.remove_edge.__doc__  
    return remove_edge

def mycelia_remove_edges_from(baseclass):    
    def remove_edges_from(self, ebunch):
        baseclass.remove_edges_from(self, ebunch)
        for e in ebunch:
            (u,v)=e[:2]  # ignore edge data

            uv = self.mycelia_edges[(u,v)]
            del self.mycelia_edges[(u,v)]
            self.server.delete_edge(uv)

    remove_edges_from.__doc__ = baseclass.remove_edges_from.__doc__
    return remove_edges_from

def mycelia_clear(baseclass):
    def clear(self):
        self.mycelia_nodes = InvertibleDict()
        self.mycelia_edges = InvertibleDict()

        baseclass.clear(self)
        self.server.clear()

    clear.__doc__ = baseclass.clear.__doc__
    return clear


class MyceliaGraph(MyceliaBaseGraph, nx.Graph):
    """An undirected graph which is drawn interactively via Mycelia.

    The implementation uses NetworkX's Graph as the underlying structure and 
    mirrors this structure to Mycelia.

    Note: 
        Multiple instantiations of MyceliaGraph will result in graph
        structures in Python which are not consistent with the graph
        structure in Mycelia. To prevent this, multiple instances are 
        disallowed.

    """
    def __init__(self, address, data=None, name='', **attr):
        """Initialize the MyceliaGraph instance.

        Arguments
        ---------

        address : str
            The location of the listening Mycelia XMLRPC server.

        All other arguments are passed to networkx.Graph.__init__.

        """
        MyceliaBaseGraph.__init__(self, address)
        nx.Graph.__init__(self, data, name, **attr)

    baseclass = nx.Graph
    add_node = mycelia_add_node(baseclass)
    add_nodes_from = mycelia_add_nodes_from(baseclass)
    remove_node = mycelia_remove_node(baseclass)
    remove_nodes_from = mycelia_remove_nodes_from(baseclass)
    add_edge = mycelia_add_edge(baseclass)
    add_edges_from = mycelia_add_edges_from(baseclass)
    remove_edge = mycelia_remove_edge(baseclass)
    remove_edges_from = mycelia_remove_edges_from(baseclass)
    clear = mycelia_clear(baseclass)
    del baseclass

class MyceliaDiGraph(MyceliaBaseGraph, nx.DiGraph):
    """A directed graph which is drawn interactively via Mycelia.

    The implementation uses NetworkX's DiGraph as the underlying structure and 
    mirrors this structure to Mycelia.

    Note: 
        Multiple instantiations of MyceliaDiGraph will result in graph
        structures in Python which are not consistent with the graph
        structure in Mycelia. To prevent this, multiple instances are 
        disallowed.

    """
    def __init__(self, address, data=None, name='', **attr):
        """Initialize the MyceliaDiGraph instance.

        Arguments
        ---------

        address : str
            The location of the listening Mycelia XMLRPC server.

        All other arguments are passed to networkx.DiGraph.__init__.

        """
        MyceliaBaseGraph.__init__(self, address)
        nx.DiGraph.__init__(self, data, name, **attr)

    baseclass = nx.DiGraph
    add_node = mycelia_add_node(baseclass)
    add_nodes_from = mycelia_add_nodes_from(baseclass)
    remove_node = mycelia_remove_node(baseclass)
    remove_nodes_from = mycelia_remove_nodes_from(baseclass)
    add_edge = mycelia_add_edge(baseclass)
    add_edges_from = mycelia_add_edges_from(baseclass)
    remove_edge = mycelia_remove_edge(baseclass)
    remove_edges_from = mycelia_remove_edges_from(baseclass)
    clear = mycelia_clear(baseclass)
    del baseclass



