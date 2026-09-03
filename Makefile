CC ?= cc
AR ?= ar
LD ?= ld

CPPFLAGS := -Iinclude
CFLAGS := -std=c11 -O2 -Wall -Wextra -Werror -pedantic -ffreestanding \
          -fno-builtin -fno-stack-protector -fno-pie
ASFLAGS := -fno-pie

BUILD := build
LIBC := $(BUILD)/libc.a
CRT0 := $(BUILD)/crt0.o
LIB_OBJS := $(BUILD)/start.o $(BUILD)/syscall.o
PROGRAMS := $(BUILD)/hello $(BUILD)/runtime_probe $(BUILD)/syscall_probe

.PHONY: all clean test inspect

all: $(PROGRAMS)

$(BUILD):
	mkdir -p $(BUILD)

$(CRT0): src/crt/crt0.S | $(BUILD)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/start.o: src/crt/start.c include/mini/syscall.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/syscall.o: src/syscall/syscall.S | $(BUILD)
	$(CC) $(ASFLAGS) -c $< -o $@

$(LIBC): $(LIB_OBJS)
	$(AR) rcs $@ $^

$(BUILD)/%.o: examples/%.c include/mini/syscall.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/runtime_probe.o: tests/runtime_probe.c include/mini/syscall.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/syscall_probe.o: tests/syscall_probe.c include/mini/syscall.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/hello: $(BUILD)/hello.o $(CRT0) $(LIBC)
	$(LD) -static -e _start --build-id=none -o $@ $(BUILD)/hello.o $(CRT0) $(LIBC)

$(BUILD)/runtime_probe: $(BUILD)/runtime_probe.o $(CRT0) $(LIBC)
	$(LD) -static -e _start --build-id=none -o $@ $(BUILD)/runtime_probe.o $(CRT0) $(LIBC)

$(BUILD)/syscall_probe: $(BUILD)/syscall_probe.o $(CRT0) $(LIBC)
	$(LD) -static -e _start --build-id=none -o $@ $(BUILD)/syscall_probe.o $(CRT0) $(LIBC)

test: all
	./tests/run.sh

inspect: all
	./tests/verify-no-host-libc.sh $(PROGRAMS)

clean:
	rm -rf $(BUILD)
