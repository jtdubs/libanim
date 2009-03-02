lib_LTLIBRARIES = libanim.la

libanim_la_SOURCES = anim.c
libanim_la_CFLAGS  = -ansi -Wall -Werror $(DEPS_CFLAGS) -g
libanim_la_LIBADD  = $(DEPS_LIBS)

check_PROGRAMS = test

test_SOURCES = test.c
test_CFLAGS  = -ansi -Wall -Werror $(DEPS_CFLAGS) -g
test_LDADD   = $(DEPS_LIBS) -lanim -lm

TESTS = test
