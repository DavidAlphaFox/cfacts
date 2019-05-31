#ifndef ENV_H
#define ENV_H

#include "read.h"
#include "frame.h"
#include "typedefs.h"

struct env
{
        int run;
        s_stream *si;
        s_frame *frame;
        s_frame *global_frame;
        s_skiplist *specials;
        s_block *blocks;
        s_error_handler *error_handler;
        s_tags *tags;
        s_unwind_protect *unwind_protect;
        s_backtrace_frame *backtrace;
        s_skiplist *packages;
};

s_env g_env;

u_form ** symbol_variable (s_symbol *sym, s_env *env);
u_form * symbol_function_ (s_symbol *sym, s_env *env);
u_form ** symbol_function (s_symbol *sym, s_env *env);
u_form ** symbol_macro (s_symbol *sym, s_env *env);
u_form ** symbol_special (s_symbol *sym, s_env *env);
u_form * setq (s_symbol *sym, u_form *value, s_env *env);
u_form * set_symbol_function (s_symbol *sym, u_form *value, s_env *env);
u_form * set_symbol_macro (s_symbol *sym, u_form *value, s_env *env);
u_form * defvar (s_symbol *name, u_form *value, s_env *env);
u_form * defparameter (s_symbol *name, u_form *value, s_env *env);
u_form * makunbound (s_symbol *name, s_env *env);
u_form * let (u_form *bindings, u_form *body, s_env *env);
u_form * let_star (u_form *bindings, u_form *body, s_env *env);
void env_init (s_env *env, s_stream *si);
void cfun (const char *name, f_cfun *f, s_env *env);
void cspecial (const char *name, f_cfun *f, s_env *env);
u_form * defun (s_symbol *name, u_form *lambda_list, u_form *body,
                s_env *env);
u_form * function (s_symbol *name, s_env *env);
u_form * defmacro (s_symbol *name, u_form *lambda_list, u_form *body,
                   s_env *env);
u_form * fmakunbound (s_symbol *name, s_env *env);
u_form * labels (u_form *bindings, u_form *body, s_env *env);
u_form * flet (u_form *bindings, u_form *body, s_env *env);

#endif
