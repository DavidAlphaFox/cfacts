
#include <stdio.h>
#include "error.h"
#include "eval.h"
#include "lambda.h"
#include "package.h"
#include "print.h"

void prin1_cons (s_cons *cons, FILE *stream, s_env *env)
{
        u_form *quote_sym = NULL;
        if (!quote_sym)
                quote_sym = (u_form*) sym("quote", NULL);
        if (cons->car == quote_sym && cons->cdr->type == FORM_CONS &&
            cons->cdr->cons.cdr == nil()) {
                fputc('\'', stream);
                prin1(cons->cdr->cons.car, stream, env);
                return;
        }
        fputc('(', stream);
        prin1(cons->car, stream, env);
        while (cons->cdr && cons->cdr->type == FORM_CONS) {
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

void prin1 (u_form *f, FILE *stream, s_env *env)
{
        if (!f) {
                return;
        }
        switch (f->type) {
        case FORM_VALUES:
                prin1(value_(f), stream, env);
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
        default:
                error(env, "unknown form type");
        }
}

void print (u_form *f, FILE *stream, s_env *env)
{
        fputs("\n", stream);
        prin1(f, stream, env);
        fputc(' ', stream);
}
