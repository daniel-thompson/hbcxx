#
# Makefile.am
#
# Part of hbcxx - executable C++ source code
#
# Copyright (C) 2014 Daniel Thompson <daniel@redfelineninja.org.uk>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#


ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = \
	src/hbcxx

src_hbcxx_SOURCES = \
	src/main.cpp \
	src/filesystem.h src/filesystem.cpp \
	src/string.h \
	src/system.h src/system.cpp \
	src/util.h \
	src/CompilationUnit.h src/CompilationUnit.cpp \
	src/DefaultLauncher.h src/DefaultLauncher.cpp \
	src/GdbLauncher.h src/GdbLauncher.cpp \
	src/Launcher.h src/Launcher.cpp \
	src/Options.h src/Options.cpp \
	src/NoArgsLauncher.h src/NoArgsLauncher.cpp \
	src/PrePreProcessor.h src/PrePreProcessor.cpp \
	src/Toolset.h src/Toolset.cpp \
	src/WrapperLauncher.h src/WrapperLauncher.cpp

# self-hosting-test has a potentially long execution time so we launch
# it first (modern automake parellizes the testing)
TESTS = \
	tests/self-hosting-test \
	tests/empty.cpp \
	tests/flags.cpp \
	tests/include.cpp \
	tests/indirect.cpp \
	tests/shlex.cpp \
	tests/source.cpp \
	tests/startswith.cpp \
	tests/system.cpp \
	tests/touch.cpp

TEST_SUPPORT = \
	tests/empty.h \
	tests/indirect.h

EXAMPLES = \
	examples/beep \
	examples/include examples/libzero.cpp examples/libzero.h \
	examples/stopwatch

AM_TESTS_ENVIRONMENT = \
## hbcxx must be on the PATH for the test scripts to find it
  PATH=$(top_builddir)/src:$$PATH; export PATH;

EXTRA_DIST = \
	ChangeLog \
	COPYING \
	README.asciidoc \
	README.html \
	$(EXAMPLES) \
	$(TESTS) $(TEST_SUPPORT)

CLEANFILES =
MAINTAINERCLEANFILES =

if HAVE_ASCIIDOC
all : README.html
CLEANFILES += README.html
.asciidoc.html:
	$(AM_V_GEN)$(ASCIIDOC) -a toc $<
else
MAINTAINERCLEANFILES += README.html
endif
