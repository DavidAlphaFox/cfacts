
#include <assert.h>
#define __USE_MISC 1
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"
#include "form.h"
#include "form_string.h"
#include "package.h"

int snprintf(char *str, size_t size, const char *format, ...);

u_form * nil ()
{
        static u_form *nil_sym = NULL;
        if (!nil_sym)
                nil_sym = (u_form*) sym("nil", NULL);
        return nil_sym;
}

s_cons * new_cons (u_form *car, u_form *cdr)
{
        s_cons *cons = malloc(sizeof(s_cons));
        if (cons) {
                cons->type = FORM_CONS;
                cons->car = car;
                cons->cdr = cdr;
        }
        return cons;
}

s_string * init_string (s_string *str, unsigned long length,
                        const char *chars)
{
        str->type = FORM_STRING;
        str->length = length;
        strncpy(string_str(str), chars, length);
        string_str(str)[length] = 0;
        return str;
}
        
s_string * new_string (unsigned long length, const char *chars)
{
        s_string *str = malloc(sizeof(s_string) + length + 1);
        if (str)
                init_string(str, length, chars);
        return str;
}

s_symbol * new_symbol (s_string *string)
{
        s_symbol *sym = malloc(sizeof(s_symbol));
        if (sym) {
                sym->type = FORM_SYMBOL;
                sym->package = NULL;
                sym->string = string;
        }
        return sym;
}

s_symbol * gensym (const char *name)
{
        static long counter = 0;
        char c[20];
        s_string *s = new_string(strlen(name), name);
        snprintf(c, sizeof(c), "%ld", counter++);
        string_append(s, c, strlen(c));
        return new_symbol(s);
}

s_package * new_package (s_symbol *name)
{
        s_package *pkg = malloc(sizeof(s_package));
        if (pkg) {
                pkg->type = FORM_PACKAGE;
                pkg->name = name;
                pkg->symbols = new_skiplist(10, M_E);
                pkg->symbols->compare = compare_symbols;
                pkg->uses = NULL;
        }
        return pkg;
}

s_long * new_long (long lng)
{
        s_long *n = malloc(sizeof(s_long));
        if (n) {
                n->type = FORM_LONG;
                n->lng = lng;
        }
        return n;
}

s_double * new_double (double dbl)
{
        s_double *n = malloc(sizeof(s_double));
        if (n) {
                n->type = FORM_DOUBLE;
                n->dbl = dbl;
        }
        return n;
}
