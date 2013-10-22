include $(top_srcdir)/Makefile.decl

NULL =

AM_CPPFLAGS = \
	-I$(top_srcdir)			\
	-I$(top_builddir)/gdk		\
	-I$(top_srcdir)/gdk		\
	-I$(top_builddir)/clutter	\
	-I$(top_srcdir)/clutter		\
	-DGDK_ENABLE_CLUTTER		\
	$(GTK_DEBUG_FLAGS)		\
	$(GTK_DEP_CFLAGS)

DEPS = \
	$(top_builddir)/clutter/libclutter-3.la

LDADD = \
	$(top_builddir)/clutter/libclutter-3.la	\
	$(top_builddir)/gdk/libgdk-3.la		\
	$(GTK_DEP_LIBS)

noinst_PROGRAMS = $(TEST_PROGS)

TEST_PROGS += \
	actor-graph \
	actor-iter \
	$(NULL)

if BUILDOPT_INSTALL_TESTS
insttestdir = $(pkglibexecdir)/installed-tests
insttest_PROGRAMS = $(TEST_PROGS)

%.test: %$(EXEEXT) Makefile
	$(AM_V_GEN) (echo '[Test]' > $@.tmp; \
	echo 'Type=session' >> $@.tmp; \
	echo 'Exec=$(insttestdir)/$<' >> $@.tmp; \
	mv $@.tmp $@)

test_files = $(TEST_PROGS:=.test)

DISTCLEANFILES = $(test_files)

testmetadir = $(datadir)/installed-tests/$(PACKAGE)
testmeta_DATA = $(test_files)
endif

-include $(top_srcdir)/git.mk
