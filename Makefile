CC = gcc -I ./include -c -fsanitize=address -g
LD = gcc -fsanitize=address -g


SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)



%.o: %.c
	$(CC) $< -o $@

all: $(OBJECTS)
	$(LD) $^ -o objdump
	./objdump std_bios.elf -c -o std_bios


clean:
	rm *.o objdump -rf
