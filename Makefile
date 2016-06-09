
all: simple d3d4linux-server.exe

check: all
	./simple sample_ps.hlsl ps_main ps_4_0

INCLUDE = include/d3d4linux.h

CXXFLAGS += -I./include -std=c++11

d3d4linux-server.exe: server.cpp $(INCLUDE) Makefile
	x86_64-w64-mingw32-c++ $(CXXFLAGS) $(filter %.cpp, $^) -static -o $@

simple: simple.cpp $(INCLUDE) Makefile
	$(CXX) $(CXXFLAGS) $(filter %.cpp, $^) -o $@

