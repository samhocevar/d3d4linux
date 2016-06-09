
all: simple d3d4linux-server.exe

CXXFLAGS += -I./include -std=c++11

d3d4linux-server.exe: server.cpp
	x86_64-w64-mingw32-c++ $(CXXFLAGS) $^ -o $@

simple: simple.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

