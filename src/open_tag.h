#ifndef OPEN_TAG_H
#define OPEN_TAG_H

#include "attr.h"

struct open_tag {
	char * name;
	struct attr_list * attrs;
	int is_oneline;
};

extern void open_tag_free(struct open_tag * tag);

#endif
