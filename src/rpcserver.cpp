/*
 * Mycelia immersive 3d network visualization tool.
 * Copyright (C) 2008-2009 Sean Whalen.
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

#include <rpcserver.hpp>

using namespace std;

RpcServer::RpcServer(Mycelia* app) : app(app)
{
    port = 9876;
    serverThread = new Threads::Thread();
    serverThread->start(this, &RpcServer::run);
}

void* RpcServer::run()
{
    xmlrpc_c::registry r;
    
    r.addMethod("clear", new Clear(app));
    r.addMethod("clear_edges", new ClearEdges(app));
    r.addMethod("center", new Center(app));
    r.addMethod("delete_edge", new DeleteEdge(app));
    r.addMethod("delete_node", new DeleteNode(app));
    r.addMethod("layout", new Layout(app));
    r.addMethod("add_edge", new AddEdge(app));
    r.addMethod("add_node", new AddNode(app));
    r.addMethod("set_callback", new SetCallback(app, this));
    r.addMethod("set_edge_label", new SetEdgeLabel(app));
    r.addMethod("set_layout_type", new SetLayoutType(app));
    r.addMethod("set_vertex_color", new SetNodeColor(app));
    r.addMethod("set_vertex_label", new SetNodeLabel(app));
    r.addMethod("set_vertex_size", new SetNodeSize(app));
    r.addMethod("set_status", new SetStatus(app));
    r.addMethod("start_layout", new StartLayout(app));
    r.addMethod("stop_layout", new StopLayout(app));
    
    xmlrpc_c::serverAbyss s(r, port);
    s.run();
    
    return 0;
}

void RpcServer::callback(int node)
{
    if(!Vrui::isMaster() || callbackUrl.size() == 0 || callbackMethod.size() == 0) return;
    
    xmlrpc_c::value callbackResult;
    xmlrpc_c::paramList params;
    params.add(xmlrpc_c::value_int(node));
    
    callbackClient.call(callbackUrl, callbackMethod, params, &callbackResult);
}

void RpcServer::setCallback(const string& url, const string& method)
{
    callbackUrl = url;
    callbackMethod = method;
}