/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIM.
 *
 * AIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <aim/boot.h>
#include <elf.h>
#include <asm.h>
#define SECT_SIZE 512
#define ELF_BUF_SIZE 4096
static uint8_t elf_hdr_buf[ELF_BUF_SIZE];

void readseg(uint8_t* va,uint32_t count,uint32_t offset){
	uint8_t* end;
	end=va+count;
	// Round down to sector boundary. 
	va -= offset % SECT_SIZE;
	// Translate from bytes to sectors;
	offset = (offset / SECT_SIZE);
	for(; va < end; va += SECT_SIZE, offset++)
		readsect(va, offset);
}

static inline uint32_t get_lba(int n)
{
	uint32_t lba;
	for (int i = 0; i < 4; i++) {
		((uint8_t *)&lba)[i] = mbr[454 + 16 * (n - 1) + i];
	}
	return lba;
}

void bootmain(){

	elf_hdr *elf;
	elf_phdr *ph,*eph;
	void (*entry)(void);
	uint8_t* va;
	uint32_t base=get_lba(2)*SECT_SIZE;
	elf=(elf_hdr*)elf_hdr_buf;
	readseg(elf, ELF_BUF_SIZE, base);

	ph = (elf_phdr*)((uint8_t*)elf + elf->e_phoff);
	eph = ph + elf->e_phnum;

	for(; ph < eph; ph++) {
		va = (uint8_t*)(ph->p_vaddr);
		readseg(va, ph->p_filesz, base+ph->p_offset);
		if(ph->p_memsz > ph->p_filesz)
			stosb(va + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
	}
	// Call the entry point from the ELF header.
	// Does not return!
	entry=(void(*)(void))(elf->e_entry);
	entry();
}

void waitdisk(){
  while((inb(0x1F7) & 0xC0) != 0x40);
}

void readsect(uint8_t* dst,uint32_t offset){
	waitdisk();
	outb(0x1F2, 1);   // count = 1
	outb(0x1F3, offset);
	outb(0x1F4, offset >> 8);
	outb(0x1F5, offset >> 16);
	outb(0x1F6, (offset >> 24) | 0xE0);
	outb(0x1F7, 0x20);  // cmd 0x20 - read sectors
	// Read data.
	waitdisk();
	insl(0x1F0, dst, SECT_SIZE);
}



