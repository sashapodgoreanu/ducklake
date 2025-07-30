/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_DLN_PARSER_TAB_H_INCLUDED
# define YY_DLN_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef DLNDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define DLNDEBUG 1
#  else
#   define DLNDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define DLNDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined DLNDEBUG */
#if DLNDEBUG
extern int dlndebug;
#endif
/* "%code requires" blocks.  */
#line 6 "parser.y"

    #include <vector>
    #include <string>
    #include "ast_nodes.h"

#line 63 "parser.tab.h"

/* Token kinds.  */
#ifndef DLNTOKENTYPE
# define DLNTOKENTYPE
  enum dlntokentype
  {
    DLNEMPTY = -2,
    DLNEOF = 0,                    /* "end of file"  */
    DLNerror = 256,                /* error  */
    DLNUNDEF = 257,                /* "invalid token"  */
    CREATE = 258,                  /* CREATE  */
    DROP = 259,                    /* DROP  */
    COPY = 260,                    /* COPY  */
    MOVE = 261,                    /* MOVE  */
    TO = 262,                      /* TO  */
    DATABOX = 263,                 /* DATABOX  */
    IDENT = 264,                   /* IDENT  */
    DOT = 265                      /* DOT  */
  };
  typedef enum dlntokentype dlntoken_kind_t;
#endif

/* Value type.  */
#if ! defined DLNSTYPE && ! defined DLNSTYPE_IS_DECLARED
union DLNSTYPE
{
#line 21 "parser.y"

    ast::Stmt*                   stmt;
    std::vector<std::string>*    names;
    char*                        str;

#line 96 "parser.tab.h"

};
typedef union DLNSTYPE DLNSTYPE;
# define DLNSTYPE_IS_TRIVIAL 1
# define DLNSTYPE_IS_DECLARED 1
#endif


extern DLNSTYPE dlnlval;


int dlnparse (void);


#endif /* !YY_DLN_PARSER_TAB_H_INCLUDED  */
