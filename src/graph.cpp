/*
 * Mycelia immersive 3d network visualization tool.
 * Copyright (C) 2008-2010 Sean Whalen.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <graph.hpp>

using namespace std;

Graph::Graph(Mycelia* application): application(application)
{
    init();
}

Graph& Graph::operator=(const Graph& g)
{
    application = g.application;
    version = g.version;
    
    nodes = g.nodes;
    nodeMap = g.nodeMap;
    
    edges = g.edges;
    edgeMap = g.edgeMap;
    
    materialVector = g.materialVector;
    
    return *this;
}

/*
 * general
 */
void Graph::clear()
{
    application->stopLayout();
    mutex.lock();
    
    init();
    
    mutex.unlock();
    application->clearSelections();
}

void Graph::init()
{
    nodes.clear();
    nodeMap.clear();
    nodeMap.rehash(1000);
    
    edges.clear();
    edgeMap.clear();
    edgeMap.rehash(1000);
    
    materialVector.clear();
    materialVector.resize(4);
    materialVector[MATERIAL_NODE_DEFAULT] = new GLMaterial(GLMaterial::Color(1.0, 1.0, 1.0));
    materialVector[MATERIAL_EDGE_DEFAULT] = new GLMaterial(GLMaterial::Color(0.3, 0.3, 0.3));
    materialVector[MATERIAL_SELECTED] = new GLMaterial(GLMaterial::Color(1.0, 0.5, 1.0));
    materialVector[MATERIAL_SELECTED_PREVIOUS] = new GLMaterial(GLMaterial::Color(1.0, 0.0, 1.0));
    
    version = -1;
    nodeId = -1;
    edgeId = -1;
}

const pair<Vrui::Point, Vrui::Scalar> Graph::locate()
{
    Vrui::Point center(0, 0, 0);
    Vrui::Scalar maxDistance = 0;
    int counted = 1;
    
    mutex.lock();
    
    foreach(int source, nodes)
    {
        if(!application->isSelectedComponent(source))
        {
            continue;
        }
        
        foreach(int target, nodes)
        {
            if(!application->isSelectedComponent(target))
            {
                continue;
            }
            
            Vrui::Scalar d = Geometry::mag(nodeMap[source].position - nodeMap[target].position);
            if(d > maxDistance) maxDistance = d;
        }
        
        center += (nodeMap[source].position - center) * (1.0 / counted);
        counted++;
    }
    
    mutex.unlock();
    
    if(maxDistance == 0) maxDistance = 30;
    return pair<Vrui::Point, Vrui::Scalar>(center, maxDistance);
}

const GLMaterial* Graph::getMaterial(int materialId)
{
    if(materialId < 0 || materialId >= (int)materialVector.size())
    {
        return materialVector[MATERIAL_NODE_DEFAULT];
    }
    
    return materialVector[materialId];
}

const int Graph::getVersion() const
{
    return version;
}

void Graph::randomizePositions(int radius)
{
    mutex.lock();
    
    foreach(int node, nodes)
    {
        float x = rand() % (2 * radius) - radius;
        float y = rand() % (2 * radius) - radius;
        float z = rand() % (2 * radius) - radius;
        nodeMap[node].position = Vrui::Point(x, y, z);
    }
    
    mutex.unlock();
}

void Graph::resetVelocities()
{
    mutex.lock();
    
    foreach(int node, nodes)
    {
        nodeMap[node].velocity = Vrui::Vector(0, 0, 0);
    }
    
    mutex.unlock();
}

void Graph::update()
{
    version++;
    Vrui::requestUpdate();
}

void Graph::write(const char* filename)
{
    mutex.lock();
    
    ofstream out(filename);
    out << "digraph G {" << endl;
    
    foreach(int node, nodes)
    {
        out << "  n" << node << "[ pos=\""
            << nodeMap[node].position[0] << ","
            << nodeMap[node].position[1] << ","
            << nodeMap[node].position[2] << "\" ];\n";
    }
    
    foreach(int edge, edges)
    {
        out << "  n" << edgeMap[edge].source << " -> n" << edgeMap[edge].target << ";\n";
    }
    
    out << "}\n";
    cout << "wrote " << filename << endl;
    
    mutex.unlock();
}

/*
 * edges
 */
const int Graph::addEdge(int source, int target)
{
    mutex.lock();
    
    if(!isValidNode(source) || !isValidNode(target))
    {
        cout << "invalid node(s): " << source << " " << target << endl;
        mutex.unlock();
        return -1;
    }
    
    edgeId++;
    edges.insert(edgeId);
    edgeMap[edgeId] = Edge(source, target);
    
    nodeMap[source].outDegree++;
    nodeMap[target].inDegree++;
    nodeMap[source].adjacent[target].push_back(edgeId);
    
    mutex.unlock();
    update();
    
    return edgeId;
}

void Graph::clearEdges()
{
    mutex.lock();
    
    foreach(int node, nodes)
    {
        nodeMap[node].adjacent.clear();
    }
    
    edges.clear();
    edgeMap.clear();
    
    mutex.unlock();
    update();
}

const int Graph::deleteEdge(int edge)
{
    mutex.lock();
    
    if(!isValidEdge(edge))
    {
        mutex.unlock();
        return -1;
    }
    
    Edge& e = edgeMap[edge];
    list<int>& neighbors = nodeMap[e.source].adjacent[e.target];
    neighbors.erase(find(neighbors.begin(), neighbors.end(), edge));
    
    edges.erase(edge);
    edgeMap.erase(edge);
    
    mutex.unlock();
    update();
    
    return edge;
}

const Edge& Graph::getEdge(int edge)
{
    return edgeMap[edge];
}

const list<int>& Graph::getEdges(int source, int target)
{
    if(hasEdge(source, target))
        return nodeMap[source].adjacent[target];
        
    return empty;
}

const std::string& Graph::getEdgeLabel(int edge)
{
    return edgeMap[edge].label;
}

const float Graph::getEdgeWeight(int edge)
{
    return edgeMap[edge].weight;
}

const set<int>& Graph::getEdges() const
{
    return edges;
}

const int Graph::getEdgeCount() const
{
    return (int)edges.size();
}

const bool Graph::hasEdge(int source, int target)
{
    return nodeMap[source].adjacent.find(target) != nodeMap[source].adjacent.end();
}

const bool Graph::isBidirectional(int edge)
{
    return isBidirectional(edgeMap[edge].source, edgeMap[edge].target);
}

const bool Graph::isBidirectional(int source, int target)
{
    return hasEdge(source, target) && hasEdge(target, source);
}

const bool Graph::isValidEdge(int edge) const
{
    return edgeMap.find(edge) != edgeMap.end();
}

void Graph::setEdgeLabel(int edge, const std::string& label)
{
    edgeMap[edge].label = string(label);
    
    update();
}

void Graph::setEdgeWeight(int edge, float weight)
{
    edgeMap[edge].weight = weight;
}

/*
 * nodes
 */
const int Graph::addNode()
{
    mutex.lock();
    
    Node n;
    n.position = Vrui::Point(VruiHelp::randomFloat(), VruiHelp::randomFloat(), VruiHelp::randomFloat());
    
    nodeId++;
    nodes.insert(nodeId);
    nodeMap[nodeId] = n;
    
    mutex.unlock();
    update();
    
    return nodeId;
}

const int Graph::addNode(const Vrui::Point& position)
{
    int id = addNode();
    setNodePosition(id, position);
    return id;
}

const int Graph::addNode(const string& s)
{
    int id = addNode();
    setNodeLabel(id, s);
    return id;
}

const int Graph::deleteNode()
{
    return deleteNode(*nodes.begin());
}

const int Graph::deleteNode(int node)
{
    mutex.lock();
    
    if(!isValidNode(node))
    {
        mutex.unlock();
        return -1;
    }
    
    vector<int> killList(0);
    
    foreach(int edge, edges)
    {
        Edge& e = edgeMap[edge];
        
        if(e.source == node || e.target == node)
        {
            killList.push_back(edge);
        }
    }
    
    foreach(int edge, killList)
    {
        edges.erase(edge);
        edgeMap.erase(edge);
    }
    
    nodes.erase(node);
    nodeMap.erase(node);
    
    mutex.unlock();
    update();
    
    return node;
}

const Attributes& Graph::getNodeAttributes(int node)
{
    return nodeMap[node].attributes;
}

const int Graph::getNodeComponent(int node)
{
    return nodeMap[node].component;
}

const int Graph::getNodeDegree(int node)
{
    return nodeMap[node].inDegree + nodeMap[node].outDegree;
}

const string& Graph::getNodeLabel(int node)
{
    return nodeMap[node].label;
}

const GLMaterial* Graph::getNodeMaterial(int node)
{
    return getMaterial(nodeMap[node].material);
}

const set<int>& Graph::getNodes() const
{
    return nodes;
}

const int Graph::getNodeCount() const
{
    return (int)nodes.size();
}

const Vrui::Point& Graph::getNodePosition(int node)
{
    return nodeMap[node].position;
}

const float Graph::getNodeSize(int node)
{
    return nodeMap[node].size;
}

const Vrui::Point& Graph::getSourceNodePosition(int edge)
{
    return nodeMap[edgeMap[edge].source].position;
}

const Vrui::Point& Graph::getTargetNodePosition(int edge)
{
    return nodeMap[edgeMap[edge].target].position;
}

const Vrui::Vector& Graph::getNodeVelocity(int node)
{
    return nodeMap[node].velocity;
}

const bool Graph::isValidNode(int node) const
{
    return nodeMap.find(node) != nodeMap.end();
}

void Graph::setNodeAttribute(int node, string& key, string& value)
{
    nodeMap[node].attributes.push_back(pair<string, string>(key, value));
}

void Graph::setNodeColor(int node, int r, int g, int b, int a)
{
    setNodeColor(node, r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

void Graph::setNodeColor(int node, double r, double g, double b, double a)
{
    GLMaterial::Color c(r, g, b, a);
    int materialId = -1;
    
    // look for color in the cache
    for(int i = 0; i < (int)materialVector.size(); i++)
    {
        if(materialVector[i]->ambient == c)
        {
            materialId = i;
            break;
        }
    }
    
    // if not found, add it
    if(materialId == -1)
    {
        materialVector.push_back(new GLMaterial(c));
        materialId = materialVector.size() - 1;
    }
    
    nodeMap[node].material = materialId;
    
    update();
}

void Graph::setNodeLabel(int node, const std::string& label)
{
    nodeMap[node].label = label;
    
    update();
}

void Graph::setNodePosition(int node, const Vrui::Point& position)
{
    nodeMap[node].position = position;
    
    update();
}

void Graph::setNodeVelocity(int node, const Vrui::Vector& velocity)
{
    nodeMap[node].velocity = velocity;
}

void Graph::setNodeSize(int node, float size)
{
    nodeMap[node].size = size;
    
    update();
}

void Graph::updateNodePosition(int node, const Vrui::Vector& delta)
{
    nodeMap[node].position += delta;
    
    update();
}

// no update() needed
void Graph::updateNodeVelocity(int node, const Vrui::Vector& delta)
{
    nodeMap[node].velocity += delta;
}

/*
 * boost wrappers
 */
boost::BoostGraph Graph::toBoost()
{
    boost::BoostGraph g;
    
    for(int node = 0; node > getNodeCount(); node++)
    {
        boost::add_vertex(g);
    }
    
    for(int edge = 0; edge < getEdgeCount(); edge++)
    {
        boost::add_edge(edgeMap[edge].source, edgeMap[edge].target, g);
    }
    
    return g;
}

// boost divides degree by 2, results won't match networkx python package
// http://lists.boost.org/boost-users/2008/11/42161.php
vector<double> Graph::getBetweennessCentrality()
{
    mutex.lock();
    
    boost::BoostGraph g = toBoost();
    vector<double> bc(getNodeCount());
    
    if(getNodeCount() == 0) return bc;
    brandes_betweenness_centrality(g,
            make_iterator_property_map(bc.begin(), boost::get(boost::vertex_index, g)));
            
    mutex.unlock();
    return bc;
}

vector<int> Graph::getShortestPath()
{
    mutex.lock();
    
    boost::BoostGraph g = toBoost();
    vector<int> p(getNodeCount());
    vector<int> d(getNodeCount());
    
    if(getNodeCount() == 0) return p;
    dijkstra_shortest_paths(g,
            application->getPreviousNode(),
            &p[0],
            &d[0],
            boost::get(boost::edge_weight, g),
            boost::get(boost::vertex_index, g),
            std::less<int>(), // compare
            boost::closed_plus<int>(), // combine
            std::numeric_limits<int>::max(), // max dist
            0, // zero dist
            boost::default_dijkstra_visitor());
            
    mutex.unlock();
    return p;
}

vector<int> Graph::getSpanningTree()
{
    mutex.lock();
    
    boost::BoostGraph g = toBoost();
    vector<int> p(getNodeCount());
    
    if(getNodeCount() == 0) return p;
    prim_minimum_spanning_tree(g, &p[0]);
    
    mutex.unlock();
    return p;
}

void Graph::setComponents()
{
    mutex.lock();
    
    boost::BoostGraph g = toBoost();
    vector<int> c(getNodeCount());
    
    connected_components(g, &c[0]);
    
    foreach(int node, nodes)
    {
        nodeMap[node].component = c[node];
    }
    
    mutex.unlock();
}