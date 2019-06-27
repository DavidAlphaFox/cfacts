
#include <stdio.h>
#include "error.h"
#include "eval.h"
#include "lambda.h"
#include "package.h"
#include "print.h"

void prin1_cons (s_cons *cons, FILE *stream, s_env *env)
{
        static u_form *quote_sym = NULL;
        static u_form *backquote_sym;
        static u_form *comma_sym;
        static u_form *comma_atsign_sym;
        static u_form *comma_dot_sym;
        if (!quote_sym) {
                quote_sym = (u_form*) sym("quote", NULL);
                backquote_sym = (u_form*) sym("backquote", NULL);
                comma_sym = (u_form*) sym("comma", NULL);
                comma_atsign_sym = (u_form*) sym("comma-atsign", NULL);
                comma_dot_sym = (u_form*) sym("comma-dot", NULL);
        }
        if (symbolp(cons->car) && consp(cons->cdr) &&
            cons->cdr->cons.cdr == nil()) {
                if (cons->car == quote_sym) {
                        fputc('\'', stream);
                        prin1(cons->cdr->cons.car, stream, env);
                        return;
                }
                if (cons->car == backquote_sym) {
                        fputc('`', stream);
                        prin1(cons->cdr->cons.car, stream, env);
                        return;
                }
                if (cons->car == comma_sym) {
                        fputc(',', stream);
                        prin1(cons->cdr->cons.car, stream, env);
                        return;
                }
                if (cons->car == comma_atsign_sym) {
                        fputs(",@", stream);
                        prin1(cons->cdr->cons.car, stream, env);
                        return;
                }
                if (cons->car == comma_dot_sym) {
                        fputs(",.", stream);
                        prin1(cons->cdr->cons.car, stream, env);
                        return;
                }
        }
        fputc('(', stream);
        prin1(cons->car, stream, env);
        while (consp(cons->cdr)) {
                fputc(' ', stream);
                cons = &cons->cdr->cons;
                prin1(cons->car, stream, env);
        }
        if (cons->cdr != nil()) {
                fputs(" . ", stream);
                prin1(cons->cdr, stream, env);
        }
        fputc(')', stream);
}

void prin1_string (s_string *s, FILE *stream)
{
        unsigned long len = s->length;
        char *c = string_str(s);
        fputc('"', stream);
        while (len--) {
                if (*c == '"')
                        fputc('\\', stream);
                fputc(*c++, stream);
        }
        fputs("\"", stream);
}

void prin1_symbol (s_symbol *sym, FILE *stream)
{
        if (!sym->package)
                fputs("#:", stream);
        if (sym->package == keyword_package())
                fputc(':', stream);
        fputs(string_str(sym->string), stream);
}

void prin1_package (s_package *pkg, FILE *stream)
{
        fputs("#<package ", stream);
        fputs(string_str(pkg->name->string), stream);
        fputs(">", stream);
}

void prin1_cfun (s_cfun *cf, FILE *stream)
{
        fputs("#<cfun ", stream);
        fputs(string_str(cf->name->string), stream);
        fputs(">", stream);
}

void prin1_lambda (s_lambda *lambda, FILE *stream)
{
        fputs("#<", stream);
        prin1_symbol(lambda->lambda_type, stream);
        fputs(" ", stream);
        prin1_symbol(lambda->name, stream);
        fputs(">", stream);
}

void prin1_long (s_long *lng, FILE *stream)
{
        fprintf(stream, "%li", lng->lng);
}

void prin1_double (s_double *dbl, FILE *stream)
{
        fprintf(stream, "%g", dbl->dbl);
}

void prin1_skiplist (s_skiplist *sl, FILE *stream, s_env *env)
{
        s_skiplist_node *n = skiplist_node_next(sl->head, 0);
        fputc('[', stream);
        while (n) {
                prin1((u_form*) n->value, stream, env);
                n = skiplist_node_next(n, 0);
                if (n)
                        fputc(' ', stream);
        }
        fputc(']', stream);
}

void prin1_skiplist_node (s_skiplist_node *n, FILE *stream, s_env *env)
{
        unsigned long level;
        fputs("#[", stream);
        prin1(n->value, stream, env);
        for (level = 0; level < n->height; level++) {
                s_skiplist_node *next = skiplist_node_next(n, level);
                fputc(' ', stream);
                prin1(next->value, stream, env);
        }
        fputc(']', stream);
}

void prin1_frame (s_frame *f, FILE *stream, s_env *env)
{
        (void) f;
        (void) env;
        fputs("#<frame>", stream);
}

void prin1_hashtable (s_hashtable *h, FILE *stream, s_env *env)
{
        (void) env;
        fprintf(stream, "#<hashtable %li entries>", h->count);
}

void prin1 (u_form *f, FILE *stream, s_env *env)
{
        if (!f) {
                return;
        }
        if (!stream)
                stream = stdout;
        if (!env)
                env = &g_env;
        switch (f->type) {
        case FORM_VALUES:
                prin1(value_(f), stream, env);
                break;
        case FORM_CONS:
                prin1_cons(&f->cons, stream, env);
                break;
        case FORM_STRING:
                prin1_string(&f->string, stream);
                break;
        case FORM_SYMBOL:
                prin1_symbol(&f->symbol, stream);
                break;
        case FORM_PACKAGE:
                prin1_package(&f->package, stream);
                break;
        case FORM_CFUN:
                prin1_cfun(&f->cfun, stream);
                break;
        case FORM_LAMBDA:
                prin1_lambda(&f->lambda, stream);
                break;
        case FORM_LONG:
                prin1_long(&f->lng, stream);
                break;
        case FORM_DOUBLE:
                prin1_double(&f->dbl, stream);
                break;
        case FORM_SKIPLIST:
                prin1_skiplist(&f->skiplist, stream, env);
                break;
        case FORM_SKIPLIST_NODE:
                prin1_skiplist_node(&f->skiplist_node, stream, env);
                break;
        case FORM_FRAME:
                prin1_frame(&f->frame, stream, env);
                break;
        case FORM_HASHTABLE:
                prin1_hashtable(&f->hashtable, stream, env);
                break;
        }
}

void print (u_form *f, FILE *stream, s_env *env)
{
        if (!stream)
                stream = stdout;
        if (!env)
                env = &g_env;
        fputs("\n", stream);
        prin1(f, stream, env);
        fputc(' ', stream);
}
