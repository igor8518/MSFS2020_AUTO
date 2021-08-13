/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

      /* Undocumented macros, especially those whose name start with YY_,
         are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_SIDSTAR_1_TAB_H_INCLUDED
# define YY_YY_SIDSTAR_1_TAB_H_INCLUDED
         /* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
enum yytokentype
{
  RIGHT = 258,
  TURN = 259,
  OVERFLY = 260,
  TRANSITION = 261,
  N262 = 262,
  APPROACHES = 263,
  APPROACH = 264,
  GATE = 265,
  GATES = 266,
  ENDGATES = 267,
  N268 = 268,
  N269 = 269,
  FIXES = 270,
  N271 = 271,
  ENDFIXES = 272,
  SIDS = 273,
  SID1 = 274,
  ENDSIDS = 275,
  STARS = 276,
  STAR = 277,
  ENDSTARS = 278,
  RNWS = 279,
  RNW = 280,
  RNWNUM = 281,
  ENDRNWS = 282,
  NAME = 283,
  FIX = 284,
  VAL = 285,
  N286 = 286,
  N287 = 287,
  FROM = 288,
  N289 = 289,
  ATORABOVE = 290,
  ATORBELOW = 291,
  HDG = 292,
  N293 = 293,
  LATLON = 294,
  DIGIN = 295,
  DIGIE = 296,
  N297 = 297,
  HDG2 = 298,
  TO = 299,
  INTERCEPT = 300,
  UNTIL = 301,
  VECTORS = 302,
  HOLDAT = 303,
  INBOUNDCOURSE = 304,
  ALT = 305,
  LEGDIST = 306,
  LEGTIME = 307,
  N308 = 308,
  N309 = 309,
  DIRECT = 310,
  RADIAL = 311,
  SPEED = 312,
  TRK = 313
};
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 30 "sidstar_1.y"

  char* string;
  double flo;
  char sym;

#line 122 "sidstar_1.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;









int yyparse(void);

#endif /* !YY_YY_SIDSTAR_1_TAB_H_INCLUDED  */
