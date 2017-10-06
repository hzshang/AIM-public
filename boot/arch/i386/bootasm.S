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
.set CR0_PE_ON,             0x1
.set PROT_MODE_CSEG,        0x8
.set PROT_MODE_DSEG,        0x10  

.section .entry
.code16

.globl _start
_start:
        cli
        cld
        xorw    %ax,%ax             # Set %ax to zero
        movw    %ax,%ds             # -> Data Segment
        movw    %ax,%es             # -> Extra Segment
        movw    %ax,%ss             # -> Stack Segment
#Enable A20
#https://stackoverflow.com/questions/21078932/why-test-port-0x64-in-a-bootloader-before-switching-into-protected-mode
seta20.1:
        inb     $0x64,%al           # Get status
        testb   $0x2,%al            # Busy?
        jnz     seta20.1            # Yes
        movb    $0xd1,%al           # Command: Write
        outb    %al,$0x64           # output port
seta20.2:
        inb     $0x64,%al           # Get status
        testb   $0x2,%al            # Busy?
        jnz     seta20.2            # Yes
        movb    $0xdf,%al           # Enable
        outb    %al,$0x60           # A20
# Switch from real to protected mode.
#CR0: |PG|----RESERVED----|NE|ET|TS|EM|MP|PE|
        lgdt    gdtdesc
        movl    %cr0, %eax
        orl     $CR0_PE_ON, %eax
        movl    %eax, %cr0
        ljmp    $PROT_MODE_CSEG, $bootasm

.text
.code32
bootasm:
        # Set up the protected-mode data segment registers
        movw $PROT_MODE_DSEG, %ax                       # Our data segment selector
        movw %ax, %ds                                   # -> DS: Data Segment
        movw %ax, %es                                   # -> ES: Extra Segment
        movw %ax, %fs                                   # -> FS
        movw %ax, %gs                                   # -> GS
        movw %ax, %ss                                   # -> SS: Stack Segment
        movl $0x0, %ebp
        movl $_start, %esp
        call bootmain
loop:
        jmp loop


# Bootstrap GDT
# http://skelix.net/skelixos/tutorial02_zh.html
.data
.p2align 2
gdt:
        .quad   0x0000000000000000 # null descriptor
        .quad   0x00cf9a000000ffff # cs
        .quad   0x00cf92000000ffff # ds
        .quad   0x0000000000000000 # reserved for further use
        .quad   0x0000000000000000 # reserved for further use

gdtdesc:
        .word   .-gdt-1
        .long   gdt










