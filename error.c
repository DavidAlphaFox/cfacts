
#include <stdarg.h>
#include <stdio.h>
#include "error.h"
#include "print.h"
#include "unwind_protect.h"

int vasprintf(char **strp, const char *fmt, va_list ap);

void push_error_handler (s_error_handler *eh, s_env *env)
{
        eh->string = NULL;
        eh->next = env->error_handler;
        env->error_handler = eh;
}

void pop_error_handler (s_env *env)
{
        if (env->error_handler)
                env->error_handler = env->error_handler->next;
}

u_form * error_ (s_string *str, s_env *env)
{
        s_error_handler *eh = env->error_handler;
        if (eh) {
                eh->string = str;
                eh->backtrace = env->backtrace;
                long_jump(&eh->buf, env);
        }
        fprintf(stderr, "cfacts: %s\n", string_str(str));
        return nil();
}

u_form * error (s_env *env, const char *msg, ...)
{
        va_list ap;
        char *buf = 0;
        int len;
        va_start(ap, msg);
        len = vasprintf(&buf, msg, ap);
        va_end(ap);
        return error_(new_string(len, buf), env);
}

void print_error (s_error_handler *eh, FILE *stream, s_env *env)
{
        s_backtrace_frame *bf;
        fputs("cfacts: ", stream);
        fputs(string_str(eh->string), stream);
        fputs("\nBacktrace:", stream);
        for (bf = eh->backtrace; bf; bf = bf->next) {
                print(bf->fun, stream, env);
                if (bf->vars)
                        prin1(bf->vars, stream, env);
        }
        fputs("\n", stream);
}
