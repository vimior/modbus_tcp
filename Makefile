CC = gcc
CXX = g++
AR = ar

BUILD_DIR = ./build/
BUILD_OBJ_DIR = $(BUILD_DIR)obj/
BUILD_MAP_DIR = $(BUILD_DIR)map/
BUILD_LIB_DIR = $(BUILD_DIR)lib/
BUILD_BIN_DIR = $(BUILD_DIR)bin/
INC_DIR = ./src/
SRC_DIR = ./src/
TEST_DIR = ./tests/

C_FLAGS = -std=c++11 -fPIC -I$(INC_DIR)
LD_FLAGS = -fPIC -shared

LIB_SOURCES := $(wildcard $(SRC_DIR)*.cpp)
LIB_BASENAME = modbus_data
LIB_NAME = lib$(LIB_BASENAME).a

TEST_SOURCES := $(wildcard $(TEST_DIR)*.cpp)

LIB_OBJS = $(addprefix $(BUILD_OBJ_DIR), $(addsuffix .o, $(basename $(LIB_SOURCES))))
TEST_OBJS = $(addprefix $(BUILD_OBJ_DIR), $(addsuffix .o, $(basename $(TEST_SOURCES))))

all: $(LIB_BASENAME) test

$(LIB_BASENAME): $(LIB_OBJS)
	mkdir -p $(BUILD_LIB_DIR)
	mkdir -p $(BUILD_OBJ_DIR)
	mkdir -p $(BUILD_MAP_DIR)
	$(AR) -r $(BUILD_LIB_DIR)/$(LIB_NAME) $^
	# $(CXX) -o $(C_FLAGS) -s -fopenmp $^ -o $(BUILD_LIB_DIR)/$(LIB_NAME) $(LD_FLAGS) -Wl,-Map,$(BUILD_MAP_DIR)$@.map

test: $(TEST_OBJS)
	mkdir -p $(BUILD_BIN_DIR)
	mkdir -p $(BUILD_OBJ_DIR)
	mkdir -p $(BUILD_MAP_DIR)
	for file in $(TEST_SOURCES); do \
		make test-`echo $$file | awk -F'/' '{print $$NF}' | awk -F'.cpp' '{print $$1}'`; \
	done

test-%:
	mkdir -p $(BUILD_BIN_DIR)
	mkdir -p $(BUILD_OBJ_DIR)
	mkdir -p $(BUILD_MAP_DIR)
	$(CXX) -o $(C_FLAGS) -s -fopenmp $(addprefix $(BUILD_OBJ_DIR)tests/, $(subst test-, , $@)).o -o $(addprefix $(BUILD_BIN_DIR), $(subst test-, , $@)) -L$(BUILD_LIB_DIR) -l$(LIB_BASENAME) -Wl,-Map,$(addprefix $(BUILD_MAP_DIR), $(subst test-, , $@)).map

# $(BUILD_OBJ_DIR)%.o: %.c
# 	mkdir -p $(dir $@)
# 	$(CC) -c $(C_FLAGS) $< -o $@

$(BUILD_OBJ_DIR)%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $(C_FLAGS) $< -o $@

clean:
	rm -rf ./build
