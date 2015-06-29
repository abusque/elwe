#include <stdlib.h>
#include <math.h>
#include "elwe.h"

struct elwe_elf *elwe_elf_create(const char *path)
{
	uint8_t e_ident[EI_NIDENT];
	struct elwe_elf *elf = malloc(sizeof(struct elwe_elf));

	elf->path = path;

	elf->file = fopen(elf->path, "rb");
	if (elf->file == NULL) {
		fprintf(stderr, "Failed to open: %s\n", elf->path);
		goto err;
	}

	fread(e_ident, 1, EI_NIDENT, elf->file);
	elf->bitness = e_ident[EI_CLASS];
	elf->endianness = e_ident[EI_DATA];
	rewind(elf->file);

	if (IS_32_BIT(elf->bitness)) {
		Elf32_Ehdr ehdr;
		fread(&ehdr, sizeof(ehdr), 1, elf->file);
		elf->phoff = ehdr.e_phoff;
		elf->phnum = ehdr.e_phnum;
		elf->phentsize = ehdr.e_phentsize;
		elf->shoff = ehdr.e_shoff;
		elf->shnum = ehdr.e_shnum;
		elf->shentsize = ehdr.e_shentsize;
	} else {
		Elf64_Ehdr ehdr;
		fread(&ehdr, sizeof(ehdr), 1, elf->file);
		elf->phoff = ehdr.e_phoff;
		elf->phnum = ehdr.e_phnum;
		elf->phentsize = ehdr.e_phentsize;
		elf->shoff = ehdr.e_shoff;
		elf->shnum = ehdr.e_shnum;
		elf->shentsize = ehdr.e_shentsize;
	}

	if (!IS_NATIVE_ENDIAN(elf->endianness)) {
		elf->phoff = bswap_64(elf->phoff);
		elf->phnum = bswap_16(elf->phnum);
		elf->phentsize = bswap_16(elf->phentsize);
		elf->shoff = bswap_64(elf->shoff);
		elf->shnum = bswap_16(elf->shnum);
		elf->shentsize = bswap_16(elf->shentsize);
	}

	return elf;

err:
	if (elf->file != NULL) {
		fclose(elf->file);
	}
	free(elf);
	return NULL;
}

void elwe_elf_destroy(struct elwe_elf *elf)
{
	fclose(elf->file);
	free(elf);
}

static
struct elwe_phdr *elwe_elf_get_phdr(struct elwe_elf *elf, uint16_t index)
{
	struct elwe_phdr *phdr = NULL;

	if (index >= elf->phnum) {
		goto err;
	}

	phdr = malloc(sizeof(struct elwe_phdr));
	fseek(elf->file, elf->phoff + index * elf->phentsize, SEEK_SET);
	if (IS_32_BIT(elf->bitness)) {
		Elf32_Phdr elf_phdr;
		fread(&elf_phdr, sizeof(elf_phdr), 1, elf->file);
		phdr->p_type = elf_phdr.p_type;
		phdr->p_memsz = elf_phdr.p_memsz;
		phdr->p_align = elf_phdr.p_align;
	} else {
		Elf64_Phdr elf_phdr;
		fread(&elf_phdr, sizeof(elf_phdr), 1, elf->file);
		phdr->p_type = elf_phdr.p_type;
		phdr->p_memsz = elf_phdr.p_memsz;
		phdr->p_align = elf_phdr.p_align;
	}

	if (!IS_NATIVE_ENDIAN(elf->endianness)) {
		phdr->p_type = bswap_32(phdr->p_type);
		phdr->p_memsz = bswap_64(phdr->p_memsz);
		phdr->p_align = bswap_64(phdr->p_align);
	}

	return phdr;
err:
	if (phdr != NULL) {
		free(phdr);
	}
	return NULL;
}

uint64_t elwe_elf_get_memsz(struct elwe_elf *elf)
{
	uint16_t i;
	uint64_t memsz = 0;

	for (i = 0; i < elf->phnum; ++i) {
		double p_memsz, segment_size;
		struct elwe_phdr *phdr = elwe_elf_get_phdr(elf, i);

		if (phdr == NULL) {
			goto err;
		}

		/* Only PT_LOAD segments contribute to memsz. Skip the
		 * rest */
		if (phdr->p_type != PT_LOAD) {
			free(phdr);
			continue;
		}

		/* Take into account the segment alignment when
		 * computing its memory size */
		p_memsz = (double) phdr->p_memsz;
		segment_size = ceil(p_memsz / phdr->p_align) * phdr->p_align;
		memsz += (uint64_t) segment_size;
		free(phdr);
	}

	return memsz;

err:
	return -1;
}
