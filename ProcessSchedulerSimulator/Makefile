CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

SRC_DIR = src
OBJ_DIR = obj
BIN = scheduler_sim

SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/FCFS.cpp $(SRC_DIR)/SJF.cpp $(SRC_DIR)/RoundRobin.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
