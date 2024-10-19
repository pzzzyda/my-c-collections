# Makefile for a dynamic library

OBJ_DIR := ./build/obj
LIB_DIR := ./build/lib

OBJ := $(patsubst ./src/%.c,$(OBJ_DIR)/%.o,$(wildcard ./src/*.c))

CC := gcc
CFLAGS := -Wall -fPIC -I./include -O3

TARGET := $(LIB_DIR)/libmcc.so

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(LIB_DIR)
	@$(CC) -shared -o $@ $^

$(OBJ_DIR)/%.o: ./src/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	@mkdir -p /usr/local/include/my-c-collections
	@install -m 644 ./include/*.h /usr/local/include/my-c-collections
	@install -m 755 $(TARGET) /usr/local/lib

uninstall:
	@rm -f /usr/local/lib/libmcc.so
	@rm -rf /usr/local/include/my-c-collections
	@rm -rf $(LIB_DIR) $(OBJ_DIR)


# ==== RULES FOR UNIT TESTING ================


DEBUG_CFLAGS := -g -O0 -Wall -I./include

debug_compile: \
$(patsubst ./src/%.c,./build/unit_test/src_%.o,$(wildcard ./src/*.c)) \
$(patsubst ./test/%.c,./build/unit_test/%.o,$(wildcard ./test/*.c))

clean:
	@rm -rf ./build/unit_test/

./build/unit_test/src_%.o: ./src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

./build/unit_test/test_%.o: ./test/test_%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

test_%: ./build/unit_test/test_%.out
	@$<

./build/unit_test/test_deque.out: ./build/unit_test/test_deque.o \
./build/unit_test/src_deque.o ./build/unit_test/src_object.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_hash_map.out: ./build/unit_test/test_hash_map.o \
./build/unit_test/src_hash_map.o ./build/unit_test/src_object.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_list.out: ./build/unit_test/test_list.o \
./build/unit_test/src_list.o ./build/unit_test/src_object.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_map.out: ./build/unit_test/test_map.o \
./build/unit_test/src_map.o ./build/unit_test/src_object.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_vector.out: ./build/unit_test/test_vector.o \
./build/unit_test/src_vector.o ./build/unit_test/src_object.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_hash_set.out: ./build/unit_test/test_hash_set.o \
./build/unit_test/src_hash_set.o ./build/unit_test/src_hash_map.o \
./build/unit_test/src_object.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_set.out: ./build/unit_test/test_set.o \
./build/unit_test/src_set.o ./build/unit_test/src_map.o \
./build/unit_test/src_object.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@

./build/unit_test/test_priority_queue.out: ./build/unit_test/test_priority_queue.o \
./build/unit_test/src_priority_queue.o ./build/unit_test/src_vector.o \
./build/unit_test/src_object.o
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@
