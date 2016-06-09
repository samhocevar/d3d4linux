
all: compile-hlsl d3d4linux-server.exe

check: all
	./compile-hlsl sample_ps.hlsl ps_main ps_4_0

INCLUDE = include/d3d4linux.h

CXXFLAGS += -Wall -I./include -std=c++11

d3d4linux-server.exe: server.cpp $(INCLUDE) Makefile
	x86_64-w64-mingw32-c++ $(CXXFLAGS) $(filter %.cpp, $^) -static -o $@

compile-hlsl: compile-hlsl.cpp $(INCLUDE) Makefile
	$(CXX) $(CXXFLAGS) $(filter %.cpp, $^) -o $@

