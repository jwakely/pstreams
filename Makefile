# PStreams Makefile

#        Copyright (C) 2001 - 2020 Jonathan Wakely
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)
#
# SPDX-License-Identifier: BSL-1.0

# TODO configure script (allow doxygenating of EVISCERATE functions)

OPTIM= -O1 -g3
EXTRA_CXXFLAGS=

CFLAGS=-pedantic -Wall -Wextra -Wpointer-arith -Wcast-qual -Wcast-align -Wredundant-decls -Wshadow $(OPTIM)
CXXFLAGS=$(CFLAGS) -std=c++98 -Woverloaded-virtual

prefix = /usr/local
includedir = $(prefix)/include
INSTALL = install
INSTALL_DATA = $(INSTALL) -p -v -m 0644

SOURCES = pstream.h
TESTS = test_pstreams test_minimum
GENERATED_FILES = ChangeLog MANIFEST
EXTRA_FILES = AUTHORS LICENSE_1_0.txt Doxyfile INSTALL.md Makefile README \
	    mainpage.html test_pstreams.cc test_minimum.cc pstreams-devel.spec

DIST_FILES = $(SOURCES) $(GENERATED_FILES) $(EXTRA_FILES)

HASH := \#
VERS := $(shell awk -F' ' '/^$(HASH)define *PSTREAMS_VERSION/{ print $$NF }' pstream.h)

all: $(TESTS) | pstreams.wout

check run_tests test: all
	@for t in $(TESTS) ; do \
	  echo $$t ; \
	  ./$$t >/dev/null 2>&1 || echo "$$t EXITED WITH STATUS $$?" ; \
	done

# TODO why does valgrind cause some tests to hang?
check-valgrind: all
	@for t in $(TESTS) ; do \
	  echo $$t ; \
	  valgrind --track-fds=yes ./$$t \
	    || echo "$$t EXITED WITH STATUS $$?" ; \
	  echo ; \
	done

check-werror: EXTRA_CXXFLAGS := -Werror
check-werror: check

test_%: test_%.cc pstream.h FORCE
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) $(LDFLAGS) -o $@ $<

MANIFEST: Makefile
	@for i in $(DIST_FILES) ; do echo "pstreams-$(VERS)/$$i" ; done > $@

docs: pstream.h mainpage.html
	@doxygen Doxyfile

mainpage.html: pstream.h Makefile
	@perl -pi -e "s/^(<p>Version) [0-9\.]*(<\/p>)/\1 $(VERS)\2/" $@

ChangeLog:
	@if [ -d .git ]; then git log --no-merges | grep -v '^commit ' > $@ ; fi

dist: pstreams-$(VERS).tar.gz pstreams-docs-$(VERS).tar.gz

srpm: pstreams-$(VERS).tar.gz
	@rpmbuild -ts $<

pstreams-$(VERS):
	@ln -s . $@

pstreams-$(VERS).tar.gz: pstream.h $(GENERATED_FILES) pstreams-$(VERS)
	@tar -czvf $@ `cat MANIFEST`

pstreams-docs-$(VERS):
	@ln -s doc/html $@

pstreams-docs-$(VERS).tar.gz: docs pstreams-docs-$(VERS)
	@tar -czvhf $@ pstreams-docs-$(VERS)

TODO : pstream.h mainpage.html test_pstreams.cc
	@grep -nH TODO $^ | sed -e 's@ *// *@@' > $@

clean:
	@rm -f  test_minimum test_pstreams
	@rm -rf doc TODO $(GENERATED_FILES)
	@rm -f  *.tar.gz pstreams-$(VERS) pstreams-docs-$(VERS)

install:
	@install -d $(DESTDIR)$(includedir)/pstreams
	@$(INSTALL_DATA) pstream.h $(DESTDIR)$(includedir)/pstreams/pstream.h

pstreams.wout:
	@echo "Wide Load" | iconv -f ascii -t UTF-32 > $@

.PHONY: TODO check test run_tests dist srpm FORCE
.INTERMEDIATE: $(GENERATED_FILES) pstreams-$(VERS) pstreams-docs-$(VERS)

# vim: sw=2 ts=8 noexpandtab
