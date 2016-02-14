#ifndef PARSER_H
#define PARSER_H

#include "main.h"

#include "attr.h"
#include "open_tag.h"

#define SUCCESS(x) ((x)>=0)
#define EOL (-2)

#define IS_EOL(x) ((x) == EOL)
#define NOT_EOL(x) ((x) != EOL)

#define FAILURE (-1)

extern int _p(char * s);

extern int Ident( char * s, char ** data );
extern int String(char * s, char ** data);
extern int Attribute(char * s, struct attr * data);
extern int OpenTag(char * s, struct open_tag * data);
extern int AttrName(char * s, struct attr * attr);
extern int Text(char * s, char ** data);
extern int CloseTag(char * s, char ** name);

#endif
