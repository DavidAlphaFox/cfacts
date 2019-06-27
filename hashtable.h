#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "typedefs.h"

struct hashtable {
  e_form_type type;
  long        count;
  long        size;
  long        rehash_size_long;
  double      rehash_size_double;
  double      rehash_threshold;
  u_form    **buckets;
};

s_hashtable * new_hashtable (long size,
                             long rehash_size_long,
                             double rehash_size_double,
                             double rehash_threshold);
int               hashtable_rehash (s_hashtable *h);
u_form *       gethash (s_hashtable *h, u_form *key);
u_form *       sethash (s_hashtable *h, u_form *key, u_form *value);
int            remhash (s_hashtable *h, u_form *key);
void           maphash (s_hashtable *h, u_form *fun, s_env *env);
void           clrhash (s_hashtable *h);
long            sxhash (u_form *x);

u_form * cfun_make_hash_table (u_form *args, s_env *env);
u_form * cfun_hash_table_p (u_form *args, s_env *env);
u_form * cfun_hash_table_count (u_form *args, s_env *env);
u_form * cfun_hash_table_rehash_size (u_form *args, s_env *env);
u_form * cfun_hash_table_rehash_threshold (u_form *args, s_env *env);
u_form * cfun_hash_table_size (u_form *args, s_env *env);
u_form * cfun_gethash (u_form *args, s_env *env);
u_form * cfun_sethash (u_form *args, s_env *env);
u_form * cfun_remhash (u_form *args, s_env *env);
u_form * cfun_maphash (u_form *args, s_env *env);
u_form * cfun_clrhash (u_form *args, s_env *env);
u_form * cfun_sxhash (u_form *args, s_env *env);

#endif
