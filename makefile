# Makefile
CC = clang++
CFLAGS = -Os -flto -fuse-ld=lld
SRC = oneko.cc
OUT = oneko.exe

.PHONY: all run clean

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
	$(OUT)

clean:
	rm $(OUT) 