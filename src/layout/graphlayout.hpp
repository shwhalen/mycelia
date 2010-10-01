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

#ifndef __GRAPHLAYOUT_HPP
#define __GRAPHLAYOUT_HPP

#include <Threads/Thread.h>

class Mycelia;

class GraphLayout
{
protected:
    Mycelia* application;
    Threads::Thread* layoutThread;
    bool stopped;
    bool dynamic;
    
public:
    GraphLayout(Mycelia* application) : application(application), dynamic(false)
    {
        layoutThread = new Threads::Thread();
    }
    
    void stop()
    {
        stopped = true;
        
        if(layoutThread && !layoutThread->isJoined())
        {
            layoutThread->join();
        }
    }
    
    virtual bool isDynamic()
    {
        return dynamic;
    }
    
    virtual void layout() = 0;
};

#endif