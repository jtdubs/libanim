lib_LTLIBRARIES = libanim.la
bin_PROGRAMS = example
check_PROGRAMS = test

TESTS = test

example_SOURCES = example.c
example_CFLAGS  = -ansi -Wall -Werror $(DEPS_CFLAGS) -g
example_LDADD   = $(DEPS_LIBS) -lanim -lm

libanim_la_SOURCES = anim.c
libanim_la_CFLAGS  = -ansi -Wall -Werror $(DEPS_CFLAGS) -g
libanim_la_LIBADD  = $(DEPS_LIBS)

test_SOURCES = test.c
test_CFLAGS  = -ansi -Wall -Werror $(DEPS_CFLAGS) -g
test_LDADD   = $(DEPS_LIBS) -lanim -lm
