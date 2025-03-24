# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fopenmp -MMD -MP
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -fopenmp

# Archivos fuente y ejecutable
SOURCES = main.cpp connect6.cpp MCTS.cpp Node.cpp
TARGET = agenteConnect6
OBJECTS = $(SOURCES:.cpp=.o)
DEPFILES = $(OBJECTS:.o=.d)

# Regla principal
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Regla para compilar archivos .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Incluir dependencias
-include $(DEPFILES)

# Limpiar archivos compilados
clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPFILES)

# Ejecutar el programa después de compilar
run: all
	./$(TARGET)
