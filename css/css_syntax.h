/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IMPORT_SYM = 258,
     IMPORTANT_SYM = 259,
     IDENT = 260,
     STRING = 261,
     NUMBER = 262,
     PERCENTAGE = 263,
     LENGTH = 264,
     EMS = 265,
     EXS = 266,
     LINK_PSCLASS_AFTER_IDENT = 267,
     VISITED_PSCLASS_AFTER_IDENT = 268,
     ACTIVE_PSCLASS_AFTER_IDENT = 269,
     FIRST_LINE_AFTER_IDENT = 270,
     FIRST_LETTER_AFTER_IDENT = 271,
     HASH_AFTER_IDENT = 272,
     CLASS_AFTER_IDENT = 273,
     LINK_PSCLASS = 274,
     VISITED_PSCLASS = 275,
     ACTIVE_PSCLASS = 276,
     FIRST_LINE = 277,
     FIRST_LETTER = 278,
     HASH = 279,
     CLASS = 280,
     URL = 281,
     RGB = 282,
     CDO = 283,
     CDC = 284,
     CSL = 285
   };
#endif
#define IMPORT_SYM 258
#define IMPORTANT_SYM 259
#define IDENT 260
#define STRING 261
#define NUMBER 262
#define PERCENTAGE 263
#define LENGTH 264
#define EMS 265
#define EXS 266
#define LINK_PSCLASS_AFTER_IDENT 267
#define VISITED_PSCLASS_AFTER_IDENT 268
#define ACTIVE_PSCLASS_AFTER_IDENT 269
#define FIRST_LINE_AFTER_IDENT 270
#define FIRST_LETTER_AFTER_IDENT 271
#define HASH_AFTER_IDENT 272
#define CLASS_AFTER_IDENT 273
#define LINK_PSCLASS 274
#define VISITED_PSCLASS 275
#define ACTIVE_PSCLASS 276
#define FIRST_LINE 277
#define FIRST_LETTER 278
#define HASH 279
#define CLASS 280
#define URL 281
#define RGB 282
#define CDO 283
#define CDC 284
#define CSL 285




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 15 "css_syntax.y"
typedef union YYSTYPE {
	char *lexeme;
	char letter;
	struct property_t *property;
	struct selector_t *selector;
	struct selector_list_t *selector_list;
	int pseudo_class;
	int pseudo_element;
} YYSTYPE;
/* Line 1240 of yacc.c.  */
#line 106 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





