lib_LTLIBRARIES = libcss_parser_pp.la libcss_parser.la

libcss_parser_pp_la_SOURCES = parser_pp.h parser_pp.cc

libcss_parser_la_SOURCES = css_syntax.y css_lex.l parser.c parser.h
AM_YFLAGS = -d
AM_LFLAGS = -i


includedir = $(prefix)/include/htmlcxx/css
include_HEADERS = parser.h parser_pp.h
