lib_LTLIBRARIES = libhtmlcxx.la
libhtmlcxx_la_SOURCES = parser.h parser.cc \
                   utils.h utils.cc\
				   tree.h\
				   debug.h
# bin_PROGRAMS = htmlcxx
# htmlcxx_LDADD = libhtmlcxx.la
# htmlcxx_SOURCES = htmlcxx.cc
						
noinst_PROGRAMS = tests
tests_SOURCES = tests.cc
tests_LDADD = libhtmlcxx.la

TESTS = tests

includedir = $(prefix)/include/htmlcxx/html
include_HEADERS = parser.h utils.h tree.h
