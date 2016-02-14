#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct attr {
	char * namespace; // Может быть NULL. Для xmlns:xsi будет "xmlns".
	char * name;	  // для xmlns:xsi будет "xsi"
	char * value;
};

struct attr_list {
	struct attr attr;
	struct attr_list * next;
};

struct open_tag {
	char * name;
	struct attr_list * attrs;
	int is_oneline;
};

#define SUCCESS(x) ((x)>=0)
#define EOL (-2)
#define FAILURE (-1)

extern int _p(char * s);

extern int Ident( char * s, char ** data );
extern int String(char * s, char ** data);
extern int Attribute(char * s, struct attr * data);
extern int OpenTag(char * s, struct open_tag * data);
extern int AttrName(char * s, struct attr * attr);
extern int Text(char * s, char ** data);

#endif
