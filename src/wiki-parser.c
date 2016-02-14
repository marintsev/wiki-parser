/*
 ============================================================================
 Name        : wiki-parser.c
 Author      : Myron Marintsev
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "parser.h"

int main(void) {
	char * str = " heaver=\"hell\"o";
	int r;
	struct attr attr;
	attr.name = NULL;
	attr.value = NULL;
	char * name;
	r = Attribute(str, &attr);
	printf("ret = %d\n", r);
	if (SUCCESS(r)) {
		/*printf("name=%s\n", name);*/
		printf( "attr.name = %s\nattr.value = %s\n", attr.name, attr.value );
	}

	return 0;
}
