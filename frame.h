#ifndef FRAME_H
#define FRAME_H

#include "form.h"

struct frame
{
        u_form *variables;
        u_form *functions;
        u_form *macros;
        struct frame *parent;
};

s_frame * new_frame (s_frame *parent);
void          frame_new_variable (s_symbol *sym, u_form *value,
                                  s_frame *frame);
void          frame_new_function (s_symbol *sym, s_lambda *value,
                                  s_frame *frame);
void          frame_new_macro (s_symbol *sym, s_lambda *value,
                               s_frame *frame);
u_form **     frame_variable (s_symbol *sym, s_frame *frame);
u_form **     frame_function (s_symbol *sym, s_frame *frame);
u_form **     frame_macro (s_symbol *sym, s_frame *frame);

void          frame_destructuring_bind (u_form *lambda_list,
                                        u_form *args,
                                        s_frame *frame, s_env *env);

#endif
