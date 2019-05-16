
#include <assert.h>
#include <stdlib.h>
#include "env.h"
#include "error.h"
#include "eval.h"
#include "lambda.h"
#include "package.h"
#include "unwind_protect.h"

s_env g_env;

u_form ** symbol_variable (s_symbol *sym, s_env *env)
{
        return frame_variable(sym, env->frame);
}

u_form ** symbol_function (s_symbol *sym, s_env *env)
{
        return frame_function(sym, env->frame);
}

u_form * symbol_function_ (s_symbol *sym, s_env *env)
{
        u_form **f = frame_function(sym, env->frame);
        return f ? *f : NULL;
}

u_form ** symbol_macro (s_symbol *sym, s_env *env)
{
        return frame_macro(sym, env->frame);
}

u_form ** symbol_special (s_symbol *sym, s_env *env)
{
        u_form *f;
        f = assoc((u_form*) sym, env->specials);
        if (consp(f))
                return &f->cons.cdr;
        return NULL;
}

u_form * defvar (s_symbol *name, u_form *value, s_env *env)
{
        u_form **f = frame_variable(name, env->global_frame);
        if (!f)
                frame_new_variable(name, value, env->global_frame);
        return (u_form*) name;
}

u_form * setq (s_symbol *name, u_form *value, s_env *env)
{
        u_form **f = symbol_variable(name, env);
        if (!f)
                return error(env, "unbound symbol %s", string_str(name->string));
        if (valuesp(value))
                value = value_(value);
        *f = value;
        return value;
}

u_form * defparameter (s_symbol *name, u_form *value, s_env *env)
{
        u_form **f = frame_variable(name, env->global_frame);
        if (!f)                
                frame_new_variable(name, value, env->global_frame);
        else {
                if (valuesp(value))
                        value = value_(value);
                *f = value;
        }
        return (u_form*) name;
}

u_form * let_star (u_form *bindings, u_form *body, s_env *env)
{
        s_frame *frame = env->frame;
        s_frame *f = new_frame(env->frame);
        u_form *r;
        s_unwind_protect up;
        env->frame = f;
        if (setjmp(up.buf)) {
                pop_unwind_protect(env);
                env->frame = frame;
                longjmp(*up.jmp, 1);
        }
        push_unwind_protect(&up, env);
        while (consp(bindings)) {
                u_form *name;
                u_form *value = nil();
                if (consp(bindings->cons.car)) {
                        name = bindings->cons.car->cons.car;
                        if (consp(bindings->cons.car->cons.cdr))
                                value = eval(cadar(bindings), env);
                } else
                        name = bindings->cons.car;
                if (!symbolp(name)) {
                        env->frame = frame;
                        return error(env, "invalid let* binding");
                }
                frame_new_variable(&name->symbol, value, f);
                bindings = bindings->cons.cdr;
        }
        r = cspecial_progn(body, env);
        pop_unwind_protect(env);
        env->frame = frame;
        return r;
}

u_form * let (u_form *bindings, u_form *body, s_env *env)
{
        s_frame *frame = env->frame;
        s_frame *f = new_frame(env->frame);
        u_form *r;
        while (consp(bindings)) {
                u_form *name;
                u_form *value = nil();
                if (consp(bindings->cons.car)) {
                        name = bindings->cons.car->cons.car;
                        if (consp(bindings->cons.car->cons.cdr))
                                value = eval(cadar(bindings), env);
                } else
                        name = bindings->cons.car;
                if (!symbolp(name))
                        return error(env, "invalid let binding");
                frame_new_variable(&name->symbol, value, f);
                bindings = bindings->cons.cdr;
        }
        env->frame = f;
        {
                s_unwind_protect up;
                if (setjmp(up.buf)) {
                        pop_unwind_protect(env);
                        env->frame = frame;
                        longjmp(*up.jmp, 1);
                }
                push_unwind_protect(&up, env);
                r = cspecial_progn(body, env);
                pop_unwind_protect(env);
        }
        env->frame = frame;
        return r;
}

void cfun (const char *name, f_cfun *fun, s_env *env)
{
        s_symbol *name_sym = sym(name, env);
        u_form *cf = malloc(sizeof(s_cfun));
        if (cf) {
                u_form *c;
                cf->type = FORM_CFUN;
                cf->cfun.name = name_sym;
                cf->cfun.fun = fun;
                c = cons((u_form*) name_sym, cf);
                push(env->global_frame->functions, c);
        }
}

void cspecial (const char *name, f_cfun *fun, s_env *env)
{
        s_symbol *name_sym = sym(name, env);
        u_form *cf = malloc(sizeof(s_cfun));
        if (cf) {
                u_form *c;
                cf->type = FORM_CFUN;
                cf->cfun.name = name_sym;
                cf->cfun.fun = fun;
                c = cons((u_form*) name_sym, cf);
                push(env->specials, c);
        }
}

u_form * defun (s_symbol *name, u_form *lambda_list, u_form *body,
                s_env *env)
{
        static s_symbol *function_sym = NULL;
        s_lambda *l;
        if (!function_sym)
                function_sym = sym("function", NULL);
        l = new_lambda(function_sym, name, lambda_list,
                                 body, env);
        frame_new_function(name, l, env->global_frame);
        return (u_form*) name;
}

u_form * function (s_symbol *name, s_env *env)
{
        u_form **f = frame_function(name, env->frame);
        if (f)
                return *f;
        return nil();
}

u_form * defmacro (s_symbol *name, u_form *lambda_list, u_form *body,
                   s_env *env)
{
        static s_symbol *macro_sym = NULL;
        s_lambda *l;
        if (!macro_sym)
                macro_sym = sym("macro", NULL);
        l = new_lambda(macro_sym, name, lambda_list,
                                 body, env);
        frame_new_macro(name, l, env->global_frame);
        return (u_form*) name;
}

u_form * labels (u_form *bindings, u_form *body, s_env *env)
{
        static s_symbol *labels_sym = NULL;
        s_frame *frame = env->frame;
        s_frame *f = new_frame(env->frame);
        u_form *r;
        if (!labels_sym)
                labels_sym = sym("labels", NULL);
        env->frame = f;
        while (consp(bindings)) {
                u_form *first = bindings->cons.car;
                u_form *name;
                u_form *lambda_list;
                u_form *body;
                s_lambda *l;
                if (!consp(first) || !consp(first->cons.cdr))
                        return error(env, "invalid binding for labels");
                name = first->cons.car;
                lambda_list = first->cons.cdr->cons.car;
                body = first->cons.cdr->cons.cdr;
                if (!symbolp(name))
                        return error(env, "invalid binding for labels");
                l = new_lambda(labels_sym, &name->symbol,
                               lambda_list, body, env);
                frame_new_function(&name->symbol, l, f);
                bindings = bindings->cons.cdr;
        }
        r = cspecial_progn(body, env);
        env->frame = frame;
        return r;
}

u_form * flet (u_form *bindings, u_form *body, s_env *env)
{
        s_symbol *flet_sym = NULL;
        s_frame *frame = env->frame;
        s_frame *f = new_frame(env->frame);
        u_form *r;
        if (!flet_sym)
                flet_sym = sym("flet", NULL);
        while (consp(bindings)) {
                u_form *first = bindings->cons.car;
                u_form *name;
                u_form *lambda_list;
                u_form *body;
                s_lambda *l;
                if (!consp(first) || !consp(first->cons.cdr))
                        return error(env, "invalid binding for flet");
                name = first->cons.car;
                lambda_list = first->cons.cdr->cons.car;
                body = first->cons.cdr->cons.cdr;
                if (!symbolp(name))
                        return error(env, "invalid binding for flet");
                l = new_lambda(flet_sym, &name->symbol,
                               lambda_list, body, env);
                frame_new_function(&name->symbol, l, f);
                bindings = bindings->cons.cdr;
        }
        env->frame = f;
        r = cspecial_progn(body, env);
        env->frame = frame;
        return r;
}

void env_init (s_env *env, s_stream *si)
{
        env->si = si;
        env->run = 1;
        env->frame = env->global_frame = new_frame(NULL);
        env->specials = nil();
        env->tags = NULL;
        init_packages(env);
        defparameter(sym("*package*", NULL),
                     (u_form*) common_lisp_package(), env);
        cspecial("quote",          cspecial_quote,          env);
        cfun("atom",           cfun_atom,           env);
        cfun("eq",             cfun_eq,             env);
        cfun("cons",           cfun_cons,           env);
        cfun("car",            cfun_car,            env);
        cfun("cdr",            cfun_cdr,            env);
        cfun("rplaca",         cfun_rplaca,         env);
        cfun("rplacd",         cfun_rplacd,         env);
        cspecial("cond",           cspecial_cond,           env);
        cspecial("case",           cspecial_case,           env);
        cspecial("do",             cspecial_do,             env);
        cspecial("when",           cspecial_when,           env);
        cspecial("unless",         cspecial_unless,         env);
        cspecial("if",             cspecial_if,             env);
        cspecial("and",            cspecial_and,            env);
        cspecial("or",             cspecial_or,             env);
        cfun("not",            cfun_not,            env);
        cspecial("prog1",          cspecial_prog1,          env);
        cspecial("progn",          cspecial_progn,          env);
        cfun("make-symbol",    cfun_make_symbol,    env);
        cfun("list",           cfun_list,           env);
        cfun("list*",          cfun_list_star,      env);
        cfun("find",           cfun_find,           env);
        cfun("assoc",          cfun_assoc,          env);
        cfun("last",           cfun_last,           env);
        cfun("length",         cfun_length,         env);
        cfun("append",         cfun_append,         env);
        cfun("nconc",          cfun_nconc,          env);
        cspecial("let",            cspecial_let,            env);
        cspecial("let*",           cspecial_let_star,       env);
        cspecial("defvar",         cspecial_defvar,         env);
        cspecial("defparameter",   cspecial_defparameter,   env);
        cspecial("block",          cspecial_block,          env);
        cspecial("return-from",    cspecial_return_from,    env);
        cspecial("return",         cspecial_return,         env);
        cspecial("tagbody",        cspecial_tagbody,        env);
        cspecial("go",             cspecial_go,             env);
        cspecial("unwind-protect", cspecial_unwind_protect, env);
        cspecial("setq",           cspecial_setq,           env);
        cspecial("lambda",         cspecial_lambda,         env);
        cspecial("defun",          cspecial_defun,          env);
        cspecial("function",       cspecial_function,       env);
        cfun("macro-function", cfun_macro_function, env);
        cspecial("defmacro",       cspecial_defmacro,       env);
        cspecial("labels",         cspecial_labels,         env);
        cspecial("flet",           cspecial_flet,           env);
        cfun("error",          cfun_error,          env);
        cfun("gensym",         cfun_gensym,         env);
        cfun("eval",           cfun_eval,           env);
        cfun("apply",          cfun_apply,          env);
        cfun("funcall",        cfun_funcall,        env);
        cfun("prin1",          cfun_prin1,          env);
        cfun("print",          cfun_print,          env);
        cfun("+",              cfun_plus,           env);
        cfun("-",              cfun_minus,          env);
        cfun("*",              cfun_mul,            env);
        cfun("/",              cfun_div,            env);
        cfun("load",           cfun_load,           env);
        cfun("find-package",   cfun_find_package,   env);
        cfun("symbol-package", cfun_symbol_package, env);
        cfun("find-symbol",    cfun_find_symbol,    env);
        cfun("values",         cfun_values,         env);
        cspecial("nth-value",      cspecial_nth_value,      env);
        load_file("init.lisp", env);
        load_file("backquote.lisp", env);
        defparameter(sym("*package*", NULL),
                     (u_form*) cfacts_package(), env);
}

s_frame * push_frame (s_env *env)
{
        return env->frame = new_frame(env->frame);
}

void pop_frame (s_env *env)
{
        env->frame = env->frame->parent;
}
