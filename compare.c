
#include <assert.h>
#include <string.h>
#include "compare.h"
#include "form.h"

int compare_equal (void *a, void *b)
{
        u_form *fa = (u_form*) a;
        u_form *fb = (u_form*) b;
        int c;
        if (fa == fb)
                return 0;
        if (!fa)
                return -1;
        if (!fb)
                return 1;
        if (fa->type == FORM_VALUES)
                return compare_equal(value_(fa), fb);
        if (fb->type == FORM_VALUES)
                return compare_equal(fa, value_(fb));
        if (fa->type < fb->type)
                return -1;
        if (fa->type > fb->type)
                return 1;
        switch (fa->type) {
        case FORM_CONS:
                if ((c = compare_equal(fa->cons.car, fb->cons.car)))
                        return c;
                return compare_equal(fa->cons.cdr, fb->cons.cdr);
        case FORM_STRING:
                return strcmp(string_str(fa), string_str(fb));
        case FORM_SYMBOL:
                return compare_symbols(fa, fb);
        case FORM_PACKAGE:
                return compare_packages(fa, fb);
        case FORM_CFUN:
                return compare_symbols(fa->cfun.name, fb->cfun.name);
        case FORM_LAMBDA:
                return compare_symbols(fa->lambda.name, fb->lambda.name);
        case FORM_LONG:
                return (fa->lng.lng < fb->lng.lng ? -1 :
                        fa->lng.lng > fb->lng.lng ? 1 : 0);
        case FORM_DOUBLE:
                return (fa->dbl.dbl < fb->dbl.dbl ? -1 :
                        fa->dbl.dbl > fb->dbl.dbl ? 1 : 0);
        case FORM_SKIPLIST:
                return skiplist_compare_ptr(fa, fb);
        case FORM_VALUES:
        case FORM_SKIPLIST_NODE:
        case FORM_FRAME:
                break;
        }
        assert(0);
        return -1;
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
