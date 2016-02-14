/*
 ============================================================================
 Name        : wiki-parser.c
 Author      : Myron Marintsev
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

// Если надо показывать деревом открытие и закрытие тэгов:
//#define SHOW_TREE
#include "parser.h"

#include <assert.h>

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

// -----------------------------------

FILE * f = NULL;
char * line = NULL;

extern void wiki_free_stack();

void done(int code) {
	if (line)
		free(line);
	if (f)
		fclose(f);
	wiki_free_stack();
	exit(code);
}

// --------------------------------

struct xml_walker {
	void (*tag_open)(struct open_tag * tag);
	void (*tag_close)(char * name);
	void (*text)(char * text);
	void (*error)(char * at);
};

char * stack[16];
int stack_ptr = 0;
int stack_size = 16;

char title[128];

void indent(int level) {
	int i;
	for (i = 0; i < level; i++)
		putchar(' ');
}

void wiki_tag_open(struct open_tag * tag) {
#ifdef SHOW_TREE
	indent( stack_ptr );
	printf("+%s\n",tag->name);
#endif

	if (stack_ptr == stack_size) {
		fprintf(stderr, "Слишком глубоко-вложенный .xml.");
		done(7);
	}
	stack[stack_ptr] = strdup(tag->name);
	stack_ptr++;

	// Начался тэг title? Очищаем строку title.
	if (strcmp(tag->name, "title") == 0) {
		title[0] = 0;
	}
}

void wiki_tag_close(char * name) {
	again: if (stack_ptr == 0) {
		fprintf(stderr, "Stack underflow.");
		done(8);
	}
	stack_ptr--;
	if (strcmp(name, stack[stack_ptr]) == 0) {
#ifdef SHOW_TREE
		indent(stack_ptr);
		printf("-%s\n", stack[stack_ptr]);
#endif

		free(stack[stack_ptr]);
		goto done;
	} else {
		free(stack[stack_ptr]);
		goto again;
	}

	done: if (strcmp(name, "title") == 0) {
		printf("%s\n", title);
	}
	/*printf("Закрывается тэг %s.\n", name);*/
}

void wiki_free_stack() {
	stack_ptr--;
	for (; stack_ptr >= 0; stack_ptr--) {
		free(stack[stack_ptr]);
	}
}

void wiki_text(char * text) {
	if (text) {
		if (stack_ptr != 0 && strcmp(stack[stack_ptr - 1], "title") == 0) {
			if (strlen(title) + strlen(text) + 1 <= 128) {
				char * dest = strncat(title, text, 128);
				assert(dest == title);
			} else {
				fprintf(stderr, "Слишком длинно: %s||%s.\n", title, text);
				//done(9);
			}
		}

	} else
		return;
}

void wiki_error(char * at) {
	fprintf(stderr, "Ошибка! (%s)", at);
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

/*void atdone() {
 if (f)
 fclose(f);
 exit(255);
 }*/

int newlines(char * str) {
	int count = 0;
	while (*str) {
		if (*str == '\n')
			count++;
		str++;
	}
	return count;
}

int main(int argc, char * argv[]) {
	/*atexit(atdone);*/

	char * fn_input = "wiki0.xml";

	// Обработать параметры

	if (argc == 2) {
		fn_input = argv[1];
	} else if (argc == 1) {
		// fn_input уже задан
	} else {
		fprintf(stderr, "Usage: ./wiki-parser [wiki.xml].\n");
		done(5);
	}

	// Открыть файл

	f = fopen(fn_input, "rt");
	if (f == NULL)
		done(4);

	// Подготовить память для строки

	int line_size = 256;
	int offset = 0;
	int line_number = 0;
	line = malloc(line_size);
	if ( NULL == fgets(line, line_size, f))
		done(1);
	offset += strlen(line);
	line_number += newlines(line);

	// Считывать построчно и разбирать

	char * str = line;
	int ok = 1;
	while (1) {
		int ecode = consume(&wiki_walker, &str);
		if (ecode == EOL) {
			if ( NULL == fgets(line, line_size, f)) {
				done(2);
			}
			offset += strlen(line);
			line_number += newlines(line);
			str = line;
		} else if (ecode == FAILURE) {
			ok = 0;
			break;
		} else if (ecode == NEED_MORE) {
			line = realloc(line, line_size * 2);
			offset -= strlen(line);
			line_number -= newlines(line);
			if ( NULL == fgets(line + line_size - 1, line_size + 1, f))
				done(3);
			offset += strlen(line);
			line_number += newlines(line);
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
		fprintf(stderr, "It's not OK (line@%d@%d=%s)", line_number, offset,
				line);
	}

	done(0);
	return 0;
}
