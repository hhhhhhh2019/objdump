#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

#define SEC_CODE 1
#define SEC_NAME 2
#define SEC_ADDR 3

char* input_path = NULL;
char* output_path = NULL;
char* sections;
int sections_count = 0;


void print_help();


int main(int argc, char** argv) {
	if (argc == 1) {
		print_help();
		return 0;
	}

	sections = malloc(0);

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h\0") == 0 || strcmp(argv[i], "--help\0") == 0) {
			print_help();
			free(sections);
			return 0;
		} else if (strcmp(argv[i], "-o\0") == 0 || strcmp(argv[i], "-output\0") == 0) {
			output_path = argv[++i];
		} else if (strcmp(argv[i], "-c\0") == 0 || strcmp(argv[i], "--code\0") == 0) {
			sections = realloc(sections, ++sections_count);
			sections[sections_count-1] = SEC_CODE;
		} else if (strcmp(argv[i], "-n\0") == 0 || strcmp(argv[i], "--name\0") == 0) {
			sections = realloc(sections, ++sections_count);
			sections[sections_count-1] = SEC_NAME;
		} else if (strcmp(argv[i], "-d\0") == 0 || strcmp(argv[i], "--addr\0") == 0) {
			sections = realloc(sections, ++sections_count);
			sections[sections_count-1] = SEC_ADDR;
		} else {
			input_path = argv[i];
		}
	}

	ELF* elf = malloc(sizeof(ELF));
	Code_sec* code = NULL;
	Name_sec* name = NULL;
	Addr_sec* addr = NULL;

	char* output = malloc(0);
	unsigned long output_size = 0;

	FILE* f = fopen(input_path, "rb");
	if (f == NULL) {
		printf("Файл %s не найден!\n", input_path);
		free(sections);
		free(elf);
		return 2;
	}

	fread(elf, sizeof(ELF), 1, f);

	if (elf->code_entry != 0) {
		fseek(f, elf->code_entry, SEEK_SET);
		unsigned long size;
		fread(&size, sizeof(long), 1, f);
		code = malloc(size);
		fseek(f, elf->code_entry, SEEK_SET);
		fread(code, size, 1, f);
	}

	for (int i = 0; i < sections_count; i++) {
		if (sections[i] == SEC_CODE) {
			if (code == NULL) {
				printf("Секция кода не найдена!\n");
				return 1;
			} else {
				output_size += code->size - 8;
				output = realloc(output, output_size);

				for (int i = 0; i < code->size - 8; i++)
					output[output_size - code->size + 8 + i] = code->data[i];
			}
		}
	}

	/*for (int i = 0; i < output_size; i++)
		printf("%02x ", output[i]);
	putc('\n', stdout);*/

	f = fopen(output_path, "wb");
	if (f == NULL) {
		printf("Файл %s создать невозможно!\n");
		free(elf);
		if (code != NULL)
			free(code);
		if (name != NULL)
			free(name);
		if (addr != NULL)
			free(addr);
		free(sections);
		free(output);
		return 1;
	}

	fwrite(output, output_size, 1, f);

	fclose(f);

	free(elf);
	if (code != NULL)
		free(code);
	if (name != NULL)
		free(name);
	if (addr != NULL)
		free(addr);
	free(sections);
	free(output);
}


void print_help() {
	printf("Программа для вытаскивания секций из ELF файла.\n\t-h --help\tвывод этого сообщения\n\t-c --code\tвытащить секцию кода\n\t-n --name\tвытащить секцию имён\n\t-a --addr\tвытащить секцию адресов\n\t-o --output\tитоговый файл\n");
}
