#include <stdio.h>
#include <stdlib.h>
#include "elwe.h"

static
void usage()
{
	printf("Usage: elwe <path/to/file>\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	struct elwe_elf *elf;
	uint64_t memsz;
	size_t length, i;
	uint8_t *build_id;

	if (argc != 2) {
		usage();
	}

	elf = elwe_elf_create(argv[1]);
	if (elf == NULL) {
		exit(EXIT_FAILURE);
	}

	memsz = elwe_elf_get_memsz(elf);
	printf("%lu\n", memsz);
	build_id = elwe_elf_get_build_id(elf, &length);

	printf("Build ID: ");
	for (i = 0; i < length; ++i) {
		printf("%02x", build_id[i]);
	}
	printf("\n");

	free(build_id);
	elwe_elf_destroy(elf);

	return 0;
}
