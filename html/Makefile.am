lib_LTLIBRARIES = libhtmlcxx.la
libhtmlcxx_la_SOURCES = Parser.h Parser.cc \
	Extensions.h Extensions.cc \
	CharsetConverter.h CharsetConverter.cc \
	Uri.h Uri.cc \
	Node.h Node.cc \
	utils.h utils.cc \
	ci_string.h \
	tree.h \
	debug.h
libhtmlcxx_la_CXXFLAGS =
libhtmlcxx_la_LDFLAGS = -version-info 1:0:0

# bin_PROGRAMS = htmlcxx
# htmlcxx_LDADD = libhtmlcxx.la
# htmlcxx_SOURCES = htmlcxx.cc
						
noinst_PROGRAMS = tests
tests_SOURCES = tests.cc
tests_LDADD = libhtmlcxx.la

TESTS = tests

includedir = $(prefix)/include/htmlcxx/html
include_HEADERS = Parser.h utils.h tree.h Node.h Uri.h CharsetConverter.h Extensions.h ci_string.h
