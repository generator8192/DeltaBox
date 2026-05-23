# Настройки компилятора
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Имена файлов
TARGET = DeltaBox
SRCS = main.cpp geometry.cpp
MAP_NAME = procedural_map

# Главная цель: собрать программу
all: $(TARGET)

$(TARGET): $(SRCS) geometry.hpp generator.cpp perlin.hpp
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) -lcurl

# Цель для сборки карты
map: $(TARGET)
	./$(TARGET)
	wine hlcsg.exe -nowadtextures $(MAP_NAME)
	wine hlbsp.exe $(MAP_NAME)
	wine hlvis.exe $(MAP_NAME)
	wine hlrad.exe $(MAP_NAME)

# Очистка мусора
clean:
	rm -f $(TARGET) *.p0 *.p1 *.p2 *.p3 *.prt *.line *.log *.err *.ent *.map *.bsp
