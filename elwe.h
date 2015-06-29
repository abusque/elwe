#ifndef _ELWE_H
#define _ELWE_H

#include <stdint.h>
#include <stdio.h>
#include <byteswap.h>
#include <elf.h>

/* Determine native endianness in order to convert when reading an ELF
 * file if there is a mismatch */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define NATIVE_ENDIANNESS ELFDATA2LSB
#else
#define NATIVE_ENDIANNESS ELFDATA2MSB
#endif

#define IS_32_BIT(x) (x == ELFCLASS32)
#define IS_NATIVE_ENDIAN(x) (x == NATIVE_ENDIANNESS)


struct elwe_elf {
	const char *path;
	FILE *file;
	uint8_t bitness;
	uint8_t endianness;

	uint64_t phoff;		/* Program header table offset in
				 * bytes */
	uint16_t phnum;		/* Prog header count */
	uint16_t phentsize;	/* Prog header entry size in bytes */

	uint64_t shoff;		/* Section header table offset in
				 * bytes */
	uint16_t shnum;		/* Section header count */
	uint16_t shentsize;	/* Section header entry size in
				 * bytes */
};

struct elwe_phdr {
	uint32_t p_type;
	uint64_t p_memsz;
	uint64_t p_align;
};

struct elwe_elf *elwe_elf_create(const char *path);
void elwe_elf_destroy(struct elwe_elf *elf);

uint64_t elwe_elf_get_memsz(struct elwe_elf *elf);

#endif	/* _ELWE_H */
