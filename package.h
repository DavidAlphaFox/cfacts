#ifndef PACKAGE_H
#define PACKAGE_H

#include "form.h"

s_package * common_lisp_package ();
s_package *     keyword_package ();
s_package *      cfacts_package ();

s_symbol * make_symbol (const char *name, s_package *pkg);
s_symbol * find_symbol (s_string *s, s_package *pkg);
s_symbol * find_symbol_ (const char *s, s_package *pkg);
s_symbol * intern (s_string *s, s_package *pkg);
s_symbol * intern_ (const char *s, s_package *pkg);
s_symbol * sym (const char *s, s_env *env);
void unintern (s_string *s, s_package *pkg);

void          init_packages (s_env *env);
s_package *   find_package (s_symbol *name, s_env *env);
s_package *        package (s_env *env);

#endif
