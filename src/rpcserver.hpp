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

#ifndef __RPCSERVER_HPP
#define __RPCSERVER_HPP

#include <graph.hpp>
#include <mycelia.hpp>

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

class RpcServer
{
private:
    Mycelia* app;
    Threads::Thread* serverThread;
    std::string callbackUrl;
    std::string callbackMethod;
    xmlrpc_c::clientSimple callbackClient;
    int port;
    
public:
    RpcServer(Mycelia*);
    
    void* run();
    void callback(int);
    void setCallback(const std::string&, const std::string&);
};

class AddEdge : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    AddEdge(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int source = params.getInt(0);
        int target = params.getInt(1);
        params.verifyEnd(2);
        
        int edge = app->g->addEdge(source, target);
        
        *retval = xmlrpc_c::value_int(edge);
    }
};

class AddNode : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    AddNode(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int node = app->g->addNode();
        
        *retval = xmlrpc_c::value_int(node);
    }
};

class Center : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    Center(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        app->resetNavigationCallback(0);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class Clear : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    Clear(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        app->g->clear();
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class ClearEdges : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    ClearEdges(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        app->g->clearEdges();
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class DeleteEdge : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    DeleteEdge(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int edge = params.getInt(0);
        params.verifyEnd(1);
        
        *retval = xmlrpc_c::value_int(app->g->deleteEdge(edge));
    }
};

class DeleteNode : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    DeleteNode(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int node = params.getInt(0);
        params.verifyEnd(1);
        
        *retval = xmlrpc_c::value_int(app->g->deleteNode(node));
    }
};

class Layout : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    Layout(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        app->resetLayoutCallback(0);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class SetCallback : public xmlrpc_c::method
{
    Mycelia* app;
    RpcServer* server;
    
public:
    SetCallback(Mycelia* app, RpcServer* server) : app(app), server(server) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        server->setCallback(params.getString(0), params.getString(1));
        params.verifyEnd(2);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class SetEdgeLabel : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    SetEdgeLabel(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int edge = params.getInt(0);
        std::string label = params.getString(1);
        params.verifyEnd(2);
        
        app->g->setEdgeLabel(edge, label);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class SetEdgeWeight : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    SetEdgeWeight(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int edge = params.getInt(0);
        double weight = params.getDouble(1);
        params.verifyEnd(2);
        
        app->g->setEdgeWeight(edge, weight);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class SetLayoutType : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    SetLayoutType(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int layout = params.getInt(0);
        params.verifyEnd(1);
        
        app->setLayoutType(layout);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class SetNodeColor : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    SetNodeColor(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int node = params.getInt(0);
        double r = params.getDouble(1);
        double g = params.getDouble(2);
        double b = params.getDouble(3);
        double a = params.getDouble(4);
        params.verifyEnd(5);
        
        app->g->setColor(node, r, g, b, a);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class SetNodeLabel : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    SetNodeLabel(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int node = params.getInt(0);
        std::string label = params.getString(1);
        params.verifyEnd(2);
        
        app->g->setNodeLabel(node, label);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class SetNodeSize : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    SetNodeSize(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        int node = params.getInt(0);
        double size = params.getDouble(1);
        params.verifyEnd(2);
        
        app->g->setSize(node, size);
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class SetStatus : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    SetStatus(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        std::string status = params.getString(0);
        params.verifyEnd(1);
        
        app->setStatus(status.c_str());
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class StartLayout  : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    StartLayout(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        app->startLayout();
        
        *retval = xmlrpc_c::value_int(0);
    }
};

class StopLayout  : public xmlrpc_c::method
{
    Mycelia* app;
    
public:
    StopLayout(Mycelia* app) : app(app) {}
    
    void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
    {
        app->stopLayout();
        
        *retval = xmlrpc_c::value_int(0);
    }
};

#endif
