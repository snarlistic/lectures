/* memory - learn about memory and things like memory alignment.
 * Copyright (C) 2016-2016 Freddy Spierenburg
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ELEMENTS \
	ELEMENT(char, c) \
	ELEMENT(short, s3) \
	ELEMENT(short, s4) \
	ELEMENT(int, i2) \
	ELEMENT(int, i3) \
	ELEMENT(int, i4) \
	ELEMENT(int, i5) \
	ELEMENT(long, l3) \
	ELEMENT(short, s) \
	ELEMENT(int, i) \
	ELEMENT(short, s2) \
	ELEMENT(long, l) \
	ELEMENT(long, l2)

#define ELEMENT(type, name) type name;
struct foo {
	ELEMENTS
};
#undef ELEMENT

void maps(void) {
	char maps[32];
	snprintf(maps, sizeof(maps) - 1, "cat /proc/%d/maps", getpid());
	printf("%s:\n", maps);
	system(maps);
	printf("\n");
}

int main(void) {
	struct foo a, b[7], c, d;
	char *u = "And another one.";
	char *t = "Another string.";
	char *s = "Hello, world!";
	static int index;
	char m;
	int n;
	int o;
	char *p;

	maps();

	printf(
		"size = %ld -> ["
#define ELEMENT(type, name) " %ld "
		ELEMENTS
#undef ELEMENT
		"]\n", sizeof(a)
#define ELEMENT(type, name) , sizeof(a.name)
		ELEMENTS
#undef ELEMENT
	);
	printf(
#define ELEMENT(type, name) "%p %ld\n"
		ELEMENTS
#undef ELEMENT
		"\n"
#define ELEMENT(type, name) , &a.name, sizeof(a.name)
		ELEMENTS
#undef ELEMENT
	);
	printf("%p -> a\n", &a);
	for (index = 0; index < sizeof(b) / sizeof(b[0]); index++) {
		printf("%p -> b[%d]\n", &b[index], index);
	}
	printf("%p -> c\n", &c);
	printf("%p -> d\n", &d);
	printf("%p -> s\n", s);
	printf("%p -> t\n", t);
	printf("%p -> u\n", u);
	for (index = 0; index < s - u + strlen(s); index ++) {
		printf("%02X(%c)%c", u[index], isprint(u[index]) ? u[index] : '.', ((index + 1) % 16) ? ' ' : '\n');
	} if ((index + 1) % 16) printf("\n");
	printf("%p -> index\n", &index);
	printf("%p -> m\n", &m);
	printf("%p -> n\n", &n);
	printf("%p -> o\n", &o);

	p = malloc(16);
	strcpy(p, "boe!"); /* keep the compiler happy. */
	maps();
	free(p);

	return(0);
}
