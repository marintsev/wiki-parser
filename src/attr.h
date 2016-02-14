#ifndef ATTR_H
#define ATTR_H

#include "main.h"

struct attr {
	char * namespace; // Может быть NULL. Для xmlns:xsi будет "xmlns".
	char * name;	  // для xmlns:xsi будет "xsi"
	char * value;
};

struct attr_list {
	struct attr attr;
	struct attr_list * next;
};

extern void attr_init(struct attr * attr);
extern void attr_free(struct attr * attr);

#endif
