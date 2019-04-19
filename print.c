
#include <stdio.h>
#include "print.h"

void print_nil ()
{
        fputs("nil", stdout);
}

void print_cons (s_cons *cons)
{
        putchar('(');
        print(cons->car);
        while (cons->cdr && cons->cdr->type == FORM_CONS) {
                putchar(' ');
                cons = &cons->cdr->cons;
                print(cons->car);
        }
        if (cons->cdr && cons->cdr->type != FORM_NULL) {
                fputs(" . ", stdout);
                print(cons->cdr);
        }
        fputs(")", stdout);
}

void print_string (s_string *s)
{
        unsigned long len = s->length;
        char *c = s->str;
        putchar('"');
        while (len--) {
                if (*c == '"')
                        putchar('\\');
                putchar(*c++);
        }
        fputs("\"", stdout);
}

void print_symbol (s_symbol *sym)
{
        fputs(sym->string->str, stdout);
}

void print_package (s_package *pkg)
{
        fputs("#<package ", stdout);
        fputs(pkg->name->string->str, stdout);
        fputs(">", stdout);
}

void print_error (const char *msg)
{
        fprintf(stderr, "cfacts: %s\n", msg);
}

void print (u_form *f)
{
        if (!f) {
                print_nil();
                return;
        }
        switch (f->type) {
        case FORM_NULL:
                print_nil();
                break;
        case FORM_CONS:
                print_cons((s_cons*)f);
                break;
        case FORM_STRING:
                print_string((s_string*)f);
                break;
        case FORM_SYMBOL:
                print_symbol((s_symbol*)f);
                break;
        case FORM_PACKAGE:
                print_package((s_package*)f);
                break;
        default:
                print_error("unknown form type");
        }
}
