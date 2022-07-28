CXX ?= c++
CXXFLAGS ?= -g -Wall -O0


all: server client


server: server.cpp
	$(CXX) $(CXXFLAGS) -o $@ server.cpp

client: client.cpp
	$(CXX) $(CXXFLAGS) -o $@ client.cpp