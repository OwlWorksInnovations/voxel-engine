CXX      := g++
CC       := gcc
CXXFLAGS := -std=c++23 -Wall -Wextra -Iinclude
CFLAGS   := -Iinclude
LDFLAGS  := -lGL -lglfw -ldl
SRC_DIR  := src
OBJ_DIR  := obj
DIST_DIR := dist
BIN      := $(DIST_DIR)/voxel-engine
CPP_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
C_SRCS   := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) \
            $(C_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
SHADERS  := $(wildcard $(SRC_DIR)/*.vs $(SRC_DIR)/*.fs)
.PHONY: all clean
all: $(BIN) $(addprefix $(DIST_DIR)/, $(notdir $(SHADERS))) assets
$(DIST_DIR)/%: $(SRC_DIR)/% | $(DIST_DIR)
	cp $< $@
$(BIN): $(OBJS) | $(DIST_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(DIST_DIR):
	mkdir -p $(DIST_DIR)
.PHONY: assets
assets: | $(DIST_DIR)
	cp -r assets $(DIST_DIR)/
clean:
	rm -rf $(OBJ_DIR) $(DIST_DIR)

