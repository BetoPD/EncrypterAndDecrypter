# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -fdiagnostics-color=always -g

# Source directory
SRC_DIR = src

# Binary directory
BIN_DIR = bin

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Output executable
TARGET = $(BIN_DIR)/app

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(SRC_FILES)
	cd $(SRC_DIR) && $(CXX) $(CXXFLAGS) *.cpp -o ../$(TARGET)  # <-- Change to src and compile

# Run the target with parameter 1 (Encrypt)
run-encrypt: $(TARGET)
	cd $(SRC_DIR) && ../$(TARGET) 1  # <-- Change to src and run

# Run the target with parameter 2 (Decrypt)
run-decrypt: $(TARGET)
	cd $(SRC_DIR) && ../$(TARGET) 2  # <-- Change to src and run

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = del /Q
    TARGET = $(BIN_DIR)/app.exe
else
    RM = rm -f
    TARGET = $(BIN_DIR)/app
endif

# Clean build artifacts
clean:
	$(RM) $(TARGET)

# Phony targets
.PHONY: all clean run-encrypt run-decrypt