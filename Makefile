CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = parabox_gen
SRCS = main.cpp geometry.cpp
MAP_NAME = procedural_map

all: $(TARGET)

$(TARGET): $(SRCS) geometry.hpp generator.cpp
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

map: $(TARGET)
	./$(TARGET)
	wine hlcsg.exe -nowadtextures $(MAP_NAME)
	wine hlbsp.exe $(MAP_NAME)
	wine hlvis.exe $(MAP_NAME)
	wine hlrad.exe $(MAP_NAME)

clean:
	rm -f $(TARGET) *.p0 *.p1 *.p2 *.p3 *.prt *.line *.log *.err *.ent *.map
