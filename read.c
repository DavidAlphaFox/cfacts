
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define __USE_XOPEN2K8 1
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "compare.h"
#include "env.h"
#include "error.h"
#include "eval.h"
#include "form.h"
#include "package.h"
#include "read.h"
#include "form_string.h"
#include "unwind_protect.h"

s_stream * stream_readline (const char *prompt)
{
        s_stream *stream = malloc(sizeof(s_stream));
        if (stream) {
                stream->s = NULL;
                stream->n = 0;
                stream->start = stream->end = 0;
                stream->in_cons = 0;
                stream->fp = NULL;
                stream->prompt = prompt;
                stream->file_name = "readline";
                stream->line = 0;
        }
        return stream;
}

s_stream * stream_stdin ()
{
        s_stream *stream = malloc(sizeof(s_stream));
        if (stream) {
                stream->s = NULL;
                stream->n = 0;
                stream->start = stream->end = 0;
                stream->in_cons = 0;
                stream->fp = stdin;
                stream->prompt = NULL;
                stream->file_name = "stdin";
                stream->line = 0;
        }
        return stream;
}

s_stream * stream_open (const char *file_name, s_env *env)
{
        s_stream *stream = malloc(sizeof(s_stream));
        if (stream) {
                stream->s = NULL;
                stream->n = 0;
                stream->start = stream->end = 0;
                stream->in_cons = 0;
                if (!(stream->fp = fopen(file_name, "r")))
                        error(env, "open %s: %s", file_name,
                              strerror(errno));
                stream->prompt = NULL;
                stream->file_name = file_name;
                stream->line = 0;
        }
        return stream;
}

void stream_close (s_stream *stream)
{
        if (stream && stream->fp)
                fclose(stream->fp);
}

int refill (s_stream *stream)
{
        while (!stream->s || stream->start == stream->end) {
                stream->start = 0;
                if (stream->fp) {
                        ssize_t end = getline(&stream->s, &stream->n,
                                              stream->fp);
                        if (end < 0)
                                return -1;
                        stream->end = end - 1;
                        stream->s[stream->end] = 0;
                }
                else {
                        if (!(stream->s = readline(stream->prompt)))
                                return -1;
                        add_history(stream->s);
                        stream->end = strlen(stream->s);
                }
                stream->line++;
        }
        return 0;
}

int read_char (s_stream *stream)
{
        if (refill(stream))
                return -1;
        return stream->s[stream->start++];
}

int peek_char (s_stream *stream)
{
        if (refill(stream))
                return -1;
        return stream->s[stream->start];
}

int read_spaces (s_stream *stream)
{
        int c;
        while ((c = peek_char(stream)) >= 0)
                switch (c) {
                case ' ': case '\t': case '\r': case '\n':
                        read_char(stream);
                        continue;
                default:
                        return 0;
                }
        return -1;
}

u_form * read_cons (s_stream *stream, s_env *env)
{
        int c = peek_char(stream);
        if (c == '(') {
                u_form *head = NULL;
                u_form **tail = &head;
                read_char(stream);
                stream->in_cons++;
                while (!read_spaces(stream) && (c = peek_char(stream)) >= 0) {
                        if (c == ')') {
                                read_char(stream);
                                *tail = nil();
                                return head;
                        }
                        if (c == '.') {
                                if (!head)
                                        return error(env, "unexpect"
                                                     "ed dot");
                                else {
                                        read_char(stream);
                                        if (!(*tail = read_form(stream, env)) ||
                                            read_spaces(stream) ||
                                            (c = read_char(stream)) < 0)
                                                return NULL;
                                        if (c == ')')
                                                return head;
                                        return error(env, "malformed "
                                                     "dotted list");
                                }
                        }
                        if (!(*tail = read_form(stream, env)))
                                return NULL;
                        *tail = (u_form*) new_cons(*tail, NULL);
                        tail = &(*tail)->cons.cdr;
                }
        }
        return NULL;
}

u_form * read_skiplist (s_stream *stream, s_env *env)
{
        int c = peek_char(stream);
        if (c == '[') {
                s_skiplist *sl = new_skiplist(5, 4);
                sl->compare = compare_equal;
                read_char(stream);
                while (!read_spaces(stream) &&
                       (c = peek_char(stream)) >= 0) {
                        if (c == ']') {
                                read_char(stream);
                                return (u_form*) sl;
                        }
                        if (c == ')')
                                error(env, "unexpected close "
                                      "parenthesis");
                        skiplist_insert(sl, read_form(stream, env));
                }
        }
        return NULL;
}

u_form * read_string (s_stream *stream)
{
        u_form *f = NULL;
        unsigned long c;
        if (peek_char(stream) == '"') {
                stream->start++;
                while (!refill(stream)) {
                        for (c = stream->start; c < stream->end &&
                                     stream->s[c] != '"'; c++)
                                ;
                        if (f)
                                f = (u_form*)
                                        string_append((s_string*) f,
                                                      stream->s + stream->start,
                                                      c - stream->start);
                        else
                                f = (u_form*)new_string(c - stream->start,
                                                        stream->s + stream->start);
                        if (stream->s[c] == '"') {
                                stream->start = c + 1;
                                return f;
                        }
                        f = (u_form*) string_append((s_string*) f, "\n",
                                                    1);
                        stream->start = c;
                }
        }
        return NULL;
}

int endchar (int c)
{
        return c == '(' || c == ')' || c == '"' || c == ' ' ||
                c == '\t' || c == '\r' || c == '\n' || c == '\'' ||
                c == '[' || c == ']';
}

u_form * read_uninterned_symbol (s_stream *stream)
{
        s_string *s;
        u_form *f;
        unsigned long i = stream->start;
        while (i < stream->end && !endchar(stream->s[i]))
                i++;
        s = new_string(i - stream->start, stream->s + stream->start);
        f = (u_form*) new_symbol(s);
        stream->start = i;
        return f;
}

u_form * read_sharp (s_stream *stream, s_env *env)
{
        if (peek_char(stream) == '#') {
                int c;
                read_char(stream);
                c = peek_char(stream);
                switch (c) {
                case '\'':
                        read_char(stream);
                        return cons_function(read_form(stream, env));
                        break;
                case ':':
                        read_char(stream);
                        return read_uninterned_symbol(stream);
                }
                return error(env, "undefined # macro character %c", c);
        }
        return NULL;
}

u_form * read_number (s_stream *stream)
{
        u_form *f = NULL;
        unsigned long i = stream->start;
        char *end;
        unsigned long j = 0;
        while (i < stream->end && '0' <= stream->s[i] && stream->s[i] <= '9')
                i++;
        if (i < stream->end && stream->s[i] == '.') {
                f = (u_form*) new_double(strtod(stream->s + stream->start,
                                                &end));
                j = end - (stream->s + stream->start);
        }
        else if (i > stream->start) {
                f = (u_form*) new_long(strtol(stream->s + stream->start,
                                              &end, 10));
                j = end - (stream->s + stream->start);
        }
        if (j > 0 && (stream->start + j >= stream->end || endchar(stream->s[stream->start + j]))) {
                stream->start += j;
                return f;
        }
        return NULL;
}

u_form * read_symbol (s_stream *stream, s_env *env)
{
        s_package *pkg = NULL;
        s_string *s;
        s_symbol *pkg_name;
        u_form *f;
        unsigned long i = stream->start;
        while (i < stream->end && !endchar(stream->s[i])
               && stream->s[i] != ':')
                i++;
        if (i == stream->start && stream->s[i] == ':') {
                pkg = keyword_package();
                stream->start = ++i;
        }
        else if (stream->s[i] == ':') {
                s = new_string(i - stream->start,
                               stream->s + stream->start);
                pkg_name = new_symbol(s);
                pkg = find_package(pkg_name, env);
                stream->start = ++i;
                if (i < stream->end &&
                    stream->s[i] == ':')
                        stream->start = ++i;
        }
        if (i < stream->end && stream->s[i] == ':') {
                stream->start = ++i;
                error(env, "too many columns after package %s",
                      string_str(pkg->name->string));
        }
        while (i < stream->end && !endchar(stream->s[i]))
                i++;
        if (i == stream->start) {
                if (pkg)
                        error(env, "no symbol name after package %s",
                              string_str(pkg->name->string));
                return NULL;
        }
        if (!pkg)
                pkg = package(env);
        s = new_string(i - stream->start,
                       stream->s + stream->start);
        f = (u_form*) intern(s, pkg);
        stream->start = i;
        return f;
}

u_form * read_quote (s_stream *stream, s_env *env)
{
        if (peek_char(stream) == '\'') {
                read_char(stream);
                return cons_quote(read_form(stream, env));
        }
        return NULL;
}

u_form * read_backquote (s_stream *stream, s_env *env)
{
        if (peek_char(stream) == '`') {
                read_char(stream);
                return cons_backquote(read_form(stream, env));
        }
        return NULL;
}

u_form * read_comma (s_stream *stream, s_env *env)
{
        if (peek_char(stream) == ',') {
                read_char(stream);
                switch (peek_char(stream)) {
                case '@':
                        read_char(stream);
                        return cons_comma_at(read_form(stream, env), env);
                case '.':
                        read_char(stream);
                        return cons_comma_dot(read_form(stream, env), env);
                default:
                        return cons_comma(read_form(stream, env), env);
                }
        }
        return NULL;
}

void read_errors (s_stream *stream, s_env *env)
{
        if (peek_char(stream) == ')') {
                read_char(stream);
                error(env, "unmatched close parenthesis");
        }
}

u_form * read_form (s_stream *stream, s_env *env)
{
        u_form *f;
        if (read_spaces(stream))
                return NULL;
        if ((f = read_quote(stream, env)))
                return f;
        if ((f = read_backquote(stream, env)))
                return f;
        if ((f = read_comma(stream, env)))
                return f;
        if ((f = read_cons(stream, env)))
                return f;
        if ((f = read_skiplist(stream, env)))
                return f;
        if ((f = read_string(stream)))
                return f;
        if ((f = read_sharp(stream, env)))
                return f;
        if ((f = read_number(stream)))
                return f;
        read_errors(stream, env);
        if ((f = read_symbol(stream, env)))
                return f;
        if (refill(stream))
                return NULL;
        return NULL;
}

u_form * load_stream (s_stream *stream, s_env *env)
{
        static s_symbol *t_sym = NULL;
        u_form *f;
        s_error_handler eh;
        if (!t_sym)
                t_sym = sym("t", NULL);
        if (setjmp(eh.buf)) {
                fprintf(stderr, "error while loading %s line %lu\n",
                        stream->file_name, stream->line);
                print_error(&eh, stderr, env);
                pop_error_handler(env);
                return nil();
        }
        push_error_handler(&eh, env);
        while ((f = read_form(stream, env)))
                eval(f, env);
        pop_error_handler(env);
        return (u_form*) t_sym;
}

u_form * load_file (const char *path, s_env *env)
{
        s_stream *stream = stream_open(path, env);
        s_unwind_protect up;
        u_form *result;
        if (setjmp(up.buf)) {
                pop_unwind_protect(env);
                stream_close(stream);
        }
        push_unwind_protect(&up, env);
        result = load_stream(stream, env);
        pop_unwind_protect(env);
        stream_close(stream);
        return result;
}
