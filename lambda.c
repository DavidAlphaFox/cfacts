
#include <stdlib.h>
#include "backtrace.h"
#include "block.h"
#include "env.h"
#include "error.h"
#include "eval.h"
#include "lambda.h"
#include "package.h"
#include "unwind_protect.h"

int check_lambda_list (u_form *lambda_list, s_env *env)
{
        while (consp(lambda_list)) {
                if (!symbolp(lambda_list->cons.car)) {
                        error(env, "malformed lambda list");
                        return 1;
                }
                lambda_list = lambda_list->cons.cdr;
        }
        return 0;
}

s_lambda * new_lambda (s_symbol *lambda_type, s_symbol *name,
                       u_form *lambda_list, u_form *body,
                       s_env *env)
{
        s_lambda *l;
        if (check_lambda_list(lambda_list, env))
                return NULL;
        if ((l = malloc(sizeof(s_lambda)))) {
                l->type = FORM_LAMBDA;
                l->lambda_type = lambda_type;
                l->name = name;
                l->lambda_list = lambda_list;
                l->body = body;
                l->frame = env->frame;
        }
        return l;
}

u_form * eval_lambda_body (s_lambda *lambda, s_frame *frame, s_env *env)
{
        s_unwind_protect up;
        u_form *f;
        if (setjmp(up.buf)) {
                pop_unwind_protect(env);
                pop_block(&nil()->symbol, env);
                pop_backtrace_frame(env);
                env->frame = frame;
                longjmp(*up.jmp, 1);
        }
        push_unwind_protect(&up, env);
        f = cspecial_progn(lambda->body, env);
        pop_unwind_protect(env);
        pop_block(&nil()->symbol, env);
        pop_backtrace_frame(env);
        env->frame = frame;
        return f;
}

u_form * funcall_lambda (s_lambda *lambda, u_form *args, s_env *env)
{
        s_frame *frame = env->frame;
        u_form *f = lambda->lambda_list;
        u_form *a = args;
        s_block block;
        int rest = 0;
        env->frame = new_frame(lambda->frame);
        push_backtrace_frame((u_form*) lambda, env->frame->variables,
                             env);
        while (consp(f) && consp(a)) {
                s_symbol *s = &f->cons.car->symbol;
                if (!symbolp(f->cons.car))
                        return error(env, "invalid lambda list");
                if (s == sym("&rest", NULL))
                        rest = 1;
                else if (rest) {
                        if (f->cons.cdr != nil())
                                return error(env, "invalid lambda "
                                             "list");
                        frame_new_variable(s, a, env->frame);
                        a = nil();
                }
                else {
                        frame_new_variable(s, a->cons.car, env->frame);
                        a = a->cons.cdr;
                }
                f = f->cons.cdr;
        }
        if (consp(f) || consp(a))
                return error(env, "invalid number of arguments");
        if (setjmp(block.buf))
                return block.return_value;
        push_block(&block, &nil()->symbol, env);
        return eval_lambda_body(lambda, frame, env);
}
