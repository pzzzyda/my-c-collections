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
	@rm -rf $(LIB_DIR) $(OBJ_DIR)


# ==== RULES FOR UNIT TESTING ================


DEBUG_CFLAGS := -g -O0 -Wall -I$(INC_DIR)

debug_compile: \
$(patsubst $(SRC_DIR)/%.c,./build/unit_test/%.o,$(SRC)) \
$(patsubst ./test/%.c,./build/unit_test/%.o,$(wildcard ./test/*.c))

clean:
	@rm -rf ./build/unit_test/

./build/unit_test/mcc_%.o: $(SRC_DIR)/mcc_%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

./build/unit_test/test_%.o: ./test/test_%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

test_%: ./build/unit_test/test_%.out
	@$<

./build/unit_test/test_deque.out: ./build/unit_test/test_deque.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_deque.o \
./build/unit_test/mcc_array.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_hash_map.out: ./build/unit_test/test_hash_map.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_vector.o \
./build/unit_test/mcc_hash_map.o ./build/unit_test/mcc_array.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_list.out: ./build/unit_test/test_list.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_list.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_rb_map.out: ./build/unit_test/test_rb_map.o \
./build/unit_test/mcc_vector.o ./build/unit_test/mcc_object.o \
./build/unit_test/mcc_rb_map.o ./build/unit_test/mcc_array.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_vector.out: ./build/unit_test/test_vector.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_vector.o \
./build/unit_test/mcc_array.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_hash_set.out: ./build/unit_test/test_hash_set.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_hash_map.o \
./build/unit_test/mcc_hash_set.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_rb_set.out: ./build/unit_test/test_rb_set.o \
./build/unit_test/mcc_object.o ./build/unit_test/mcc_rb_map.o \
./build/unit_test/mcc_rb_set.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_priority_queue.out: ./build/unit_test/mcc_array.o\
./build/unit_test/test_priority_queue.o ./build/unit_test/mcc_object.o \
./build/unit_test/mcc_vector.o ./build/unit_test/mcc_priority_queue.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@
