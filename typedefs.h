#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef struct values  s_values;
typedef struct cons    s_cons;
typedef struct string  s_string;
typedef struct symbol  s_symbol;
typedef struct package s_package;
typedef struct cfun    s_cfun;
typedef struct lambda  s_lambda;
typedef struct lng     s_long;
typedef struct dbl     s_double;

typedef union form u_form;

typedef struct backtrace_frame s_backtrace_frame;
typedef struct binding s_binding;
typedef struct block s_block;
typedef struct env s_env;
typedef struct error_handler s_error_handler;
typedef struct frame s_frame;
typedef struct stream s_stream;
typedef struct tags s_tags;
typedef struct unwind_protect s_unwind_protect;

typedef u_form * f_cfun (u_form *args, s_env *env);

#endif
