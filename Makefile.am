lib_LTLIBRARIES = libhtmlcxx.la
libhtmlcxx_la_SOURCES = html_parser.h html_parser.cc \
                   html_utils.h html_utils.cc\
				   tree.h\
				   debug.h
# bin_PROGRAMS = htmlcxx
# htmlcxx_LDADD = libhtmlcxx.la
# htmlcxx_SOURCES = htmlcxx.cc
						
noinst_PROGRAMS = tests
tests_SOURCES = tests.cc
tests_LDADD = libhtmlcxx.la

bin_PROGRAMS = htmlcxx
htmlcxx_SOURCES = htmlcxx.cc
htmlcxx_LDADD = libhtmlcxx.la

TESTS = tests

includedir = $(prefix)/include/htmlcxx
include_HEADERS = html_parser.h html_utils.h tree.h
