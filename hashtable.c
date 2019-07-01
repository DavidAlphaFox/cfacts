#include <assert.h>
#include "city.h"
#include "env.h"
#include "error.h"
#include "eval.h"
#include "form.h"
#include "hashtable.h"
#include "package.h"

void init_hashtable_buckets (s_hashtable *h)
{
        long i;
        h->buckets = malloc(h->size * sizeof(u_form*));
        h->count = 0;
        for (i = 0; i < h->size; i++)
                h->buckets[i] = nil();
}

s_hashtable * new_hashtable (long size,
                             long rehash_size_long,
                             double rehash_size_double,
                             double rehash_threshold)
{
        s_hashtable *h;
        h = malloc(sizeof(s_hashtable));
        if (h) {
                h->type = FORM_HASHTABLE;
                h->size = size;
                h->rehash_size_long = rehash_size_long;
                h->rehash_size_double = rehash_size_double;
                h->rehash_threshold = rehash_threshold;
                init_hashtable_buckets(h);
                return h;
        }
        return NULL;
}

int hashtable_rehash (s_hashtable *h)
{
        double r = h->count / (double) h->size;
        if (r > h->rehash_threshold) {
                long size = h->size;
                u_form **buckets = h->buckets;
                if (h->rehash_size_long)
                        h->size += h->rehash_size_long;
                else
                        h->size *= h->rehash_size_double;
                init_hashtable_buckets(h);
                while (size--) {
                        u_form *b = *buckets++;
                        while (consp(b)) {
                                u_form *k = caar(b);
                                u_form *v = cdar(b);
                                sethash(h, k, v);
                                b = b->cons.cdr;
                        }
                }
        }
        return 0;
}

long hashtable_index (s_hashtable *h, u_form *key)
{
        long hash = sxhash(key);
        long index = hash % h->size;
        if (index < 0)
                index += h->size;
        return index;
}

u_form * gethash (s_hashtable *h, u_form *key)
{
        long index = hashtable_index(h, key);
        u_form *a = h->buckets[index];
        while (consp(a)) {
                if (equal(caar(a), key))
                        return cdar(a);
                a = a->cons.cdr;
        }
        return NULL;
}

u_form * sethash (s_hashtable *h, u_form *key, u_form *value)
{
        long index = hashtable_index(h, key);
        u_form *a = h->buckets[index];
        while (consp(a) && consp(a->cons.car)) {
                if (equal(a->cons.car->cons.car, key)) {
                        a->cons.car->cons.cdr = value;
                        return value;
                }
                a = a->cons.cdr;
        }
        push(h->buckets[index], cons(key, value));
        h->count++;
        hashtable_rehash(h);
        return value;
}

int remhash (s_hashtable *h, u_form *key)
{
        long index = hashtable_index(h, key);
        u_form **a = &h->buckets[index];
        while (consp(*a) && consp((*a)->cons.car)) {
                if (equal((*a)->cons.car->cons.car, key)) {
                        *a = (*a)->cons.cdr;
                        h->count--;
                        return 1;
                }
                a = &(*a)->cons.cdr;
        }
        return 0;
}

void maphash (s_hashtable *h, u_form *fun, s_env *env)
{
        long i;
        for (i = 0; i < h->size; i++) {
                u_form *a = h->buckets[i];
                while (consp(a) && consp(a->cons.car)) {
                        u_form *k = a->cons.car->cons.car;
                        u_form *v = a->cons.car->cons.cdr;
                        u_form *args = cons(k, cons(v, nil()));
                        funcall(fun, args, env);
                        a = a->cons.cdr;
                }
        }
}

void clrhash (s_hashtable *h)
{
        long i;
        for (i = 0; i < h->size; i++)
                h->buckets[i] = nil();
}

long update_hash_ (uint64 *h, void *buf, size_t len)
{
        *h = CityHash64WithSeed((const char *)buf, len, *h);
        return (long) *h;
}

long update_hash_string (uint64 *h, s_string *s)
{
        *h = CityHash64WithSeed(string_str(s), s->length, *h);
        return (long) *h;
}

long update_hash (uint64 *h, u_form *x)
{
        if (x)
                switch (x->type) {
                case FORM_VALUES:
                        return update_hash(h, value_(x));
                case FORM_CONS:
                        update_hash_(h, &x->type, sizeof(x->type));
                        update_hash(h, x->cons.car);
                        return update_hash(h, x->cons.cdr);
                case FORM_STRING:
                        update_hash_(h, &x->type, sizeof(x->type));
                        return update_hash_string(h, &x->string);
                case FORM_SYMBOL:
                        update_hash_(h, &x->type, sizeof(x->type));
                        update_hash_string
                                (h, x->symbol.package->name->string);
                        update_hash_(h, ":", 1);
                        return update_hash_string(h, x->symbol.string);
                case FORM_PACKAGE:
                        update_hash_(h, &x->type, sizeof(x->type));
                        return update_hash_string
                                (h, x->package.name->string);
                case FORM_CFUN:
                        update_hash_(h, &x->type, sizeof(x->type));
                        update_hash(h, (u_form*) x->cfun.name);
                        return update_hash_(h, &x->cfun.fun,
                                            sizeof(void*));
                case FORM_LAMBDA:
                case FORM_SKIPLIST:
                case FORM_SKIPLIST_NODE:
                case FORM_HASHTABLE:
                        update_hash_(h, &x->type, sizeof(x->type));
                        return update_hash_(h, &x, sizeof(u_form*));
                case FORM_LONG:
                        update_hash_(h, &x->type, sizeof(x->type));
                        return update_hash_(h, &x->lng.lng,
                                            sizeof(x->lng.lng));
                case FORM_DOUBLE:
                        update_hash_(h, &x->type, sizeof(x->type));
                        return update_hash_(h, &x->dbl.dbl,
                                            sizeof(x->dbl.dbl));
                default:
                        break;
                }
        assert(0);
        return 0;
}

long sxhash (u_form *x)
{
        uint64 h = 0;
        return update_hash(&h, x);
}

u_form * cfun_make_hash_table (u_form *args, s_env *env)
{
        s_long default_size = { FORM_LONG, 10 };
        u_form *size = getf(args, (u_form*) kw("size"),
                            (u_form*) &default_size);
        s_double default_rehash_size = { FORM_DOUBLE, 10.0 };
        u_form *rehash_size = getf(args, (u_form*) kw("rehash-size"),
                                   (u_form*) &default_rehash_size);
        s_double default_rehash_threshold = { FORM_DOUBLE, 1.5 };
        u_form *rehash_threshold = getf
                (args, (u_form*) kw("rehash-threshold"),
                 (u_form*) &default_rehash_threshold);
        if (!integerp(size) || size->lng.lng < 2 ||
            !numberp(rehash_size) ||
            (integerp(rehash_size) && rehash_size->lng.lng < 1) ||
            (floatp(rehash_size) && rehash_size->dbl.dbl <= 1.0) ||
            !floatp(rehash_threshold) ||
            rehash_threshold->dbl.dbl <= 0.0)
                error(env, "invalid arguments for make-hash-table");
        return (u_form*) new_hashtable
                (size->lng.lng,
                 integerp(rehash_size) ? rehash_size->lng.lng : 0,
                 floatp(rehash_size) ? rehash_size->dbl.dbl : 0.0,
                 rehash_threshold->dbl.dbl);
}

u_form * cfun_hash_table_p (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        if (!t_sym)
                t_sym = (u_form*) sym("t", NULL);
        if (!consp(args) || args->cons.cdr != nil())
                error(env, "invalid arguments for hash-table-p");
        return hashtablep(args->cons.car) ? t_sym : nil();
}

u_form * cfun_hash_table_count (u_form *args, s_env *env)
{
        if (!consp(args) || !hashtablep(args->cons.car) ||
            args->cons.cdr != nil())
                error(env, "invalid arguments for hash-table-count");
        return (u_form*) new_long(args->cons.car->hashtable.count);
}

u_form * cfun_hash_table_rehash_size (u_form *args, s_env *env)
{
        s_hashtable *h;
        if (!consp(args) || !hashtablep(args->cons.car) ||
            args->cons.cdr != nil())
                error(env, "invalid arguments for "
                      "hash-table-rehash-size");
        h = &args->cons.car->hashtable;
        if (h->rehash_size_long)
                return (u_form*) new_long(h->rehash_size_long);
        return (u_form*) new_double(h->rehash_size_double);
}

u_form * cfun_hash_table_rehash_threshold (u_form *args, s_env *env)
{
        s_hashtable *h;
        if (!consp(args) || !hashtablep(args->cons.car) ||
            args->cons.cdr != nil())
                error(env, "invalid arguments for "
                      "hash-table-rehash-threshold");
        h = &args->cons.car->hashtable;
        return (u_form*) new_double(h->rehash_threshold);
}

u_form * cfun_hash_table_size (u_form *args, s_env *env)
{
        s_hashtable *h;
        if (!consp(args) || !hashtablep(args->cons.car) ||
            args->cons.cdr != nil())
                error(env, "invalid arguments for hash-table-size");
        h = &args->cons.car->hashtable;
        return (u_form*) new_long(h->size);
}

u_form * cfun_gethash (u_form *args, s_env *env)
{
        s_hashtable *h;
        u_form *f;
        s_values *v = new_values(2);
        if (!consp(args) || !consp(args->cons.cdr) ||
            !hashtablep(args->cons.cdr->cons.car) ||
            args->cons.cdr->cons.cdr != nil())
                error(env, "invalid arguments for gethash");
        h = &args->cons.cdr->cons.car->hashtable;
        f = gethash(h, args->cons.car);
        if (!f) {
                values_(v)[0] = nil();
                values_(v)[1] = nil();
        } else {
                values_(v)[0] = f;
                values_(v)[1] = (u_form*) sym("t", NULL);
        }
        return (u_form*) v;
}

u_form * cfun_sethash (u_form *args, s_env *env)
{
        s_hashtable *h;
        if (!consp(args) || !consp(args->cons.cdr) ||
            !hashtablep(args->cons.cdr->cons.car) ||
            args->cons.cdr->cons.cdr->cons.cdr != nil())
                error(env, "invalid arguments for sethash");
        h = &args->cons.cdr->cons.car->hashtable;
        return sethash(h, args->cons.car,
                       args->cons.cdr->cons.cdr->cons.car);
}

u_form * cfun_remhash (u_form *args, s_env *env)
{
        static u_form *t_sym = NULL;
        s_hashtable *h;
        if (!t_sym)
                t_sym = (u_form*) sym("t", NULL);
        if (!consp(args) || !consp(args->cons.cdr) ||
            !hashtablep(args->cons.cdr->cons.car) ||
            args->cons.cdr->cons.cdr != nil())
                error(env, "invalid arguments for remhash");
        h = &args->cons.cdr->cons.car->hashtable;
        return remhash(h, args->cons.car) ? t_sym : nil();
}

u_form * cfun_maphash (u_form *args, s_env *env)
{
        s_hashtable *h;
        if (!consp(args) || !consp(args->cons.cdr) ||
            !hashtablep(args->cons.cdr->cons.car) ||
            args->cons.cdr->cons.cdr != nil())
                error(env, "invalid arguments for remhash");
        h = &args->cons.cdr->cons.car->hashtable;
        maphash(h, args->cons.car, env);
        return nil();
}

u_form * cfun_clrhash (u_form *args, s_env *env)
{
        s_hashtable *h;
        if (!consp(args) || !hashtablep(args->cons.car) ||
            args->cons.cdr != nil())
                error(env, "invalid arguments for clrhash");
        h = &args->cons.car->hashtable;
        clrhash(h);
        return (u_form*) h;
}

u_form * cfun_sxhash (u_form *args, s_env *env)
{
        if (!consp(args) || args->cons.cdr != nil())
                return error(env, "invalid arguments for sxhash");
        return (u_form*) new_long(sxhash(args->cons.car));
}
