
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "env.h"
#include "error.h"
#include "form.h"
#include "package.h"
#include "read.h"
#include "eval.h"
#include "print.h"

int repl (s_env *env)
{
        while (env->run) {
                s_error_handler eh;
                if (setjmp(eh.buf))
                        print_error(&eh, stderr, env);
                else {
                        u_form *r;
                        u_form *e;
                        push_error_handler(&eh, env);
                        if (!(r = read_form(env->si, env))) {
                                env->run = 0;
                                break;
                        }
                        e = eval(r, env);
                        prin1(e, stdout, env);
                        puts("");
                        pop_error_handler(env);
                }
        }
        return 0;
}

int main ()
{
        s_stream *stream;
        srand(42);
        if (isatty(0))
                stream = stream_readline("cfacts> ");
        else
                stream = stream_stdin();
        env_init(&g_env, stream);
        using_history();
        return repl(&g_env);
}
