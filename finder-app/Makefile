SOURCE = ./writer.c
OUTPUT = writer

all:
	gcc writer.c -o writer

CROSS_COMPILE:
	aarch64-none-linux-gnu-gcc writer.c -o writer
	
clean:
	rm -rf *.o writer
