#include <stdlib.h>
#include "eval.h"
#include "frame.h"

s_frame * new_frame (s_frame *parent)
{
        s_frame *f = malloc(sizeof(s_frame));
        if (f) {
                f->variables = nil();
                f->functions = nil();
                f->macros = nil();
                f->parent = parent;
        }
        return f;
}

void frame_new_variable (s_symbol *sym, u_form *value, s_frame *frame)
{
        u_form *a;
        if (valuesp(value))
                value = value_(value);
        a = (u_form*) new_cons((u_form*) sym, value);
        frame->variables = (u_form*) new_cons(a, frame->variables);
}

void frame_new_function (s_symbol *sym, s_lambda *value, s_frame *frame)
{
        u_form *a;
        a = (u_form*) new_cons((u_form*) sym, (u_form*) value);
        frame->functions = (u_form*) new_cons(a, frame->functions);
}

void frame_new_macro (s_symbol *sym, s_lambda *value, s_frame *frame)
{
        u_form *a = (u_form*) new_cons((u_form*) sym, (u_form*) value);
        frame->macros = (u_form*) new_cons(a, frame->macros);
}

u_form ** frame_variable (s_symbol *sym, s_frame *frame)
{
        while (frame) {
                u_form *f;
                f = assoc((u_form*) sym, frame->variables);
                if (consp(f))
                        return &f->cons.cdr;
                frame = frame->parent;
        }
        return NULL;
}

u_form ** frame_function (s_symbol *sym, s_frame *frame)
{
        while (frame) {
                u_form *f;
                f = assoc((u_form*) sym, frame->functions);
                if (consp(f))
                        return &f->cons.cdr;
                frame = frame->parent;
        }
        return NULL;
}

u_form ** frame_macro (s_symbol *sym, s_frame *frame)
{
        while (frame) {
                u_form *f;
                f = assoc((u_form*) sym, frame->macros);
                if (consp(f))
                        return &f->cons.cdr;
                frame = frame->parent;
        }
        return NULL;
}
