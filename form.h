#ifndef FORM_H
#define FORM_H

#include "typedefs.h"
#include "skiplist.h"
#include "frame.h"

typedef enum form_type {
        FORM_VALUES,
        FORM_CONS,
        FORM_STRING,
        FORM_SYMBOL,
        FORM_PACKAGE,
        FORM_CFUN,
        FORM_LAMBDA,
        FORM_LONG,
        FORM_DOUBLE,
        FORM_SKIPLIST,
        FORM_SKIPLIST_NODE,
        FORM_FRAME,
	FORM_HASHTABLE
} e_form_type;

struct values {
        e_form_type type;
        unsigned long count;
};

#define values_(v) ((u_form**) (((s_values*) v) + 1))
#define value_(v) (values_(v)[0])

struct cons {
        e_form_type type;
        u_form *car;
        u_form *cdr;
};

struct string {
        e_form_type type;
        unsigned long length;
};

#define string_str(s) ((char*)(((s_string*) s) + 1))

struct symbol {
        e_form_type type;
        s_package *package;
        s_string *string;
};

struct package {
        e_form_type type;
        s_symbol *name;
        s_skiplist *symbols;
        u_form *uses;
};

struct cfun {
        e_form_type type;
        s_symbol *name;
        f_cfun *fun;
};

struct lambda {
        e_form_type type;
        s_symbol *lambda_type;
        s_symbol *name;
        u_form *lambda_list;
        u_form *body;
        s_frame *frame;
};

struct lng {
        e_form_type type;
        long lng;
};

struct dbl {
        e_form_type type;
        double dbl;
};

#include "hashtable.h"

union form {
        e_form_type type;
        s_values values;
        s_cons cons;
        s_string string;
        s_symbol symbol;
        s_package package;
        s_cfun cfun;
        s_lambda lambda;
        s_long lng;
        s_double dbl;
        s_skiplist skiplist;
        s_skiplist_node skiplist_node;
        s_frame frame;
        s_hashtable hashtable;
};

#define null(x)    ((x) == nil())
#define valuesp(x) ((x) && (x)->type == FORM_VALUES)
#define consp(x)   ((x) && (x)->type == FORM_CONS)
#define listp(x)   ((x) && ((x)->type == FORM_CONS || x == nil()))
#define stringp(x) ((x) && (x)->type == FORM_STRING)
#define symbolp(x) ((x) && (x)->type == FORM_SYMBOL)
#define packagep(x) ((x) && (x)->type == FORM_PACKAGE)
#define functionp(x) ((x) && ((x)->type == FORM_CFUN ||         \
                              (x)->type == FORM_LAMBDA))
#define integerp(x) ((x) && (x)->type == FORM_LONG)
#define floatp(x) ((x) && (x)->type == FORM_DOUBLE)
#define numberp(x) (integerp(x) || floatp(x))
#define hashtablep(x) ((x) && (x)->type == FORM_HASHTABLE)

#define value(x) (valuesp(x) ? value_(x) : x)
#define push(place, x) place = cons(x, place)
u_form * pop (u_form **place);

s_string *  init_string (s_string *s, unsigned long length,
                         const char *str);

u_form *    nil ();
s_values *  new_values (unsigned long count);
s_cons *    new_cons (u_form *car, u_form *cdr);
s_string *  new_string (unsigned long length, const char *str);
s_symbol *  new_symbol (s_string *string);
s_symbol *  gensym (const char *name);
s_package * new_package (s_symbol *name);
s_lambda *  new_lambda (s_symbol *type, s_symbol *name,
                        u_form *lambda_list, u_form *body,
                        s_env *env);
s_long *    new_long (long lng);
s_double *  new_double (double dbl);

#endif
