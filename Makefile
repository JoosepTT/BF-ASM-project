CC = gcc
CFLAGS = -m32 -Wall -O2

NASM = nasm
NASMFLAGS = -f elf32

TARGET = bf_compiler

OBJECTS = main.o instructions.o mem.o

all: $(TARGET)


# linkimine:
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# kompileerimine:
main.o: main.c bf_struct.h
	$(CC) $(CFLAGS) -c $<

instructions.o: instructions.c bf_struct.h
	$(CC) $(CFLAGS) -c $<


mem.o: mem.asm
	$(NASM) $(NASMFLAGS) $< -o $@


# puhastamine:
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f *.o
	rm -f hello hello.o test_output program
	@echo "Puhastatud"


.PHONY: all clean
