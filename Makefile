VRUIDIR = $(HOME)/Vrui-1.0
include $(VRUIDIR)/etc/Vrui.makeinclude

BASEDIR = /opt/local
CC = $(BASEDIR)/bin/g++
CFLAGS = -I $(BASEDIR)/include -I $(shell pwd)/src -Wno-deprecated -Wall -g -O2
LINKFLAGS = -L$(BASEDIR)/lib -lGLU

VPATH = src:src/generators:src/layout:src/parsers:src/tools:src/windows
OBJS = 	barabasigenerator.o erdosgenerator.o wattsgenerator.o \
	arflayout.o arfwindow.o edgebundler.o frlayout.o \
	chacoparser.o dotparser.o gmlparser.o xmlparser.o \
	graphbuilder.o nodeselector.o \
	attributewindow.o imagewindow.o \
	graph.o mycelia.o vruihelp.o rpcserver.o

# boost
CFLAGS += -I $(BASEDIR)/include/boost
LINKFLAGS += -lboost_system-mt -lboost_regex-mt

# ftgl font renderer
CFLAGS += -I $(BASEDIR)/include/freetype2
LINKFLAGS += -lftgl -lfreetype

# rpc server
CFLAGS += -D__RPCSERVER__
LINKFLAGS += -lxmlrpc_server_abyss++ -lxmlrpc_server++ -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss \
-lxmlrpc_client++ -lxmlrpc_client -lxmlrpc++ -lxmlrpc -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok -lcurl

# nvidia cuda sdk
#CUDA_TOOLKIT_DIR = /usr/local/cuda
#CUDA_SDK_DIR = "/Developer/GPU Computing/C/common/inc"
#ifneq ($(wildcard $(CUDA_TOOLKIT_DIR)),)
#	NVCC = $(CUDA_TOOLKIT_DIR)/bin/nvcc
#	NVCC_CFLAGS = -I $(CUDA_SDK_DIR) -O2
#	CFLAGS += -I $(CUDA_TOOLKIT_DIR)/include -D__CUDA__
#	LINKFLAGS += -L$(CUDA_TOOLKIT_DIR)/lib -lcuda -lcudart
#	OBJS += gpulayout.o
#endif

.SUFFIXES: .cpp .cu .o

.cpp.o:
	@echo Compiling $<...
	@$(CC) $(VRUI_CFLAGS) $(CFLAGS) -c $<

.cu.o:
	@echo Compiling $<...
	@$(NVCC) $(NVCC_CFLAGS) -c $<

all: mycelia

mycelia: $(OBJS)
	@$(CC) -o $@ $(VRUI_LINKFLAGS) $(LINKFLAGS) $+

pch: src/precompiled.hpp
	@$(CC) -x c++-header $(VRUI_CFLAGS) $(CFLAGS) $<

clean:
	rm -f $(OBJS)
	rm -f src/precompiled.hpp.gch
