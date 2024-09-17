# Makefile for a dynamic library

SRC_DIR := ./src
OBJ_DIR := ./build/obj
LIB_DIR := ./build/lib
INC_DIR := ./include

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

CC := gcc
CFLAGS := -Wall -fPIC -I$(INC_DIR)

TARGET := $(LIB_DIR)/libmcc.so

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(LIB_DIR)
	@$(CC) -shared -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	@mkdir -p /usr/local/include/my-c-collections
	@install -m 644 $(INC_DIR)/*.h /usr/local/include/my-c-collections
	@install -m 755 $(TARGET) /usr/local/lib

uninstall:
	@rm -f /usr/local/lib/libmcc.so
	@rm -rf /usr/local/include/my-c-collections


# ==== RULES FOR UNIT TESTING ================


DEBUG_CFLAGS := -g -O0 -Wall -I$(INC_DIR)

clean:
	@rm -rf $(LIB_DIR) $(OBJ_DIR) ./build/unit_test

./build/unit_test/mcc_%.o: $(SRC_DIR)/mcc_%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

./build/unit_test/test_%.o: ./test/test_%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

test_%: ./build/unit_test/test_%.out
	@$<

./build/unit_test/test_deque.out: ./build/unit_test/test_deque.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_deque.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_hash_map.out: ./build/unit_test/test_hash_map.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_vector.o \
./build/unit_test/mcc_hash_map.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_list.out: ./build/unit_test/test_list.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_list.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_map.out: ./build/unit_test/test_map.o \
./build/unit_test/mcc_vector.o ./build/unit_test/mcc_object.o \
./build/unit_test/mcc_map.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_vector.out: ./build/unit_test/test_vector.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_vector.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@
