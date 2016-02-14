#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdio.h>

struct attr {
	char *name, *value;
};

struct open_tag {
	char * name;
	int is_oneline;
};

#define SUCCESS(x) ((x)>=0)

extern int _p(char * s);

extern int Ident( char * s, char ** data );
extern int String(char * s, char ** data);
extern int Attribute(char * s, struct attr * data);

#endif
