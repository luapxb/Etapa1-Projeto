CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3
SRC_DIR = src
SRC = $(SRC_DIR)/grafo.cpp $(SRC_DIR)/instancia.cpp $(SRC_DIR)/solucao.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = solver

all: $(TARGET)

$(TARGET): main.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) main.cpp $(OBJ) -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean