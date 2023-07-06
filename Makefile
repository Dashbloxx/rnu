CC = i686-elf-gcc
AS = i686-elf-as
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -Tlinker.ld -ffreestanding -O2 -nostdlib

SRCS := $(wildcard kernel/*.c)
ASMS := $(wildcard kernel/*.S)
OBJS := $(ASMS:.S=.o) $(SRCS:.c=.o)

all: kernel.bin

kernel.bin: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.S
	$(AS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f kernel/*.o kernel.bin