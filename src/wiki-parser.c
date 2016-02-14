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

void xml_string_print(char * str) {
	// TODO: escape value string properly
	printf("\"%s\"", str);
}

void attr_print(struct attr * attr) {
	printf(" ");
	if (attr->namespace)
		printf("%s:", attr->namespace);
	printf("%s=", attr->name);
	xml_string_print(attr->value);
}

void open_tag_print(struct open_tag * tag) {
	printf("<%s", tag->name);
	struct attr_list * p = tag->attrs;
	while (p) {
		attr_print(&p->attr);
		p = p->next;
	}
	if (tag->is_oneline)
		printf(" />");
	else
		printf(">");
}

void text_print(char * text) {
	if (!text)
		return;
	printf("%s", text);
}

// --------------------------------

struct xml_walker {
	void (*tag_open)(struct open_tag * tag);
	void (*tag_close)(char * name);
	void (*text)(char * text);
	void (*error)(char * at);
};

void wiki_tag_open(struct open_tag * tag) {
	printf("Открывается тэг: ");
	open_tag_print(tag);
	printf("\n");
}

void wiki_tag_close(char * name) {
	printf("Закрывается тэг %s.\n", name);
}

void wiki_text(char * text) {
	printf("Текст (%ld):", text ? strlen(text) : 0);
	text_print(text);
	printf("\n");
}

void wiki_error(char * at) {
	printf("Ошибка!");
}

struct xml_walker wiki_walker = { .tag_open = wiki_tag_open, .tag_close =
		wiki_tag_close, .text = wiki_text, .error = wiki_error };

// ------------------------------

#define IF_MATCH(matcher,true_body,false_body) \
	r = matcher; \
	printf("ret = %d\n", r); \
	if( SUCCESS(r) ) \
	{ \
		shift += r; \
		true_body \
	} \
	else \
	{ \
		false_body \
	}

// FAILURE. Ошибка
//     EOL. Конец строки
//     >=0. Количество взятых символов
int consume(struct xml_walker * walker, char ** str) {
	if( *str == NULL )
		return FAILURE;

	printf( "consuming at \"%s\"...\n", *str );

	if (**str == 0)
		return EOL;

	struct open_tag tag;
	int shift = 0;
	int r = OpenTag(*str, &tag);
	char * text = NULL;
	if (SUCCESS(r)) {
		shift = r;
		*str += shift;
		walker->tag_open(&tag);
		return shift;
	} else {
		r = Text(*str, &text);
		if (SUCCESS(r)) {
			shift = r;
			*str += shift;
			walker->text(text);
			return shift;
		} else {
			walker->error(*str);
			return FAILURE;
		}
	}
}

int main(void) {
	FILE * f = fopen("wiki1.xml", "rt");

	char line[256];
	if ( NULL == fgets(line, 256, f))
		return 1;

	/*int i;
	 for (i = strlen(line); i >= 0; i--)
	 if (line[i] == '\n')
	 line[i] = 0;*/

	//char * str = "<test heaver=\"hell\" />";
	/*int r;*/
	/*struct attr attr;
	 attr.namespace = NULL;
	 attr.name = NULL;
	 attr.value = NULL;*/

	//char * str = " xmlns=\"http://www.mediawiki.org/xml/export-0.10/\"";
	//char * str = " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
	// xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.mediawiki.org/xml/export-0.10/ http://www.mediawiki.org/xml/export-0.10.xsd" version="0.10" xml:lang="ru"
	/*char * name;*/
	/*r = Attribute(str, &attr);*/
	/*int shift = 0;*/
	//printf("len=%lu", strlen(line));
	/*char * text = NULL;
	 struct open_tag tag;
	 int r = OpenTag(line, &tag);
	 printf("r=%d\n", r);
	 if (SUCCESS(r)) {
	 r = Text(line + r, &text);
	 printf("r=%d\n", r);
	 if (SUCCESS(r)) {
	 printf("len=%ld. text=\"%s\"", text ? strlen(text) : 0,
	 text ? text : "");
	 }
	 }*/

	char * str = line;
	int ok = 1;
	while (1) {
		int ecode = consume(&wiki_walker, &str);
		if (ecode == EOL) {
			if ( NULL == fgets(line, 256, f)) {
				return 2;
			}
			str = line;
		} else if (ecode == FAILURE) {
			ok = 0;
			break;
		}
	}
	if (!ok) {
		printf("Ошибка, ошибка!!!");
	}

	/*if (str == NULL)
	 break;*/
	/*}*/

	/*IF_MATCH(OpenTag(line, &tag),

	 open_tag_print(&tag)
	 ;,

	 printf("Ошибка.")
	 ;

	 )*/

	/*if (SUCCESS(r)) {
	 open_tag_print(&tag);
	 //attr_print(&attr);
	 }*/

	fclose(f);

	return 0;
}
