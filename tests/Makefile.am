
TESTS = check_skiplist check_hashtable
check_PROGRAMS = check_skiplist check_hashtable
check_skiplist_SOURCES = check_skiplist.c $(top_builddir)/compare.h $(top_builddir)/compare.c $(top_builddir)/skiplist.h $(top_builddir)/skiplist.c
check_skiplist_CFLAGS = @CHECK_CFLAGS@
check_skiplist_LDADD = @CHECK_LIBS@

check_hashtable_SOURCES = check_hashtable.c $(top_builddir)/backtrace.c $(top_builddir)/block.c $(top_builddir)/city.c $(top_builddir)/compare.c $(top_builddir)/env.c $(top_builddir)/error.c $(top_builddir)/eval.c $(top_builddir)/form.h $(top_builddir)/form.c $(top_builddir)/form_string.c $(top_builddir)/frame.c $(top_builddir)/hashtable.h $(top_builddir)/hashtable.c $(top_builddir)/lambda.c $(top_builddir)/package.h $(top_builddir)/package.c $(top_builddir)/print.c $(top_builddir)/read.c $(top_builddir)/skiplist.c $(top_builddir)/tags.c $(top_builddir)/unwind_protect.c
check_hashtable_CFLAGS = @CHECK_CFLAGS@
check_hashtable_LDADD = @CHECK_LIBS@ -lreadline
