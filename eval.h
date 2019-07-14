#ifndef EVAL_H
#define EVAL_H

#include "env.h"
#include "form.h"

u_form * cons_quote (u_form *form);
u_form * atom (u_form *form);
u_form * eq (u_form *a, u_form *b);
u_form * eql (u_form *a, u_form *b);
u_form * equal (u_form *a, u_form *b);
u_form * cons (u_form *car, u_form *cdr);
u_form * car (u_form *form);
u_form * cdr (u_form *form);
u_form * caar (u_form *form);
u_form * cadr (u_form *form);
u_form * cdar (u_form *form);
u_form * cddr (u_form *form);
u_form * cadar (u_form *form);
u_form * caddr (u_form *form);
u_form * find (u_form *item, u_form *list);
u_form * assoc (u_form *item, u_form *alist);
u_form * last (u_form *list);
long length (u_form *list);
u_form * reverse (u_form *list);
u_form * getf (u_form *list, u_form *indicator, u_form *def);
u_form * sort (u_form *list, s_env *env);

u_form * cons_function (u_form *form);
u_form * cons_backquote (u_form *form);
u_form * cons_comma_at (u_form *form, s_env *env);
u_form * cons_comma_dot (u_form *form, s_env *env);
u_form * cons_comma (u_form *form, s_env *env);

u_form * cspecial_quote (u_form *args, s_env *env);
u_form * cfun_atom (u_form *args, s_env *env);
u_form * cfun_eq (u_form *args, s_env *env);
u_form * cfun_eql (u_form *args, s_env *env);
u_form * cfun_equal (u_form *args, s_env *env);
u_form * cfun_cons (u_form *args, s_env *env);
u_form * cfun_car (u_form *args, s_env *env);
u_form * cfun_cdr (u_form *args, s_env *env);
u_form * cfun_caar (u_form *args, s_env *env);
u_form * cfun_cadr (u_form *args, s_env *env);
u_form * cfun_cdar (u_form *args, s_env *env);
u_form * cfun_cddr (u_form *args, s_env *env);
u_form * cfun_caaar (u_form *args, s_env *env);
u_form * cfun_caadr (u_form *args, s_env *env);
u_form * cfun_cadar (u_form *args, s_env *env);
u_form * cfun_caddr (u_form *args, s_env *env);
u_form * cfun_cdaar (u_form *args, s_env *env);
u_form * cfun_cdadr (u_form *args, s_env *env);
u_form * cfun_cddar (u_form *args, s_env *env);
u_form * cfun_cdddr (u_form *args, s_env *env);
u_form * cfun_rplaca (u_form *args, s_env *env);
u_form * cfun_rplacd (u_form *args, s_env *env);
u_form * cspecial_cond (u_form *args, s_env *env);

u_form * cspecial_case (u_form *args, s_env *env);
u_form * cspecial_do (u_form *args, s_env *env);
u_form * cspecial_when (u_form *args, s_env *env);
u_form * cspecial_unless (u_form *args, s_env *env);
u_form * cspecial_if (u_form *args, s_env *env);
u_form * cspecial_and (u_form *args, s_env *env);
u_form * cspecial_or (u_form *args, s_env *env);
u_form * cfun_not (u_form *args, s_env *env);

u_form * cfun_consp (u_form *args, s_env *env);
u_form * cfun_stringp (u_form *args, s_env *env);
u_form * cfun_symbolp (u_form *args, s_env *env);
u_form * cfun_packagep (u_form *args, s_env *env);
u_form * cfun_functionp (u_form *args, s_env *env);

u_form * cspecial_prog1 (u_form *form, s_env *env);
u_form * cspecial_progn (u_form *form, s_env *env);
u_form * cfun_make_symbol (u_form *args, s_env *env);
u_form * cfun_list (u_form *args, s_env *env);
u_form * cfun_list_star (u_form *args, s_env *env);
u_form * cfun_find (u_form *args, s_env *env);
u_form * cfun_assoc (u_form *args, s_env *env);
u_form * cfun_last (u_form *args, s_env *env);
u_form * cfun_length (u_form *args, s_env *env);
u_form * cfun_reverse (u_form *args, s_env *env);
u_form * cfun_append (u_form *args, s_env *env);
u_form * cfun_nconc (u_form *args, s_env *env);
u_form * cfun_notany (u_form *args, s_env *env);
u_form * cfun_every (u_form *args, s_env *env);
u_form * cfun_mapcar (u_form *args, s_env *env);
u_form * cfun_sort (u_form *args, s_env *env);

u_form * cspecial_let (u_form *args, s_env *env);
u_form * cspecial_let_star (u_form *args, s_env *env);
u_form * cspecial_defvar (u_form *args, s_env *env);
u_form * cspecial_defparameter (u_form *args, s_env *env);
u_form * cspecial_setq (u_form *args, s_env *env);
u_form * cfun_makunbound (u_form *args, s_env *env);

u_form * cspecial_block (u_form *args, s_env *env);
u_form * cspecial_return_from (u_form *args, s_env *env);
u_form * cspecial_return (u_form *args, s_env *env);
u_form * cspecial_tagbody (u_form *args, s_env *env);
u_form * cspecial_go (u_form *args, s_env *env);
u_form * cspecial_unwind_protect (u_form *args, s_env *env);

u_form * cspecial_lambda (u_form *args, s_env *env);
u_form * cspecial_defun (u_form *args, s_env *env);
u_form * cspecial_function (u_form *args, s_env *env);
u_form * cfun_macro_function (u_form *args, s_env *env);
u_form * cspecial_defmacro (u_form *args, s_env *env);
u_form * cfun_fmakunbound (u_form *args, s_env *env);
u_form * cspecial_labels (u_form *args, s_env *env);
u_form * cspecial_flet (u_form *args, s_env *env);

u_form * cfun_error (u_form *args, s_env *env);
u_form * cfun_gensym (u_form *args, s_env *env);

u_form * cfun_eval (u_form *args, s_env *env);
u_form * cfun_apply (u_form *args, s_env *env);
u_form * cfun_funcall (u_form *args, s_env *env);

u_form * cfun_prin1 (u_form *args, s_env *env);
u_form * cfun_print (u_form *args, s_env *env);

u_form * eval (u_form *form, s_env *env);
u_form * apply (u_form *fun, u_form *args, s_env *env);
u_form * funcall (u_form *fun, u_form *args, s_env *env);

u_form * cfun_plus (u_form *args, s_env *env);
u_form * cfun_minus (u_form *args, s_env *env);
u_form * cfun_mul (u_form *args, s_env *env);
u_form * cfun_div (u_form *args, s_env *env);

u_form * cfun_load (u_form *args, s_env *env);

u_form * cfun_find_package (u_form *args, s_env *env);
u_form * cfun_symbol_package (u_form *args, s_env *env);
u_form * cfun_find_symbol (u_form *args, s_env *env);

u_form * cfun_values (u_form *args, s_env *env);
u_form * cspecial_nth_value (u_form *args, s_env *env);
u_form * cspecial_multiple_value_bind (u_form *args, s_env *env);
u_form * cspecial_multiple_value_list (u_form *args, s_env *env);
u_form * cspecial_multiple_value_setq (u_form *args, s_env *env);

u_form * cfun_lt (u_form *args, s_env *env);
u_form * cfun_lte (u_form *args, s_env *env);
u_form * cfun_gt (u_form *args, s_env *env);
u_form * cfun_gte (u_form *args, s_env *env);

#endif
