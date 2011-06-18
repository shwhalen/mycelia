import SimpleXMLRPCServer
import threading
import urllib
import xmlrpclib

from BeautifulSoup import BeautifulSoup

def click_callback(node):
    try:
        LookupThread(node).start()
    except Exception as e:
        print e
        return -1
    return 0


class GoogleOpener(urllib.FancyURLopener):
    version = 'Mozilla/5.0 (Windows; U; Windows NT 5.1; it; rv:1.8.1.11) Gecko/20071127 Firefox/2.0.0.11'


class LookupThread(threading.Thread):
    def __init__(self, parent_node):
        threading.Thread.__init__(self)
        self.parent_node = parent_node


    def run(self):
        parent_url = nodes[self.parent_node]
        parent_domain = parent_url.split('/')[2].split('.')[-2]
        soup = BeautifulSoup(GoogleOpener().open(parent_url).read())
        for anchor in soup.body.findAll(name='a', attrs={'href': True}):
            url = anchor['href']
            if url.startswith('http://') and url.find(parent_domain) == -1 and not url[-3:] in set(['jpg', 'png', 'gif']):
                child_node = server.add_node()
                child_domain = url.split('/')[2]
                server.set_node_label(child_node, child_domain)
                server.add_edge(self.parent_node, child_node)
                server.center()
                nodes[child_node] = url


# connect to server and clear any previous work
server = xmlrpclib.Server('http://localhost:9876')
server.clear()
server.set_layout_type(1)

# create initial node for google search term
root_node = server.add_node()
server.set_node_label(root_node, 'orange')
nodes = {root_node: 'http://www.google.com/search?q=orange'}
LookupThread(root_node).start()

# callback server to handle node clicks
port = 20999
server.set_callback('http://localhost:%i' % port, 'click_callback')
callback_server = SimpleXMLRPCServer.SimpleXMLRPCServer(('localhost', port))
callback_server.register_introspection_functions()
callback_server.register_function(click_callback)
callback_server.serve_forever()
