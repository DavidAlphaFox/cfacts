
#include <assert.h>
#include <check.h>
#include "form.h"
#include "hashtable.h"
#include "package.h"

s_hashtable *g_h = NULL;

void setup_inserts ()
{
        g_h = new_hashtable(10, 0, 10.0, 1.5);
}

void teardown_inserts ()
{
        g_h = NULL;
}

void setup_remhash ()
{
        g_h = new_hashtable(10, 0, 10.0, 1.5);
        sethash(g_h, (u_form*) kw("a"), (u_form*) sym("a", NULL));
        sethash(g_h, (u_form*) kw("b"), (u_form*) sym("b", NULL));
        sethash(g_h, (u_form*) kw("c"), (u_form*) sym("c", NULL));
        sethash(g_h, (u_form*) kw("d"), (u_form*) sym("d", NULL));
        sethash(g_h, (u_form*) kw("e"), (u_form*) sym("e", NULL));
        sethash(g_h, (u_form*) kw("f"), (u_form*) sym("f", NULL));
        sethash(g_h, (u_form*) kw("g"), (u_form*) sym("g", NULL));
        sethash(g_h, (u_form*) kw("h"), (u_form*) sym("h", NULL));
        sethash(g_h, (u_form*) kw("i"), (u_form*) sym("i", NULL));
        sethash(g_h, (u_form*) kw("j"), (u_form*) sym("j", NULL));
}

void teardown_remhash ()
{
        g_h = NULL;
}

START_TEST (test_hashtable_create)
{
        s_hashtable *h = new_hashtable(10, 0, 10.0, 1.5);
        assert(h && h->count == 0);
}
END_TEST

START_TEST (test_hashtable_insert_one)
{
        sethash(g_h, (u_form*) kw("a"), (u_form*) sym("a", NULL));
        assert(g_h->count == 1);
        sethash(g_h, (u_form*) kw("a"), (u_form*) sym("b", NULL));
        assert(g_h->count == 1);
        assert(gethash(g_h, (u_form*) kw("a")) == (u_form*)
               sym("b", NULL));
        assert(gethash(g_h, (u_form*) kw("b")) == NULL);
}
END_TEST

START_TEST (test_hashtable_insert_two)
{
        sethash(g_h, (u_form*) kw("a"), (u_form*) sym("a", NULL));
        sethash(g_h, (u_form*) kw("b"), (u_form*) sym("b", NULL));
        assert(g_h->count == 2);
        assert(gethash(g_h, (u_form*) kw("a")) == (u_form*)
               sym("a", NULL));
        assert(gethash(g_h, (u_form*) kw("b")) == (u_form*)
               sym("b", NULL));
        assert(gethash(g_h, (u_form*) kw("c")) == NULL);
}
END_TEST

START_TEST (test_hashtable_insert_ten)
{
        sethash(g_h, (u_form*) kw("a"), (u_form*) sym("a", NULL));
        sethash(g_h, (u_form*) kw("b"), (u_form*) sym("b", NULL));
        sethash(g_h, (u_form*) kw("c"), (u_form*) sym("c", NULL));
        sethash(g_h, (u_form*) kw("d"), (u_form*) sym("d", NULL));
        sethash(g_h, (u_form*) kw("e"), (u_form*) sym("e", NULL));
        sethash(g_h, (u_form*) kw("f"), (u_form*) sym("f", NULL));
        sethash(g_h, (u_form*) kw("g"), (u_form*) sym("g", NULL));
        sethash(g_h, (u_form*) kw("h"), (u_form*) sym("h", NULL));
        sethash(g_h, (u_form*) kw("i"), (u_form*) sym("i", NULL));
        sethash(g_h, (u_form*) kw("j"), (u_form*) sym("j", NULL));
        assert(g_h->count == 10);
        assert(gethash(g_h, (u_form*) kw("a")) == (u_form*)
               sym("a", NULL));
        assert(gethash(g_h, (u_form*) kw("b")) == (u_form*)
               sym("b", NULL));
        assert(gethash(g_h, (u_form*) kw("c")) == (u_form*)
               sym("c", NULL));
        assert(gethash(g_h, (u_form*) kw("d")) == (u_form*)
               sym("d", NULL));
        assert(gethash(g_h, (u_form*) kw("e")) == (u_form*)
               sym("e", NULL));
        assert(gethash(g_h, (u_form*) kw("f")) == (u_form*)
               sym("f", NULL));
        assert(gethash(g_h, (u_form*) kw("g")) == (u_form*)
               sym("g", NULL));
        assert(gethash(g_h, (u_form*) kw("h")) == (u_form*)
               sym("h", NULL));
        assert(gethash(g_h, (u_form*) kw("i")) == (u_form*)
               sym("i", NULL));
        assert(gethash(g_h, (u_form*) kw("j")) == (u_form*)
               sym("j", NULL));
        assert(gethash(g_h, (u_form*) kw("z")) == NULL);
}
END_TEST

START_TEST (test_hashtable_insert_twenty)
{
        sethash(g_h, (u_form*) kw("a"), (u_form*) sym("a", NULL));
        sethash(g_h, (u_form*) kw("b"), (u_form*) sym("b", NULL));
        sethash(g_h, (u_form*) kw("c"), (u_form*) sym("c", NULL));
        sethash(g_h, (u_form*) kw("d"), (u_form*) sym("d", NULL));
        sethash(g_h, (u_form*) kw("e"), (u_form*) sym("e", NULL));
        sethash(g_h, (u_form*) kw("f"), (u_form*) sym("f", NULL));
        sethash(g_h, (u_form*) kw("g"), (u_form*) sym("g", NULL));
        sethash(g_h, (u_form*) kw("h"), (u_form*) sym("h", NULL));
        sethash(g_h, (u_form*) kw("i"), (u_form*) sym("i", NULL));
        sethash(g_h, (u_form*) kw("j"), (u_form*) sym("j", NULL));
        sethash(g_h, (u_form*) kw("k"), (u_form*) sym("k", NULL));
        sethash(g_h, (u_form*) kw("l"), (u_form*) sym("l", NULL));
        sethash(g_h, (u_form*) kw("m"), (u_form*) sym("m", NULL));
        sethash(g_h, (u_form*) kw("n"), (u_form*) sym("n", NULL));
        sethash(g_h, (u_form*) kw("o"), (u_form*) sym("o", NULL));
        sethash(g_h, (u_form*) kw("p"), (u_form*) sym("p", NULL));
        sethash(g_h, (u_form*) kw("q"), (u_form*) sym("q", NULL));
        sethash(g_h, (u_form*) kw("r"), (u_form*) sym("r", NULL));
        sethash(g_h, (u_form*) kw("s"), (u_form*) sym("s", NULL));
        sethash(g_h, (u_form*) kw("t"), (u_form*) sym("t", NULL));
        assert(g_h->count == 20);
        assert(gethash(g_h, (u_form*) kw("a")) == (u_form*)
               sym("a", NULL));
        assert(gethash(g_h, (u_form*) kw("b")) == (u_form*)
               sym("b", NULL));
        assert(gethash(g_h, (u_form*) kw("c")) == (u_form*)
               sym("c", NULL));
        assert(gethash(g_h, (u_form*) kw("d")) == (u_form*)
               sym("d", NULL));
        assert(gethash(g_h, (u_form*) kw("e")) == (u_form*)
               sym("e", NULL));
        assert(gethash(g_h, (u_form*) kw("f")) == (u_form*)
               sym("f", NULL));
        assert(gethash(g_h, (u_form*) kw("g")) == (u_form*)
               sym("g", NULL));
        assert(gethash(g_h, (u_form*) kw("h")) == (u_form*)
               sym("h", NULL));
        assert(gethash(g_h, (u_form*) kw("i")) == (u_form*)
               sym("i", NULL));
        assert(gethash(g_h, (u_form*) kw("j")) == (u_form*)
               sym("j", NULL));
        assert(gethash(g_h, (u_form*) kw("k")) == (u_form*)
               sym("k", NULL));
        assert(gethash(g_h, (u_form*) kw("l")) == (u_form*)
               sym("l", NULL));
        assert(gethash(g_h, (u_form*) kw("m")) == (u_form*)
               sym("m", NULL));
        assert(gethash(g_h, (u_form*) kw("n")) == (u_form*)
               sym("n", NULL));
        assert(gethash(g_h, (u_form*) kw("o")) == (u_form*)
               sym("o", NULL));
        assert(gethash(g_h, (u_form*) kw("p")) == (u_form*)
               sym("p", NULL));
        assert(gethash(g_h, (u_form*) kw("q")) == (u_form*)
               sym("q", NULL));
        assert(gethash(g_h, (u_form*) kw("r")) == (u_form*)
               sym("r", NULL));
        assert(gethash(g_h, (u_form*) kw("s")) == (u_form*)
               sym("s", NULL));
        assert(gethash(g_h, (u_form*) kw("t")) == (u_form*)
               sym("t", NULL));
        assert(gethash(g_h, (u_form*) kw("z")) == NULL);
}
END_TEST

START_TEST (test_remhash_one)
{
        assert(g_h->count == 10);
        remhash(g_h, (u_form*) (kw("a")));
        assert(g_h->count == 9);
}
END_TEST

START_TEST (test_remhash_two)
{
        assert(g_h->count == 10);
        remhash(g_h, (u_form*) (kw("a")));
        remhash(g_h, (u_form*) (kw("b")));
        assert(g_h->count == 8);
}
END_TEST

START_TEST (test_remhash_ten)
{
        assert(g_h->count == 10);
        remhash(g_h, (u_form*) (kw("a")));
        remhash(g_h, (u_form*) (kw("b")));
        remhash(g_h, (u_form*) (kw("c")));
        remhash(g_h, (u_form*) (kw("d")));
        remhash(g_h, (u_form*) (kw("e")));
        remhash(g_h, (u_form*) (kw("f")));
        remhash(g_h, (u_form*) (kw("g")));
        remhash(g_h, (u_form*) (kw("h")));
        remhash(g_h, (u_form*) (kw("i")));
        remhash(g_h, (u_form*) (kw("j")));
        assert(g_h->count == 0);
}
END_TEST

Suite * skiplist_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_inserts;
    TCase *tc_remhash;
    s = suite_create("Hashtable");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_hashtable_create);
    suite_add_tcase(s, tc_core);
    tc_inserts = tcase_create("Inserts");
    tcase_add_checked_fixture(tc_inserts, setup_inserts, teardown_inserts);
    tcase_add_test(tc_inserts, test_hashtable_insert_one);
    tcase_add_test(tc_inserts, test_hashtable_insert_two);
    tcase_add_test(tc_inserts, test_hashtable_insert_ten);
    tcase_add_test(tc_inserts, test_hashtable_insert_twenty);
    suite_add_tcase(s, tc_inserts);
    tc_remhash = tcase_create("remhash");
    tcase_add_checked_fixture(tc_remhash, setup_remhash, teardown_remhash);
    tcase_add_test(tc_remhash, test_remhash_one);
    tcase_add_test(tc_remhash, test_remhash_two);
    tcase_add_test(tc_remhash, test_remhash_ten);
    suite_add_tcase(s, tc_remhash);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = skiplist_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? 0 : 1;
}
