
#include <stdlib.h>
#include <string.h>
#include "env.h"
#include "eval.h"
#include "form.h"
#include "package.h"

int compare_symbols (void *a, void *b)
{
        s_symbol *sa = (s_symbol*) a;
        s_symbol *sb = (s_symbol*) b;
        int c;
        if (sa == sb)
                return 0;
        if (!sa)
                return -1;
        if (!sb)
                return 1;
        c = skiplist_compare_ptr(sa->package, sb->package);
        if (c != 0)
                return c;
        return strcmp(string_str(sa->string), string_str(sb->string));
}

s_package * common_lisp_package ()
{
        static s_package *pkg = NULL;
        if (!pkg) {
                s_symbol *sym = make_symbol("common-lisp", NULL);
                pkg = (s_package*) new_package(sym);
        }
        return pkg;
}

s_package * keyword_package ()
{
        static s_package *pkg = NULL;
        if (!pkg) {
                s_symbol *sym = make_symbol("keyword", NULL);
                pkg = new_package(sym);
        }
        return pkg;
}

s_package * cfacts_package ()
{
        static s_package *pkg = NULL;
        if (!pkg) {
                s_symbol *sym = make_symbol("cfacts", NULL);
                pkg = new_package(sym);
                push(pkg->uses, (u_form*) common_lisp_package());
        }
        return pkg;
}

s_symbol * make_symbol (const char *name, s_package *pkg)
{
        s_string *str = new_string(strlen(name), name);
        s_symbol *sym = new_symbol(str);
        sym->package = pkg;
        return sym;
}

s_symbol * find_symbol (s_string *s, s_package *pkg)
{
        s_symbol sym;
        s_skiplist_node *n;
        u_form *p;
        sym.type = FORM_SYMBOL;
        sym.package = pkg;
        sym.string = s;
        n = skiplist_find(pkg->symbols, &sym);
        if (n)
                return n->value;
        p = pkg->uses;
        while (consp(p)) {
                s_symbol *found = find_symbol(s, &p->cons.car->package);
                if (found)
                        return found;
                p = p->cons.cdr;
        }
        return NULL;
}

s_symbol * find_symbol_ (const char *s, s_package *pkg)
{
        unsigned long len = strlen(s);
        s_string *str = alloca(sizeof(s_string) + len + 1);
        init_string(str, len, s);
        return find_symbol(str, pkg);
}

s_symbol * intern (s_string *s, s_package *pkg)
{
        s_symbol *sym;
        if (!pkg)
                pkg = cfacts_package();
        if (pkg) {
                sym = find_symbol(s, pkg);
                if (sym)
                        return sym;
        }
        sym = new_symbol(s);
        sym->package = pkg;
        if (pkg)
                skiplist_insert(pkg->symbols, sym);
        return sym;
}

s_symbol * intern_ (const char *s, s_package *pkg)
{
        s_symbol *sym = find_symbol_(s, pkg);
        s_string *str;
        if (sym)
                return sym;
        str = new_string(strlen(s), s);
        return intern(str, pkg);
}

s_symbol * sym (const char *s, s_env *env)
{
        s_package *pkg;
        if (env)
                pkg = package(env);
        else
                pkg = common_lisp_package();
        return intern_(s, pkg);
}

void unintern (s_string *s, s_package *pkg)
{
        s_symbol sym;
        if (!pkg)
                pkg = cfacts_package();
        sym.type = FORM_SYMBOL;
        sym.package = pkg;
        sym.string = s;
        skiplist_delete(pkg->symbols, &sym);
}

void delete_package (s_package *pkg)
{
        (void) pkg;
}

int compare_packages (void *a, void *b)
{
        s_package *pa = (s_package*) a;
        s_package *pb = (s_package*) b;
        if (!a && !b)
                return 0;
        if (!a)
                return -1;
        if (!b)
                return 1;
        return compare_symbols(pa->name, pb->name);
}

s_package * find_package (s_symbol *name, s_env *env)
{
        s_package pkg;
        s_skiplist_node *n;
        pkg.name = name;
        n = skiplist_find(env->packages, &pkg);
        if (n)
                return (s_package*) n->value;
        return NULL;
}

void init_packages (s_env *env)
{
        env->packages = new_skiplist(10, M_E);
        env->packages->compare = compare_packages;
        skiplist_insert(env->packages, common_lisp_package());
        skiplist_insert(env->packages, cfacts_package());
        skiplist_insert(env->packages, keyword_package());
}

s_package * package (s_env *env)
{
        static s_symbol *package_sym = NULL;
        u_form *f;
        if (!package_sym)
                package_sym = sym("*package*", NULL);
        f = eval((u_form*) package_sym, env);
        if (f->type != FORM_PACKAGE) {
                f = (u_form*) cfacts_package();
                setq(package_sym, f, env);
        }
        return &f->package;
}
