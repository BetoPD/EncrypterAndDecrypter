# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -fdiagnostics-color=always -g

# Source and Binary directories
SRC_DIR = src
BIN_DIR = bin

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Output executable (por defecto para Linux)
TARGET = $(BIN_DIR)/app

# Detect OS: si es Windows, modifica TARGET y RM
ifeq ($(OS),Windows_NT)
    RM = del /Q
    TARGET := $(BIN_DIR)/app.exe
else
    RM = rm -f
endif

# Default target
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) $(SRC_FILES) -o $(TARGET)

# Ejecutar la aplicación con parámetro 1 (Encrypt)
run-encrypt: $(TARGET)
	$(TARGET) 1

# Ejecutar la aplicación con parámetro 2 (Decrypt)
run-decrypt: $(TARGET)
	$(TARGET) 2

# Limpiar archivos generados
clean:
	$(RM) $(TARGET) $(BIN_DIR)/*.o

# Declarar targets que no son archivos
.PHONY: all clean run-encrypt run-decrypt
