include $(top_srcdir)/Makefile.decl
-include $(INTROSPECTION_MAKEFILE)

# Preamble
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = \
	--add-include-path=../gdk \
	--warn-all
INTROSPECTION_COMPILER_ARGS = \
	--includedir=$(srcdir) \
	--includedir=. \
	--includedir=../gdk

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"Gsk\"			\
	-DGSK_COMPILATION			\
	-I$(top_builddir)			\
	-I$(top_builddir)/gsk			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/gsk			\
	$(GTK_DEBUG_FLAGS)			\
	$(GSK_DEP_CFLAGS)

# libtool stuff: set version and export symbols for resolving
# since automake doesn't support conditionalized libsomething_la_LDFLAGS
# we use the general approach here
LDADD = \
	$(GTK_LINK_FLAGS) \
	-version-info $(LT_VERSION_INFO) \
	-export-dynamic	\
	-rpath $(libdir) \
	$(no_undefined)

BUILT_SOURCES =

CLEANFILES =

lib_LTLIBRARIES =

gsk_public_source_h = \
	gskdebug.h \
	gskenums.h \
	gsklayer.h \
	gsklayercontent.h \
	gsklayeriter.h \
	gsklayoutmanager.h \
	gskmacros.h \
	gskrenderer.h \
	gsktypes.h

gsk_private_source_h = \
	gsklayercontentprivate.h \
	gsklayerprivate.h

gsk_private_source_c =

gsk_public_built_source_h = \
	gskenumtypes.h

gsk_built_source_c = \
	gskenumtypes.c

gsk_source_c = \
	gskdebug.c \
	gsklayer.c \
	gsklayercontent.c \
	gsklayeriter.c \
	gsklayerstate.c \
	gsklayoutmanager.c \
	gskrenderer.c

BUILT_SOURCES += $(gsk_public_built_source_h) $(gsk_built_source_c)

all_sources = $(gsk_private_source_c) $(gsk_source_c) $(gsk_public_source_h) $(gsk_private_source_h) $(BUILT_SOURCES)

libgsk_3_la_SOURCES = $(all_sources)
libgsk_3_la_CFLAGS = $(AM_CFLAGS) $(GDK_HIDDEN_VISIBILITY_CFLAGS)
libgsk_3_la_LIBADD = $(GSK_DEP_LIBS) $(top_builddir)/gdk/libgdk-3.la
libgsk_3_la_DEPENDENCIES = $(top_builddir)/gdk/libgdk-3.la
libgsk_3_la_LDFLAGS = $(LDADD)

lib_LTLIBRARIES += libgsk-3.la

gskenumtypes.h: stamp-gskenumtypes.h
	@true
stamp-gskenumtypes.h: $(gsk_public_source_h) gskenumtypes.h.template
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--template gskenumtypes.h.template \
		$(gsk_public_source_h) > xgen-geth \
	&& ( cmp -s xgen-geth gskenumtypes.h || cp xgen-geth gskenumtypes.h ) \
	&& rm -f xgen-geth \
	&& echo timestamp > $(@F)
gskenumtypes.c: $(gsk_public_source_h) gskenumtypes.c.template
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--template gskenumtypes.c.template \
		$(gsk_public_source_h) > xgen-getc \
	&& cp xgen-getc gskenumtypes.c \
	&& rm -f xgen-getc

CLEANFILES += stamp-gskenumtypes.h

gskincludedir = $(includedir)/gtk-3.0/gsk
gskinclude_HEADERS = $(gsk_public_source_h) $(gsk_public_built_source_h) gsk.h

if HAVE_INTROSPECTION

introspection_files = $(gsk_source_c) $(gsk_public_source_h)

Gsk-3.0.gir: libgsk-3.la Makefile
Gsk_3_0_gir_SCANNERFLAGS = \
	--add-include-path=$(top_builddir)/gdk \
	--include-uninstalled=$(top_builddir)/gdk/Gdk-3.0.gir \
	--c-include="gsk/gsk.h"
Gsk_3_0_gir_LIBS = libgsk-3.la
Gsk_3_0_gir_FILES = $(introspection_files)
Gsk_3_0_gir_CFLAGS = $(AM_CPPFLAGS)
Gsk_3_0_gir_EXPORT_PACKAGES = gsk-3.0
Gsk_3_0_gir_INCLUDES = GObject-2.0 cairo-1.0 Graphene-1.0
INTROSPECTION_GIRS += Gsk-3.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)

endif

-include $(top_srcdir)/git.mk
