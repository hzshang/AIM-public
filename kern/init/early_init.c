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
#include <aim/init.h>

//copy from http://arjunsreedharan.org/post/82710718100/kernel-101-lets-write-a-kernel
void cleanScreen(){
	const char *str = "Hello World!";
	char *vidptr = (char*)0xb8000;
	unsigned int i = 0;
	unsigned int j = 0;

	/* loops clears the screen*/
	while(j < 80 * 25 * 2) {
		vidptr[j] = ' ';
		vidptr[j+1] = 0x07; 		
		j = j + 2;
	}
	j = 0;

	/* writes the string to video memory */
	while(str[j] != '\0') {
		vidptr[i] = str[j];
		vidptr[i+1] = 0x07;
		++j;
		i = i + 2;
	}
	return;
}


__noreturn
void master_early_init(void)
{
	arch_early_init();
	cleanScreen();
	while(1);
}

