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

void xml_string_print( char * str )
{
	// TODO: escape value string properly
	printf("\"%s\"", str );
}

void attr_print(struct attr * attr) {

	printf(" %s=", attr->name );
	xml_string_print( attr->value );
}

void open_tag_print(struct open_tag * tag) {
	printf("<%s", tag->name);
	struct attr_list * p = tag->attrs;
	while (p) {
		attr_print( &p->attr );
		p = p->next;
	}
	if( tag->is_oneline )
		printf(" />");
	else
		printf(">");
}



int main(void) {
	char * str = "<test heaver=\"hell\" />";
	int r;
	/*struct attr attr;
	 attr.name = NULL;
	 attr.value = NULL;*/

	struct open_tag tag;

	/*char * name;*/
	r = OpenTag(str, &tag);
	printf("ret = %d\n", r);
	if (SUCCESS(r)) {
		open_tag_print(&tag);

	}

	return 0;
}
