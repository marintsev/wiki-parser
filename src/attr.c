#include "attr.h"

void attr_init(struct attr * attr) {
	attr->name = NULL;
	attr->namespace = NULL;
	attr->value = NULL;
}

void attr_free(struct attr * attr) {
	if (attr->name)
		free(attr->name);
	if (attr->namespace)
		free(attr->namespace);
	if (attr->value)
		free(attr->value);
}
