#!/usr/bin/env/python

import time
import mycelia
import networkx

def example1():
    g.clear()
    g.add_node(5, color=(1,1,0,1), size=3, label='blah')
    g.add_edge(1,2,label='hi')
    g.node[2]['color'] = (1,.5,.5)
    g.node[2]['label'] = 2
    g.sync_node_attr(2)
    g.center()

def example2():
    g.clear()
    networkx.generators.random_lobster(50, .3, .5, create_using=g)

def example3():
    g.clear()
    for x in range(30):
        g.add_node(x)
    g.center()
    time.sleep(5)
    g.stop_layout()
    for x in range(30):
        g.add_cycle(range(30,40))

    time.sleep(5)
    g.start_layout()

if __name__ == '__main__':
    g = mycelia.MyceliaDiGraph('http://localhost:9876')

    raw_input("\nChange the layout to dynamic and press enter when finished...\n")

    example1()
    
    w = 10
    msg = "Waiting %s seconds before next example..." % w

    print msg
    time.sleep(w)

    example2()

    print msg
    time.sleep(w)

    example3()

