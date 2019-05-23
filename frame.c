#include <stdlib.h>
#include "error.h"
#include "eval.h"
#include "frame.h"
#include "package.h"

int compare_frame_bindings (void *a, void *b)
{
        s_cons *ca = (s_cons*) a;
        s_cons *cb = (s_cons*) b;
        if (a == b)
                return 0;
        if (!a)
                return -1;
        if (!b)
                return 1;
        return compare_symbols(ca->car, cb->car);
}

s_frame * new_frame (s_frame *parent)
{
        s_frame *f = malloc(sizeof(s_frame));
        if (f) {
                f->variables = NULL;
                f->functions = NULL;
                f->macros = NULL;
                f->parent = parent;
        }
        return f;
}

void frame_new_variable (s_symbol *sym, u_form *value, s_frame *frame)
{
        s_cons *binding;
        if (valuesp(value))
                value = value_(value);
        binding = new_cons((u_form*) sym, value);
        if (!frame->variables) {
                frame->variables = new_skiplist(5, 4);
                frame->variables->compare = compare_frame_bindings;
        }
        skiplist_insert(frame->variables, binding);
}

void frame_new_function (s_symbol *sym, u_form *value, s_frame *frame)
{
        s_cons *binding;
        binding = new_cons((u_form*) sym, value);
        if (!frame->functions) {
                frame->functions = new_skiplist(5, 4);
                frame->functions->compare = compare_frame_bindings;
        }
        skiplist_insert(frame->functions, binding);
}

void frame_new_macro (s_symbol *sym, s_lambda *value, s_frame *frame)
{
        s_cons *binding;
        binding = new_cons((u_form*) sym, (u_form*) value);
        if (!frame->macros) {
                frame->macros = new_skiplist(5, 4);
                frame->macros->compare = compare_frame_bindings;
        }
        skiplist_insert(frame->macros, binding);
}

u_form ** frame_variable (s_symbol *sym, s_frame *frame)
{
        s_cons search;
        search.type = FORM_CONS;
        search.car = (u_form*) sym;
        search.cdr = NULL;
        while (frame) {
                if (frame->variables) {
                        s_skiplist_node *n;
                        n = skiplist_find(frame->variables, &search);
                        if (n) {
                                s_cons *binding = (s_cons*) n->value;
                                return &binding->cdr;
                        }
                }
                frame = frame->parent;
        }
        return NULL;
}

u_form ** frame_function (s_symbol *sym, s_frame *frame)
{
        s_cons search;
        search.type = FORM_CONS;
        search.car = (u_form*) sym;
        search.cdr = NULL;
        while (frame) {
                if (frame->functions) {
                        s_skiplist_node *n;
                        n = skiplist_find(frame->functions, &search);
                        if (n) {
                                s_cons *binding = (s_cons*) n->value;
                                return &binding->cdr;
                        }
                }
                frame = frame->parent;
        }
        return NULL;
}

u_form ** frame_macro (s_symbol *sym, s_frame *frame)
{
        s_cons search;
        search.type = FORM_CONS;
        search.car = (u_form*) sym;
        search.cdr = NULL;
        while (frame) {
                if (frame->macros) {
                        s_skiplist_node *n;
                        n = skiplist_find(frame->macros, &search);
                        if (n) {
                                s_cons *binding = (s_cons*) n->value;
                                return &binding->cdr;
                        }
                }
                frame = frame->parent;
        }
        return NULL;
}

void fdb_rest (u_form *lambda_list, s_symbol *rest, u_form **args,
               s_frame *frame, s_env *env)
{
        u_form *lambda_term = lambda_list->cons.car;
        if (consp(lambda_list->cons.cdr) ||
            !symbolp(lambda_term))
                error(env, "invalid %s in lambda-list",
                      string_str(rest));
        frame_new_variable(&lambda_term->symbol, *args, frame);
        *args = nil();
}

u_form * getf (u_form *plist, u_form *indicator, u_form *def)
{
        while (consp(plist) && consp(plist->cons.cdr)) {
                if (eql(plist->cons.car, indicator))
                        return plist->cons.cdr->cons.car;
                plist = plist->cons.cdr->cons.cdr;
        }
        return def;
}

void fdb_plist (u_form *lambda_term, u_form *plist, s_frame *frame,
                s_env *env)
{
        s_symbol *name;
        u_form *def = nil();
        u_form *kw;
        u_form *value;
        if (consp(lambda_term)) {
                if (!symbolp(lambda_term->cons.car))
                        error(env, "invalid lambda-list");
                name = &lambda_term->cons.car->symbol;
                def = cadr(lambda_term);
        }
        else {
                if (!symbolp(lambda_term))
                        error(env, "invalid lambda-list");
                name = &lambda_term->symbol;
        }
        kw = (u_form*) intern(name->string, keyword_package());
        value = getf(plist, kw, def);
        frame_new_variable(name, value, frame);
}

void frame_destructuring_bind (u_form *lambda_list, u_form *args,
                               s_frame *frame, s_env *env)
{
        static u_form *rest_sym = NULL;
        static u_form *body_sym;
        static u_form *key_sym;
        s_symbol *rest = NULL;
        u_form *plist = NULL;
        if (!rest_sym) {
                rest_sym = (u_form*) sym("&rest", NULL);
                body_sym = (u_form*) sym("&body", NULL);
                key_sym = (u_form*) sym("&key", NULL);
        }
        while (consp(lambda_list) && consp(args)) {
                u_form *lambda_term = lambda_list->cons.car;
                if (rest)
                        fdb_rest(lambda_list, rest, &args, frame, env);
                else if (lambda_term == rest_sym ||
                         lambda_term == body_sym)
                        rest = &lambda_term->symbol;
                else if (plist)
                        fdb_plist(lambda_term, plist, frame, env);
                else if (lambda_term == key_sym)
                        plist = args;
                else if (consp(lambda_term)) {
                        frame_destructuring_bind(lambda_term,
                                                 args->cons.car,
                                                 frame, env);
                        args = args->cons.cdr;
                }
                else if (symbolp(lambda_list->cons.car)) {
                        frame_new_variable(&lambda_term->symbol,
                                           args->cons.car, frame);
                        args = args->cons.cdr;
                }
                else
                        error(env, "invalid lambda list term");
                lambda_list = lambda_list->cons.cdr;
        }
        if (consp(lambda_list) || (!plist && consp(args)))
                error(env, "invalid number of arguments");
}
