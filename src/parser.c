#include "parser.h"

// String <- " ^["]* "
// _ <- [ \t\n]
// IdentChar <- [a-zA-Z]
// Ident <- IdentChar+
// AttrName <- Namespace ':' Ident / Ident
// OpenTag ← '<' Ident (_+ AttrName '=' String)* _* '/' '>'
// CloseTag <- '<' '/' Ident '>'
// TextChar <- [^<]
// TextPart <- TextChar
// Text <- TextPart*

/* 1. Возвращать (сдвиг или -1) или (новую позицию char * или NULL)?
 *
 *
 */

/*#define SUCCESS(x) (x !)
 #define FAIL(x) ((x) == NULL))*/

#define FAIL(x) ((x)<0)

/*  0 -- EOF
 * -1 -- Failure
 *  1 -- Success
 */
int Char(char * s, char pattern) {
	if (*s) {
		if (*s == pattern) {
			return 1;
		} else {
			return FAILURE;
		}
	} else {
		return EOL;
	}
}

int Set(char * s, int (*acceptor)(char c, void * data), void * data) {
	if (*s) {
		if (acceptor(*s, data)) {
			return 1;
		} else {
			return -2;
		}
	} else {
		return -1;
	}
}

// Запихнуть в void * значение VAL типа TYPE
/*#define SET(type,var,val) var = (type)(val)
 #define GET(type,var) (type)(var)*/

#define SET(type,var,val) *((type*)(var)) = val
#define GET(type,var) (*((type*)(var)))

int _IdentChar1(char c, void * data) {
	SET(char, data, c);
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int _IdentCharn(char c, void * data) {
	SET(char, data, c);
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
			|| (c >= '0' && c <= '9');
}

int IdentChar1(char * s, char * data) {
	return Set(s, _IdentChar1, data);
}

int IdentCharn(char * s, char * data) {
	return Set(s, _IdentCharn, data);
}

//#define ARRAY_SET(buffer,size,ptr,val) { buffer[ptr]=val; ptr++; }

/* В случае успеха выделяет память и *data указывает на неё.
 * В случае неуспеха *data == NULL, ничего освобождать не надо.
 */
int Ident(char * s, char ** data) {
	// TODO: перевести на макросы
	int buffer_size = 16;
	char * buffer = malloc(16);
	int buffer_ptr = 0;

	int ret;
	char IdentChar_data;
	ret = IdentChar1(s, &IdentChar_data);
	if (SUCCESS(ret)) {
		s += ret;
		buffer[buffer_ptr] = IdentChar_data;
		buffer_ptr++;
		while (1) {
			ret = IdentCharn(s, &IdentChar_data);
			if (SUCCESS(ret)) {
				s += ret;
				if (buffer_size == buffer_ptr) {
					buffer = realloc(buffer, buffer_size * 2);
					buffer_size *= 2;
				}
				buffer[buffer_ptr] = IdentChar_data;
				buffer_ptr++;
			} else
				break;
		}
		buffer[buffer_ptr] = 0;
		buffer_ptr++;
		// TODO: realloc
		*data = buffer;
		return buffer_ptr - 1;
	}

	free(buffer);
	*data = NULL;
	return FAILURE;
}

int __(char c, void * data) {
	return c == ' ' || c == '\t' || c == '\n';
}

int _s(char * s) {
	int r;
	int shift = 0;
	while (1) {
		r = Set(s + shift, __, NULL);
		if (SUCCESS(r)) {
			shift += r;
		} else {
			return shift;
		}
	}
}

int _p(char * s) {
	int r;
	int shift = 0;
	r = Set(s, __, NULL);
	if (SUCCESS(r)) {
		shift += r;
		while (1) {
			r = Set(s + shift, __, NULL);
			if (SUCCESS(r)) {
				shift += r;
			} else {
				return shift;
			}
		}
	} else {
		return FAILURE;
	}
}

#define ARRAY_PTR( name ) name##_ptr
#define ARRAY_SIZE( name ) name##_size
#define ARRAY_BUF( name ) name##_buf

#define ARRAY_INIT( name, defsize ) \
	int ARRAY_SIZE(name) = defsize;\
	char * ARRAY_BUF(name) = malloc(ARRAY_SIZE(name));\
	int ARRAY_PTR(name) = 0;

#define ARRAY_PUSH( name, value ) \
	if (ARRAY_SIZE(name) == ARRAY_PTR(name)) {\
		ARRAY_BUF(name) = realloc(ARRAY_BUF(name), ARRAY_SIZE(name) * 2);\
		ARRAY_SIZE(name) *= 2;\
	}\
	ARRAY_BUF(name)[ARRAY_PTR(name)] = value;\
	ARRAY_PTR(name)++;

int _StringChar(char c, void * data) {
	if (c != '"') {
		SET(char, data, c);
		return 1;
	} else
		return 0;
}

/* String <- " ^["]* "
 * Если SUCCESS, то *data -- свежесозданная строка.
 * Если FAILURE, то *data == NULL, ничего освобождать не надо.
 */
int String(char * s, char ** data) {
	ARRAY_INIT(buffer, 16);
	int r = Char(s, '"');
	int shift = 0;
	char cur_char;
	if (SUCCESS(r)) {
		shift += r;

		while (1) {
			r = Set(s + shift, _StringChar, &cur_char);
			if (SUCCESS(r)) {
				shift += r;
				ARRAY_PUSH(buffer, cur_char);
			} else {
				break;
			}
		}
		r = Char(s + shift, '"');
		if (SUCCESS(r)) {
			shift += r;
			ARRAY_PUSH(buffer, 0);
			*data = ARRAY_BUF(buffer);
			return shift;
		}
	}
	free(ARRAY_BUF(buffer));
	*data = NULL;
	return FAILURE;
}

int _TextChar(char c, void * data) {
	if (c != '<') {
		SET(char, data, c);
		return 1;
	} else
		return 0;
}

/*
 * SUCCESS: *data -- выделенная строка.
 * FAILURE: *data == NULL. Ничего освобождать не надо.
 */
int Text(char * s, char ** data) {
	ARRAY_INIT(buffer, 16);
	int r;
	int shift = 0;
	char cur_char;

	r = Set(s, _TextChar, &cur_char);
	if (SUCCESS(r)) {
		shift += r;
		ARRAY_PUSH(buffer, cur_char);
		while (1) {
			r = Set(s + shift, _TextChar, &cur_char);
			if (SUCCESS(r)) {
				shift += r;
				ARRAY_PUSH(buffer, cur_char);
			} else
				break;
		}
		ARRAY_PUSH(buffer, 0);
		*data = ARRAY_BUF(buffer);
		return shift;
	}
	free(ARRAY_BUF(buffer));
	*data = NULL;
	return FAILURE;
}

// AttrName <- Ident ':' Ident / Ident
int AttrName(char * s, struct attr * attr) {
	int r;
	r = Ident(s, &attr->namespace);
	int shift = 0;
	if (SUCCESS(r)) {
		shift += r;
		r = Char(s + shift, ':');
		if (SUCCESS(r)) {
			shift += r;
			r = Ident(s + shift, &attr->name);
			if (SUCCESS(r)) {
				shift += r;
				return shift;
			}
		} else {
			attr->name = attr->namespace;
			attr->namespace = NULL;
			return shift;
		}
	}
	if (attr->name)
		free(attr->name);
	if (attr->namespace)
		free(attr->namespace);
	return FAILURE;
}

// Attribute = _+ AttrName '=' String
int Attribute(char * s, struct attr * data) {
	int shift = 0;

	data->name = NULL;
	data->namespace = NULL;
	data->value = NULL;

	int r = _p(s);
	if (SUCCESS(r)) {
		shift += r;
		r = AttrName(s + shift, data);
		if (SUCCESS(r)) {
			shift += r;
			r = Char(s + shift, '=');
			if (SUCCESS(r)) {
				shift += r;
				r = String(s + shift, &data->value);
				if (SUCCESS(r)) {
					shift += r;
					return shift;
				}
			}
		}
	}
	attr_free(data);
	return FAILURE;
}

// OpenTag ← '<' Ident Attribute* _* '/'? '>'
int OpenTag(char * s, struct open_tag * data) {
	int shift = 0;
	int r;
	data->name = NULL;
	data->attrs = NULL;
	r = Char(s, '<');
	if (SUCCESS(r)) {
		shift += r;
		r = Ident(s + shift, &data->name);
		if (SUCCESS(r)) {
			shift += r;
			struct attr_list * head = NULL;
			while (1) {
				struct attr attr;
				r = Attribute(s + shift, &attr);
				if (SUCCESS(r)) {
					shift += r;
					struct attr_list * new = malloc(sizeof(struct attr_list));
					new->attr = attr;
					new->next = NULL;

					if (head == NULL) {
						head = new;
					} else {
						struct attr_list * p = head;
						while (p->next)
							p = p->next;
						p->next = new;
					}
				} else {
					break;
				}
			}

			r = _s(s + shift);
			if (SUCCESS(r)) {
				shift += r;
				r = Char(s + shift, '/');
				if (SUCCESS(r)) {
					shift += r;
					data->is_oneline = 1;
				} else {
					data->is_oneline = 0;
				}

				r = Char(s + shift, '>');
				if (SUCCESS(r)) {
					shift += r;
					data->attrs = head;
					return shift;
				}
			}
		}
	}
	open_tag_free(data);

	return FAILURE;
}

// CloseTag <- '<' '/' Ident '>'
int CloseTag(char * s, char ** name) {
	int shift = 0;
	int r;
	r = Char(s, '<');
	if (SUCCESS(r)) {
		shift += r;
		r = Char(s + shift, '/');
		if (SUCCESS(r)) {
			shift += r;
			r = Ident(s + shift, name);
			if (SUCCESS(r)) {
				shift += r;
				r = Char(s + shift, '>');
				if (SUCCESS(r)) {
					shift += r;
				} else {
					if (*name)
						free(*name);
				}
			}
		}
	}
	return r;
}

/*
 int Element( char * s, void ** data )
 {

 }*/
