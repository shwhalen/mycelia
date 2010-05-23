#!/usr/bin/env python

from SimpleHTMLParser import SimpleTextHTMLParser
from SimpleXMLRPCServer import SimpleXMLRPCServer

from string import find, split
from threading import Thread
from urllib import FancyURLopener
from xmlrpclib import Server

# sets a plausible User-Agent, or google will forbid search
class GoogleOpener(FancyURLopener):
    version = 'Mozilla/5.0 (Windows; U; Windows NT 5.1; it; rv:1.8.1.11) Gecko/20071127 Firefox/2.0.0.11'

# adds hyperlinks from a url or search term as new nodes to the graph,
# connected to the page containing the link
class LookupThread(Thread):
    def __init__(self, parent_node):
        global nodes
        
        Thread.__init__(self)
        parent_url = nodes[parent_node]
        
        # if a url is passed in without protocol identifier, assume it's a search term for google
        if parent_url.startswith('http://'):
            self.is_search = False
        else:
            parent_url = 'http://www.google.com/search?q=' + parent_url
            self.is_search = True
            
        self.parent_url = parent_url
        self.parent_node = parent_node
    
    def run(self):
        global graph

        opener = GoogleOpener()
        html = opener.open(self.parent_url).read()
        opener.close()
        
        parser = SimpleTextHTMLParser()
        parser.parse(html, self.parent_url)
        
        for url in parser.listURLs:
            skip_url = False
            
            if self.is_search:
                # if this is a search, skip links to google including ip addresses
                for s in ['google.com', '/search?q=']:
                    if find(url, s) != -1:
                        skip_url = True
                        break
            else:
                # skip self-links
                if url == self.parent_url: 
                    skip_url = True
                    break
        
                # skip links to non-html pages
                for s in ['.png', '.jpg', '.gif']:
                    if url.endswith(s):
                        skip_url = True
                        break

            if not skip_url:
				v = graph.new_vertex()
 				domain = split(url, '/')[2]
				graph.set_vertex_label(v, domain)
				graph.new_edge(self.parent_node, v)
				nodes[v] = url

# called when user selects a node
def vertex_callback(id):
    try:
        LookupThread(id).start()
    except Exception, detail:
        print type(detail), detail 
        return -1

    return 0

graph = Server('http://localhost:9876')
graph.clear()

# create initial node for google search term
root_node = graph.new_vertex()
root_term = 'orange'
nodes = {}
nodes[root_node] = root_term
LookupThread(root_node).start()

# callback server to handle node double clicks
port = 20999
graph.set_callback('http://localhost:' + str(port), 'vertex_callback')

callback_server = SimpleXMLRPCServer(('127.0.0.1', port))
callback_server.register_introspection_functions()
callback_server.register_function(vertex_callback)
callback_server.serve_forever()
