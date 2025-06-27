CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -Isrc
LDFLAGS =

# Define os arquivos objeto
OBJS = src/grafo.o src/instancia.o src/solucao.o src/solver.o

# Nome do executável final
TARGET = solver

all: $(TARGET)

$(TARGET): main.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $< $(OBJS) $(LDFLAGS) -o $@

# Regra para compilar cada arquivo .cpp para .o
src/grafo.o: src/grafo.cpp src/grafo.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/instancia.o: src/instancia.cpp src/instancia.h src/grafo.h src/solucao.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/solucao.o: src/solucao.cpp src/solucao.h src/grafo.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/solver.o: src/solver.cpp src/solver.h src/grafo.h src/solucao.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
