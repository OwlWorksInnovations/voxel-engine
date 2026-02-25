CXX      := g++
CC       := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
CFLAGS   := -Iinclude
LDFLAGS  := -lGL -lglfw -ldl

SRC_DIR  := src
OBJ_DIR  := obj
BIN      := opengl-setup

CPP_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
C_SRCS   := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) \
            $(C_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN)
