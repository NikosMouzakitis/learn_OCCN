CXX = g++
CXXFLAGS = -std=c++11 -fPIC -no-pie -fpermissive
INCLUDES = -I/home/nicko/work/systemc-2.2.0/build/include -I/home/nicko/work/occn_3/include
LIBDIRS = -L/home/nicko/work/systemc-2.2.0/build/lib-linux64 -L/home/nicko/work/occn_3/lib-linux64
LIBS = -lsystemc -loccn -lpthread

SRCS = main.cpp router.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = noc_mesh2x2

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBDIRS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) noc_topology.json

.PHONY: all clean
