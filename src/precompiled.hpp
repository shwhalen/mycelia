#ifndef __PRECOMPILED_HPP
#define __PRECOMPILED_HPP

// stl
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <limits>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <tr1/unordered_map>

// syscalls
#include <sys/wait.h>

// glu
#include <GL/glu.h>

// vrui
#include <Geometry/Ray.h>
#include <GL/GLContextData.h>
#include <GL/GLGeometryWrappers.h>
#include <GL/GLMaterial.h>
#include <GL/GLModels.h>
#include <GL/GLObject.h>
#include <GL/GLTransformationWrappers.h>
#include <GLMotif/Button.h>
#include <GLMotif/CascadeButton.h>
#include <GLMotif/FileSelectionDialog.h>
#include <GLMotif/Menu.h>
#include <GLMotif/PopupMenu.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RadioBox.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/Slider.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/SubMenu.h>
#include <GLMotif/TextField.h>
#include <GLMotif/ToggleButton.h>
#include <GLMotif/WidgetManager.h>
#include <Images/ReadImageFile.h>
#include <Images/RGBImage.h>
#include <Threads/Mutex.h>
#include <Threads/Thread.h>
#include <Vrui/Application.h>
#include <Vrui/Tools/Tool.h>
#include <Vrui/VRScreen.h>
#include <Vrui/Vrui.h>

// boost
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/betweenness_centrality.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>

// font rendering
#include <FTGL/ftgl.h>

#endif
