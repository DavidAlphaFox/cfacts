
ACLOCAL_AMFLAGS = -I build-aux/m4

bin_PROGRAMS =
CLEANFILES =
include_HEADERS =
lib_LTLIBRARIES =

if DEBUG
AM_CFLAGS = -DDEBUG -O0 -ggdb -std=c99 -pedantic -fgnu89-inline
else
AM_CFLAGS = -DNDEBUG -O2 -std=c99 -pedantic -fgnu89-inline
endif

bin_PROGRAMS += cfacts
cfacts_LDADD = -lreadline -lncurses -lgc
cfacts_SOURCES = \
	backtrace.c \
	block.c \
	cfacts.c \
	city.c \
	compare.c \
	env.c \
	error.c \
	eval.c \
	form.c \
	form_string.c \
	frame.c \
	hashtable.c \
	lambda.c \
	package.c \
	print.c \
	read.c \
	skiplist.c \
	tags.c \
	unwind_protect.c

SUBDIRS = tests
