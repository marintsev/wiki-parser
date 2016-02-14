#include "open_tag.h"

void open_tag_free(struct open_tag * tag) {
	if (tag->name)
		free(tag->name);
	if (tag->attrs) {
		struct attr_list *q, *p = tag->attrs;
		while (p) {
			attr_free(&p->attr);
			q = p;
			free(q);
			p = p->next;
		}
	}
}
