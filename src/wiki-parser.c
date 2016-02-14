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
	/*printf("Открывается тэг: ");
	 open_tag_print(tag);
	 printf("\n");*/
}

void wiki_tag_close(char * name) {
	/*printf("Закрывается тэг %s.\n", name);*/
}

void wiki_text(char * text) {
	/*printf("Текст (%ld):", text ? strlen(text) : 0);
	 text_print(text);
	 printf("\n");*/
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

#define NEED_MORE	(-3)

//   FAILURE. Ошибка
//       EOL. Конец строки
// NEED_MORE. Надо считать ещё.
//     >=0. Количество взятых символов
int consume(struct xml_walker * walker, char ** str) {
	if (*str == NULL)
		return FAILURE;

	/*printf("consuming at \"%s\"...\n", *str);*/

	if (**str == 0)
		return EOL;

	struct open_tag tag;
	int shift = 0;
	char * text = NULL;
	int r;
	r = CloseTag(*str, &text);
	if (SUCCESS(r)) {
		shift = r;
		*str += shift;
		walker->tag_close(text);
		if (text)
			free(text);
		return shift;
	} else {
		if (IS_EOL(r))
			return NEED_MORE;

		r = Text(*str, &text);
		if (SUCCESS(r)) {
			shift = r;
			*str += shift;
			walker->text(text);
			if (text)
				free(text);
			return shift;
		} else {
			if (IS_EOL(r))
				return NEED_MORE;

			r = OpenTag(*str, &tag);
			if (SUCCESS(r)) {
				shift = r;
				*str += shift;
				walker->tag_open(&tag);
				open_tag_free(&tag);
				return shift;
			} else {
				if (IS_EOL(r))
					return NEED_MORE;

				walker->error(*str);
				return FAILURE;
			}
		}
	}
}

FILE * f = NULL;
char * line = NULL;

void done(int code) {
	if (line)
		free(line);
	if (f)
		fclose(f);
	exit(code);
}

/*void atdone() {
	if (f)
		fclose(f);
	exit(255);
}*/

int main(int argc, char * argv[]) {
	/*atexit(atdone);*/

	char * fn_input = "wiki0.xml";

	if (argc == 2) {
		fn_input = argv[1];
	} else {
		fprintf(stderr, "Usage: bad.\n");
		done(5);
	}

	f = fopen(fn_input, "rt");
	if (f == NULL)
		done(4);

	int line_size = 256;
	line = malloc(line_size);
	if ( NULL == fgets(line, line_size, f))
		done(1);

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
			if ( NULL == fgets(line, line_size, f)) {
				done(2);
			}
			str = line;
		} else if (ecode == FAILURE) {
			ok = 0;
			break;
		} else if (ecode == NEED_MORE) {
			line = realloc(line, line_size * 2);
			if ( NULL == fgets(line + line_size - 1, line_size + 1, f))
				done(3);
			line_size *= 2;
			fprintf(stderr, "line_size=%d, line=%s.\n", line_size, line);
			str = line;
		} else if (ecode >= 0) {
			// всё хорошо
		} else {
			fprintf(stderr, "ecode=%d.\n", ecode);
		}
	}
	if (!ok) {
		printf("It's not OK :(");
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

	done(0);
	return 0;
}
