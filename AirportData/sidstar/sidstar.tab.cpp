/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.

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

      /* C LALR(1) parser skeleton written by Richard Stallman, by
         simplifying the original so-called "semantic" parser.  */

         /* All symbols defined below should begin with yy or YY, to avoid
            infringing on user name space.  This should be done even for local
            variables, as they might otherwise be expanded by user macros.
            There are some unavoidable exceptions within include files to
            define necessary library symbols; they are noted "INFRINGES ON
            USER NAME SPACE" below.  */

            /* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

#define YYDEBUG 1

/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
//#line 1 "C:\\Users\\igor8\\source\\repos\\PMDG_AUTO\\AirportData\\sidstar_1.y"

#include <stdio.h>
#include <string>
#include <locale.h>
#include <ssfunc.h>
#include <vector>
void yyerror(char* s) {
  fprintf(stderr, "%s\n", s);
}
extern int ScanToken();
int yylex() { return ScanToken(); };
int countfixes = 0;
int countrnws = 0;



#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
enum yytokentype {
  RIGHT = 258,
  TURN = 259,
  N260 = 260,
  N261 = 261,
  N262 = 262,
  APPROACHES = 263,
  APPROACH = 264,
  N265 = 265,
  N266 = 266,
  N267 = 267,
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
  NAMEROUTE = 283,
  FIX = 284,
  VAL = 285,
  N286 = 286,
  N287 = 287,
  FROM = 288,
  N289 = 289,
  ATORABOVE = 290,
  ATORBELOW = 291,
  N292 = 292,
  N293 = 293,
  LATLON = 294,
  DIGIN = 295,
  DIGIE = 296,
  N297 = 297,
  N298 = 298,
  TO = 299,
  INTERCEPT = 300,
  UNTIL = 301,
  N302 = 302,
  N303 = 303,
  N304 = 304,
  N305 = 305,
  N306 = 306,
  N307 = 307,
  N308 = 308,
  N309 = 309,
  DIRECT = 310,
  RADIAL = 311,
  SPEED = 312,
  TRK = 313
};
#endif
/* Tokens.  */
#define RIGHT 258
#define TURN 259
#define N260 260
#define N261 261
#define N262 262
#define APPROACHES 263
#define APPROACH 264
#define N265 265
#define N266 266
#define N267 267
#define N268 268
#define N269 269
#define FIXES 270
#define N271 271
#define ENDFIXES 272
#define SIDS 273
#define SID1 274
#define ENDSIDS 275
#define STARS 276
#define STAR 277
#define ENDSTARS 278
#define RNWS 279
#define RNW 280
#define RNWNUM 281
#define ENDRNWS 282
#define NAMEROUTE 283
#define FIX 284
#define VAL 285
#define N286 286
#define N287 287
#define FROM 288
#define N289 289
#define ATORABOVE 290
#define ATORBELOW 291
#define N292 292
#define N293 293
#define LATLON 294
#define DIGIN 295
#define DIGIE 296
#define N297 297
#define N298 298
#define TO 299
#define INTERCEPT 300
#define UNTIL 301
#define N302 302
#define N303 303
#define N304 304
#define N305 305
#define N306 306
#define N307 307
#define N308 308
#define N309 309
#define DIRECT 310
#define RADIAL 311
#define SPEED 312
#define TRK 313


class sV14 {
public:
  char a1[56]; //0
  double sinlat; //56
  double coslat; //64
  double sinlon; //72
  double coslon; //80
  double radlat; //88
  double radlon; //96
  double a14; //104
  double a15; //112
  double a16; //120
  double a17; //128
  double a18; //136
  double a19; //144
  double a20; //152
  double a21; //160
  double a22; //168
  double a23; //176
  double a24; //184
  double a25; //192
  double a26; //200
  __int32 a27; //208
  double a28; //212
  __int32 a29; //220
  double a30; //224
  __int64 m1[45]; //232
  sV14* n;
};

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
  //#line 30 "sidstar_1.y"

  char* string;
  double flo;
  char sym;
  //void* fixpoint;
  void* point;
  __int64 int64;

  //#line 248 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;
extern YYLTYPE yylloc;
/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
//#line 271 "C:\\Users\\igor8\\source\\repos\\PMDG_AUTO\\AirportData\\sidstar.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID(int yyi)
#else
static int
YYID(yyi)
int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void* malloc(YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free(void*); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
  int yyts_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

   /* Copy COUNT objects from FROM to TO.  The source and destination do
      not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

      /* Relocate STACK from its old location to the new one.  The
         local variables YYSIZE and YYSTACKSIZE give the old and new number of
         elements in the stack, and YYPTR gives the new location of the
         stack.  Advance YYPTR to a properly aligned location for the next
         stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

         /* YYFINAL -- State number of the termination state.  */
#define YYFINAL  300
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   311

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  256
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  46
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNRULES -- Number of states.  */
#define YYNSTATES  312

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_int16 yytranslate[] =
{
      -183, 	0, 	0, 	0, 	0, 	-263, 	-248, 	-177, 	0, 	0,
-237, 	0, 	0, 	-212, 	-226, 	-190, 	-173, 	0, 	-186, 	-276,
0, 	0, 	-165, 	0, 	-175, 	-170, 	-233, 	0, 	-123, 	-140,
-137, 	-127, 	0, 	0, 	-126, 	0, 	-104, 	-118, 	-115, 	0,
-103, 	0, 	0, 	-168, 	0, 	0, 	0, 	-110, 	-227, 	-176,
0, 	-95, 	0, 	-146, 	-87, 	-85, 	-79, 	-86, 	-81, 	-232,
-78, 	-166, 	-109, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
-124, 	0, 	0, 	-73, 	0, 	0, 	0, 	0, 	0, 	-70,
-85, 	0, 	0, 	0, 	-107, 	-277, 	0, 	0, 	-69, 	-227,
-66, 	-42, 	-176, 	-80, 	-67, 	-63, 	0, 	0, 	-33, 	-51,
-262, 	-50, 	-24, 	0, 	0, 	0, 	-46, 	0, 	-42, 	-43,
-45, 	-44, 	-62, 	-275, 	-81, 	-41, 	-37, 	0, 	-275, 	0,
-40, 	-176, 	0, 	-66, 	0, 	-34, 	-30, 	-27, 	0, 	0,
-81, 	-60, 	-26, 	0, 	-81, 	-159, 	0, 	-176, 	-176, 	-57,
-35, 	-39, 	-60, 	0, 	-81, 	-60, 	-60, 	-22, 	-14, 	-36,
0, 	0, 	-42, 	-11, 	-10, 	-9, 	-60, 	0, 	-60, 	0,
0, 	-60, 	0, 	-5, 	-60, 	4, 	5, 	0, 	-132, 	0,
0, 	0, 	0, 	0, 	0, 	6, 	7, 	0, 	0, 	0,
0, 	0, 	0, 	0
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,    16,    19,    23,    26,    29
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      60,     0,    -1,    62,    64,    -1,    29,    28,    39,    40,
      30,    30,    41,    30,    30,    -1,    61,    61,    -1,    15,
      61,    17,    -1,    25,    26,    -1,    63,    63,    -1,    24,
      63,    27,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    44,    44,    45,    57,    58,    61,    65,    66
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */

static const char* const yytname[] =
{
  "$end", "error", "LEFT", "RIGHT", "TURN", "OVERFLY", "TRANSITION", "ENDAPPROACHES",
  "APPROACHES", "APPROACH", "GATE", "GATES", "ENDGATES", "N268", "N269",
  "FIXES", "N271", "ENDFIXES", "SIDS", "SID1", "ENDSIDS", "STARS", "STAR",
  "ENDSTARS", "RNWS", "RNW", "RNWNUM", "ENDRNWS", "NAME", "FIX",
  "VAL", "N286", "N287", "FROM", "N289", "ATORABOVE", "ATORBELOW", "HDG",
  "N293", "LATLON", "DIGIN", "DIGIE", "N297", "HDG2", "TO", "INTERCEPT",
  "UNTIL", "VECTORS", "HOLDAT", "INBOUNDCOURSE", "ALT", "LEGDIST", "LEGTIME", "N308", "N309",
  "DIRECT", "RADIAL", "SPEED", "TRK", "$accept", "TT1", "FROMTO",
  "TRKVAL", "SPEEDVAL", "ALTVAL", "OVERFLYBOOL", "DIRECTHEAD", "TURNDIRECT", "AOBVAL", "AOAVAL", "DIRECTBOOL", "LEGVAL",
  "RNWARRAY", "FULLRNW", "STARARRAY", "SIDARRAY", "FIXARRAY", "FULLSTAR", "TT19", "SIDPOINTS", "APPROACHPOINTS", "ROUTEARRAY",
  "TRANSITIONPOINTS", "ROUTEPOINT", "FULLFIX", "GATEARRAY", "FULLGATE", "SIDSTARRNWARRAY", "RNWPOINT", "FIXNAME", "ROUTEPOINTFIX", "FIXPOINT",
  "GATE_POINTS", "APPROACH_POINTS", "APPROACHARRAY", "FULLAPPROACH", "TRANSITIONARRAY", "FULLTRANSITION", "FULLSIDARRAY", "FULLSID", "TT41", "FIX_POINTS",
  "SID_POINTS", "STAR_POINTS", "TT45", "TT46", "TT47", "TT48", "TT49", "TT50", "TT51", "TT52",0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
     -1, 	0, 	0, 	32, 	32, 	25, 	25, 	26, 	26, 	33,
33, 	34, 	34, 	35, 	41, 	41, 	41, 	16, 	16, 	24,
24, 	29, 	29, 	12, 	12, 	13, 	43, 	43, 	14, 	14,
17, 	42, 	42, 	15, 	15, 	18, 	36, 	36, 	37, 	40,
38, 	38, 	39, 	27, 	27, 	28, 	19, 	19, 	20, 	20,
22, 	22, 	21, 	21, 	23, 	23, 	23, 	23, 	23, 	23,
23, 	23, 	30, 	30, 	30, 	31, 	3, 	3, 	4, 	4,
8, 	8, 	9, 	9, 	2, 	2, 	2, 	2, 	1, 	1,
11, 	11, 	11, 	5, 	5, 	7, 	7, 	6, 	6, 	10,
10, 	0, 	0, 	0, 	0, 	0

};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint16 yyr2[] =
{
      2, 	8, 	1, 	0, 	3, 	0, 	2, 	8, 	8, 	0,
3, 	0, 	2, 	7, 	0, 	3, 	3, 	0, 	2, 	7,
10, 	0, 	2, 	0, 	2, 	2, 	0, 	3, 	0, 	2,
6, 	0, 	3, 	0, 	2, 	5, 	0, 	2, 	3, 	2,
0, 	2, 	4, 	0, 	2, 	2, 	0, 	2, 	0, 	2,
0, 	2, 	0, 	2, 	1, 	2, 	4, 	8, 	6, 	7,
7, 	9, 	7, 	8, 	8, 	2, 	0, 	2, 	0, 	2,
0, 	2, 	0, 	2, 	2, 	2, 	2, 	2, 	1, 	1,
0, 	2, 	2, 	0, 	1, 	0, 	2, 	1, 	1, 	0,
1, 	0

};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
   /*static const yytype_uint16 yydefact[] =
   {
          0,     0,     0,     0,     0,     0,     1,     0,     2,     0,
          5,     4,     0,     0,     0,     6,     8,     7,     0,     0,
          0,     0,     0,     3
   };*/
static const yytype_int16 yydefact[] =
{
8, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	-171, 	0,
1, 	0, 	0, 	0, 	0, 	0, 	2, 	0, 	0, 	0,
0, 	0, 	3, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
283, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	-272, 	-272, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	9, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	-272, 	-169, 	-98, 	-31, 	-272, 	0, 	0,
0, 	0, 	0, 	0, 	-93, 	0, 	11, 	0, 	0, 	0,
0, 	0, 	-149, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	-2, 	0, 	-83, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	-93, 	0, 	0, 	0, 	-200,
0, 	-247, 	-217, 	0, 	0, 	-255, 	-93, 	0, 	0, 	-93,
-93, 	0, 	0, 	-120, 	0, 	0, 	-99, 	0, 	0, 	0,
-160, 	0, 	-93, 	0, 	0, 	-93, 	0, 	0, 	-93, 	0,
0, 	0, 	-53, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	0, 	0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
     4, 	130, 	61, 	117, 	156, 	120, 	76, 	62, 	149, 	164,
88, 	177, 	13, 	17, 	34, 	26, 	6, 	42, 	33, 	48,
138, 	121, 	137, 	63, 	11, 	53, 	72, 	89, 	64, 	19,
65, 	80, 	45, 	36, 	43, 	52, 	91, 	107, 	90, 	66,
67, 	5, 	22, 	28, 	0, 	0, 	0, 	0

};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -32
static const yytype_int16 yypact[] =
{
0, 	2, 	17, 	17, 	0, 	0, 	0, 	0, 	23, 16,
0, 	18, 	15, 	0, 	0, 	0, 	0, 	24, 	0, 	0,
25, 	33, 	0, 	22, 	0, 	0, 	0, 	28, 	0, 	0,
0, 	0, 	32, 	34, 	0, 	11, 	0, 	0, 	0, 	46,
0, 	27, 	29, 	0, 	5, 	1, 	19, 	0, 	0, 	0,
10, 	0, 	12, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	47, 	43, 	54, 	40, 	36, 	46, 	46,
0, 	4, 	6, 	0, 	87, 	88, 	86, 	45, 	74, 	0,
0, 	75, 	77, 	76, 	0, 	0, 	55, 	90, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	20, 	65, 	0, 	0,
0, 	0, 	0, 	52, 	44, 	41, 	0, 	37, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	0, 	56, 	0, 	84,
0, 	0, 	50, 	0, 	48, 	0, 	0, 	0, 	79, 	78,
0, 	0, 	0, 	67, 	0, 	0, 	53, 	0, 	0, 	0,
0, 	0, 	0, 	58, 	0, 	0, 	0, 	0, 	0, 	0,
51, 	49, 	0, 	0, 	0, 	0, 	0, 	60, 	0, 	59,
62, 	0, 	71, 	0, 	0, 	0, 	0, 	69, 	0, 	57,
63, 	73, 	64, 	7, 	8, 	0, 	0, 	61, 	81, 	82,
0, 	0, 	0, 	0
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     0, 	168, 	0, 	139, 	0, 	0, 	208, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	293, 	0, 	0, 	128,
0, 	0, 	0, 	-49, 	0, 	0, 	0, 	190, 	-84, 	0,
0, 	-65, 	0, 	0, 	0, 	0, 	-82, 	0, 	0, 	209,
0, 	0, 	0, 	0, 	0, 	0, 	0, 	0

};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
     68, 	31, 	26, 	9, 	68, 	104, 	68, 	68, 	100, 	68,
108, 	24, 	85, 	128, 	38, 	38, 	8, 	38, 	25, 	68,
68, 	9, 	68, 	68, 	129, 	68, 	114, 	38, 	38, 	68,
38, 	38, 	55, 	38, 	101, 	115, 	10, 	85, 	85, 	104,
103, 	31, 	32, 	68, 	36, 	36, 	14, 	36, 	68, 	131,
116, 	68, 	68, 	56, 	68, 	68, 	152, 	68, 	68, 	70,
18, 	70, 	70, 	85, 	70, 	142, 	81, 	85, 	15, 	145,
16, 	57, 	136, 	1, 	70, 	70, 	58, 	70, 	70, 	158,
70, 	82, 	59, 	55, 	70, 	2, 	60, 	3, 	150, 	151,
70, 	20, 	39, 	85, 	50, 	12, 	51, 	23, 	70, 	66,
21, 	66, 	66, 	70, 	66, 	39, 	39, 	10, 	29, 	70,
70, 	27, 	70, 	70, 	66, 	66, 	21, 	66, 	66, 	70,
66, 	71, 	57, 	21, 	66, 	30, 	146, 	58, 	43, 	43,
147, 	43, 	148, 	59, 	84, 	85, 	86, 	60, 	66, 	72,
35, 	72, 	72, 	66, 	72, 	37, 	66, 	66, 	38, 	66,
66, 	40, 	41, 	66, 	72, 	72, 	39, 	72, 	72, 	94,
72, 	95, 	44, 	13, 	72, 	13, 	66, 	46, 	66, 	66,
47, 	66, 	74, 	75, 	175, 	176, 	35, 	35, 	72, 	55,
49, 	66, 	66, 	72, 	66, 	66, 	54, 	66, 	69, 	72,
72, 	66, 	72, 	72, 	30, 	30, 	92, 	93, 	73, 	78,
109, 	87, 	77, 	79, 	99, 	66, 	80, 	83, 	80, 	80,
66, 	80, 	96, 	97, 	56, 	102, 	66, 	66, 	57, 	106,
66, 	80, 	80, 	58, 	80, 	80, 	112, 	80, 	110, 	59,
36, 	80, 	111, 	60, 	113, 	118, 	119, 	122, 	124, 	153,
125, 	126, 	127, 	135, 	132, 	80, 	36, 	36, 	133, 	36,
80, 	139, 	116, 	85, 	163, 	140, 	80, 	80, 	141, 	42,
80, 	154, 	144, 	161, 	31, 	26, 	155, 	31, 	26, 	9,
143, 	162, 	42, 	42, 	165, 	166, 	167, 	31, 	42, 	85,
171, 	157, 	85, 	3, 	159, 	160, 	134, 	14, 	98, 	173,
174, 	178, 	179, 	89, 	83, 	168, 	7, 	169, 	123, 	105,
170, 	0, 	0, 	172, 	0, 	0, 	0, 	0, 	85, 	0,
0, 	0

};

static const yytype_int16 yycheck[] =
{
      49, 	0, 	0, 	0, 	259, 	89, 	261, 	262, 	285, 	264,
92, 	287, 	284, 	288, 	261, 	262, 	279, 	264, 	294, 	274,
275, 	269, 	277, 	278, 	299, 	280, 	288, 	274, 	275, 	284,
277, 	278, 	259, 	280, 	311, 	297, 	284, 	284, 	310, 	123,
89, 	274, 	275, 	298, 	261, 	262, 	283, 	264, 	303, 	114,
312, 	306, 	307, 	280, 	309, 	310, 	138, 	312, 	313, 	259,
286, 	261, 	262, 	310, 	264, 	130, 	298, 	284, 	280, 	134,
282, 	298, 	121, 	256, 	274, 	275, 	303, 	277, 	278, 	144,
280, 	313, 	309, 	259, 	284, 	268, 	313, 	270, 	137, 	138,
290, 	281, 	261, 	310, 	262, 	272, 	264, 	283, 	298, 	259,
273, 	261, 	262, 	303, 	264, 	274, 	275, 	284, 	283, 	309,
310, 	276, 	312, 	313, 	274, 	275, 	287, 	277, 	278, 	265,
280, 	267, 	298, 	294, 	284, 	295, 	285, 	303, 	277, 	278,
289, 	280, 	291, 	309, 	300, 	301, 	302, 	313, 	298, 	259,
263, 	261, 	262, 	303, 	264, 	285, 	306, 	307, 	285, 	309,
310, 	277, 	278, 	313, 	274, 	275, 	283, 	277, 	278, 	283,
280, 	285, 	266, 	262, 	284, 	264, 	259, 	285, 	261, 	262,
285, 	264, 	257, 	258, 	306, 	307, 	274, 	275, 	298, 	259,
283, 	274, 	275, 	303, 	277, 	278, 	296, 	280, 	283, 	309,
310, 	284, 	312, 	313, 	277, 	278, 	68, 	69, 	285, 	285,
280, 	310, 	281, 	284, 	311, 	298, 	259, 	285, 	261, 	262,
303, 	264, 	285, 	283, 	280, 	284, 	309, 	310, 	298, 	261,
313, 	274, 	275, 	303, 	277, 	278, 	259, 	280, 	295, 	309,
261, 	284, 	295, 	313, 	285, 	285, 	260, 	283, 	281, 	296,
285, 	285, 	304, 	283, 	285, 	298, 	277, 	278, 	285, 	280,
303, 	285, 	312, 	284, 	290, 	285, 	309, 	310, 	285, 	261,
313, 	296, 	288, 	285, 	263, 	263, 	305, 	266, 	266, 	266,
131, 	285, 	274, 	275, 	285, 	285, 	285, 	276, 	280, 	310,
285, 	142, 	284, 	0, 	145, 	146, 	118, 	279, 	80, 	285,
285, 	285, 	285, 	284, 	283, 	156, 	3, 	158, 	108, 	90,
161, 	-1, 	-1, 	164, 	-1, 	-1, 	-1, 	-1, 	310, 	0,
0, 	0

};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,    15,    60,    62,    29,    61,     0,    24,    64,    28,
      17,    61,    25,    63,    39,    26,    27,    63,    40,    30,
      30,    41,    30,    30
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-1)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
   /* This is here to suppress warnings from the GCC cpp's
      -Wunused-macros.  Normally we don't worry about that warning, but
      some users do, and we want to make it easy for users to remove
      YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


      /* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
         If N is 0, then set CURRENT to the empty location which ends
         the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


         /* YY_LOCATION_PRINT -- Print the location on the stream.
            This macro was not mandated originally: define only if we know
            we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


            /* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */

#if YYDEBUG

# ifndef YYFPRINTF
#include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif
#include <sidstar.tab.h>

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print(FILE* yyoutput, int yytype, YYSTYPE const* const yyvaluep, YYLTYPE const* const yylocationp)
#else
static void
yy_symbol_value_print(yyoutput, yytype, yyvaluep, yylocationp)
FILE* yyoutput;
int yytype;
YYSTYPE const* const yyvaluep;
YYLTYPE const* const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE(yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT(yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE(yyoutput);
# endif
  switch (yytype)
  {
  case NAMEROUTE: {
    fprintf(yyoutput, "%s", yyvaluep->string);
    break;
  }
  case RNWNUM: {
    fprintf(yyoutput, "%s", yyvaluep->string);
    break;
  }
  case VAL: {
    fprintf(yyoutput, "%f", yyvaluep->flo);
    break;
  }
  case DIGIE: {
    fprintf(yyoutput, "%s", yyvaluep->string);
    break;
  }
  case DIGIN: {
    fprintf(yyoutput, "%s", yyvaluep->string);
    break;
  }
  default:
    break;
  }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print(FILE* yyoutput, int yytype, YYSTYPE const* const yyvaluep, YYLTYPE const* const yylocationp)
#else
static void
yy_symbol_print(yyoutput, yytype, yyvaluep, yylocationp)
FILE* yyoutput;
int yytype;
YYSTYPE const* const yyvaluep;
YYLTYPE const* const yylocationp;
#endif
{
  if (yytype > YYNTOKENS)
    YYFPRINTF(yyoutput, "токен %s (", yytname[yytype - 255]);
  else
    YYFPRINTF(yyoutput, "не терминал %s (", yytname[yytype + 60]);

  //YY_LOCATION_PRINT (yyoutput, *yylocationp);
  //YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print(yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF(yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print(yytype_int16* yybottom, yytype_int16* yytop)
#else
static void
yy_stack_print(yybottom, yytop)
yytype_int16* yybottom;
yytype_int16* yytop;
#endif
{
  YYFPRINTF(stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
  {
    int yybot = *yybottom;
    YYFPRINTF(stderr, " %d", yybot);
  }
  YYFPRINTF(stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print(YYSTYPE* yyvsp, YYLTYPE* yylsp, int yyrule, int* yytsp)
#else
static void
yy_reduce_print(yyvsp, yylsp, yyrule)
YYSTYPE* yyvsp;
YYLTYPE* yylsp;
int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = 0;// yyrline[yyrule];
  YYFPRINTF(stderr, "\nСвертка стека по правилу %d:\n",
    yyrule - 1);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
  {
    YYFPRINTF(stderr, "   $%d = ", yyi + 1);
    /* if (yyrhs[yyprhs[yyrule] + yyi] < 0) {
       YYFPRINTF(stderr, "   Not GOOD!!! ", yyrule);
       yy_symbol_print(stderr, 0,
         &(yyvsp[(yyi + 1) - (yynrhs)])
         , &(yylsp[(yyi + 1) - (yynrhs)]));
     }
     else {*/
    yy_symbol_print(stderr, yytsp[(yyi + 1) - (yynrhs)],
      &(yyvsp[(yyi + 1) - (yynrhs)])
      , &(yylsp[(yyi + 1) - (yynrhs)]));
    /* }*/
    YYFPRINTF(stderr, "\n");
  }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, yytsp); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
   /* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen(const char* yystr)
#else
static YYSIZE_T
yystrlen(yystr)
const char* yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char*
yystpcpy(char* yydest, const char* yysrc)
#else
static char*
yystpcpy(yydest, yysrc)
char* yydest;
const char* yysrc;
#endif
{
  char* yyd = yydest;
  const char* yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr(char* yyres, const char* yystr)
{
  if (*yystr == '"')
  {
    YYSIZE_T yyn = 0;
    char const* yyp = yystr;

    for (;;)
      switch (*++yyp)
      {
      case '\'':
      case ',':
        goto do_not_strip_quotes;

      case '\\':
        if (*++yyp != '\\')
          goto do_not_strip_quotes;
        /* Fall through.  */
      default:
        if (yyres)
          yyres[yyn] = *yyp;
        yyn++;
        break;

      case '"':
        if (yyres)
          yyres[yyn] = '\0';
        return yyn;
      }
  do_not_strip_quotes:;
  }

  if (!yyres)
    return yystrlen(yystr);

  return yystpcpy(yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error(char* yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (!(YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
  {
    int yytype = YYTRANSLATE(yychar);
    YYSIZE_T yysize0 = yytnamerr(0, yytname[yytype]);
    YYSIZE_T yysize = yysize0;
    YYSIZE_T yysize1;
    int yysize_overflow = 0;
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    char const* yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
    int yyx;

# if 0
    /* This is so xgettext sees the translatable formats that are
 constructed on the fly.  */
    YY_("syntax error, unexpected %s");
    YY_("syntax error, unexpected %s, expecting %s");
    YY_("syntax error, unexpected %s, expecting %s or %s");
    YY_("syntax error, unexpected %s, expecting %s or %s or %s");
    YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
    char* yyfmt;
    char const* yyf;
    static char const yyunexpected[] = "syntax error, unexpected %s";
    static char const yyexpecting[] = ", expecting %s";
    static char const yyor[] = " or %s";
    char yyformat[sizeof yyunexpected
      + sizeof yyexpecting - 1
      + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
        * (sizeof yyor - 1))];
    char const* yyprefix = yyexpecting;

    /* Start YYX at -YYN if negative to avoid negative indexes in
 YYCHECK.  */
    int yyxbegin = yyn < 0 ? -yyn : 0;

    /* Stay within bounds of both yycheck and yytname.  */
    int yychecklim = YYLAST - yyn + 1;
    int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
    int yycount = 1;

    yyarg[0] = yytname[yytype];
    yyfmt = yystpcpy(yyformat, yyunexpected);

    for (yyx = yyxbegin; yyx < yyxend; ++yyx)
      if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
      {
        if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
        {
          yycount = 1;
          yysize = yysize0;
          yyformat[sizeof yyunexpected - 1] = '\0';
          break;
        }
        yyarg[yycount++] = yytname[yyx];
        yysize1 = yysize + yytnamerr(0, yytname[yyx]);
        yysize_overflow |= (yysize1 < yysize);
        yysize = yysize1;
        yyfmt = yystpcpy(yyfmt, yyprefix);
        yyprefix = yyor;
      }

    yyf = YY_(yyformat);
    yysize1 = yysize + yystrlen(yyf);
    yysize_overflow |= (yysize1 < yysize);
    yysize = yysize1;

    if (yysize_overflow)
      return YYSIZE_MAXIMUM;

    if (yyresult)
    {
      /* Avoid sprintf, as that infringes on the user's name space.
         Don't have undefined behavior even if the translation
         produced a string with the wrong number of "%s"s.  */
      char* yyp = yyresult;
      int yyi = 0;
      while ((*yyp = *yyf) != '\0')
      {
        if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr(yyp, yyarg[yyi++]);
          yyf += 2;
        }
        else
        {
          yyp++;
          yyf++;
        }
      }
    }
    return yysize;
  }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct(const char* yymsg, int yytype, YYSTYPE* yyvaluep, YYLTYPE* yylocationp)
#else
static void
yydestruct(yymsg, yytype, yyvaluep, yylocationp)
const char* yymsg;
int yytype;
YYSTYPE* yyvaluep;
YYLTYPE* yylocationp;
#endif
{
  YYUSE(yyvaluep);
  YYUSE(yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT(yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
  {

  default:
    break;
  }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse(void* YYPARSE_PARAM);
#else
int yyparse();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse(void);
#else
int yyparse();
#endif
#endif /* ! YYPARSE_PARAM */
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif

# define YY_INITIAL_VALUE(Value) Value

#define YY_ASSERT(E) ((void) (0 && (E)))
/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
= { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


#define _QWORD __int64
__int64 qword_1F9BB42AD90;
__int64* qword_1F9BB42ADB0;
// case 5
__int64 S9400(__int64 a1) {
  __int64 result; // rax@1
  __int64 v2; // rdx@3
  __int64* v3; // rcx@5

  result = a1;
  if (a1)
  {
    v2 = qword_1F9BB42AD90;
    if (qword_1F9BB42AD90)
    {
      v3 = qword_1F9BB42ADB0;
      qword_1F9BB42ADB0 = (__int64*)result;
      *(_QWORD*)(v3 + 152) = result;
      result = v2;
    }
    else
    {
      qword_1F9BB42AD90 = a1;
      qword_1F9BB42ADB0 = (__int64*)a1;
    }
  }
  else
  {
    result = qword_1F9BB42AD90;
  }
  return result;
}
int Fb5, Fd8;
char byte_1F9BB42AE10;

/*__int64 __fastcall S2D30(sV14* a1, int a2, char* a3, __int64 a4, __int64 a5, char a6)
{
  __int64 v6; // r15@1
  char* v7; // rsi@1
  int v8; // edi@1
  sV14* v9; // r14@1
  signed __int64 v10; // rcx@1
  char* v11; // rcx@2
  __int64 v12; // rdx@2
  char v13; // al@3
  char v14; // al@4

  v6 = a4;
  v7 = a3;
  v8 = a2;
  v9 = a1;
  sub_1F9BB1A7040(a1->a8);
  v10 = v9 + 136;
  *(_BYTE*)(v9->a27) = 0;
  *(_QWORD*)(v9->a28) = 0i64;
  *(_DWORD*)(v9->a29) = 0;
  *(_QWORD*)(v9->a30) = 0i64;
  sub_1F9BB132860(v9->a17, 1);
  *(_DWORD*)(v9 + 280) = -1000000;
  *(_DWORD*)(v9 + 296) = -1000000;
  *(_QWORD*)(v9 + 304) = -4526534890170089472i64;
  *(_QWORD*)(v9 + 312) = -4526534890170089472i64;
  *(_QWORD*)(v9 + 320) = -4526534890170089472i64;
  *(_BYTE*)(v9 + 288) = 1;
  *(_QWORD*)(v9 + 328) = -4526534890170089472i64;
  *(_QWORD*)(v9 + 336) = -4526534890170089472i64;
  *(_QWORD*)(v9 + 344) = -4526534890170089472i64;
  *(_DWORD*)(v9 + 292) = -1000000;
  *(_QWORD*)(v9 + 352) = -4526534890170089472i64;
  *(_DWORD*)(v9 + 284) = 0;
  sub_1F9BB1A7040(v9 + 416);
  sub_1F9BB1A7040(v9 + 488);
  sub_1F9BB1A7040(v9 + 624);
  sub_1F9BB1A7040(v9 + 696);
  sub_1F9BB1A7040(v9 + 872);
  sub_1F9BB161B90(v9 + 1000);
  sub_1F9BB161B90(v9 + 1240);
  *(_BYTE*)(v9 + 1504) = 0;
  *(_QWORD*)(v9 + 1508) = 0i64;
  *(_DWORD*)(v9 + 1516) = 0;
  *(_QWORD*)(v9 + 1520) = 0i64;
  *(_QWORD*)(v9 + 1536) = &off_1F9BB33B8C0;
  sub_1F9BB17C02C(v9 + 1544, 24i64, 4i64, (int(__fastcall*)(__int64))sub_1F9BB05B920);
  sub_1F9BB13F3C0(v9 + 1536);
  *(_QWORD*)(v9 + 1656) = &off_1F9BB326068;
  sub_1F9BB13EF40(v9 + 1656);
  sub_1F9BB1336D0(v9);
  *(_QWORD*)(v9 + 600) = 0i64;
  *(_QWORD*)(v9 + 576) = 0i64;
  *(_QWORD*)(v9 + 592) = 0i64;
  *(_DWORD*)(v9 + 364) = v8;
  if (v7)
  {
    v11 = v7;
    v12 = v9 - (_QWORD)v7;
    do
    {
      v13 = *v11;
      v11[v12] = *v11;
      ++v11;
    } while (v13);
    do
    {
      v14 = *v7;
      v7[v12 + 21] = *v7;
      ++v7;
    } while (v14);
  }
  else
  {
    *(_BYTE*)v9 = 0;
    *(_BYTE*)(v9 + 21) = 0;
  }
  sub_1F9BB1A6AF0(v9 + 64, a5);
  *(_QWORD*)(v9 + 1496) = v6;
  *(_BYTE*)(v9 + 368) = a6;
  *(_BYTE*)(v9 + 369) = 0;
  return v9;
}*/



sV14* __fastcall S9DB0(char* fixpoint, void* a2, char* latsign, double lat1, double lat2, char* lonsign, double lon1, double lon2)
{
  double Lat1; // xmm6_8@1
  char* latSign; // r14@1
  void* v10; // rdi@1
  char* fixPoint; // rbp@1
  FILE* file; // rcx@3
  void* p6B0; // rbx@3
  sV14* v14; // rbx@5
  char* fixPoint1; // rdx@5
  char v16; // cl@6
  char* v17; // rcx@8
  char v18; // al@9
  char lonSign; // rcx@10
  char latSign1; // al@10
  double Lat2; // xmm3@10
  double Lon2; // xmm2@10
  double longitude; // xmm2@14
  double latitude; // xmm1@14
  sV14* result; // rax@16
  __int64 v26_1; // [sp+48h] [bp-60h]@3
  __int64 v26_2; // [sp+48h] [bp-60h]@3
  __int64 v27_1; // [sp+58h] [bp-50h]@3
  __int64 v27_2; // [sp+58h] [bp-50h]@3
  __int64 v28_1; // [sp+68h] [bp-40h]@3
  __int64 v28_2; // [sp+58h] [bp-50h]@3
  __int64 v29_1; // [sp+78h] [bp-30h]@3
  __int64 v29_2; // [sp+58h] [bp-50h]@3
  char v30; // [sp+88h] [bp-20h]@3
  char v31; // [sp+89h] [bp-1Fh]@5

  Lat1 = lat1;
  latSign = latsign;
  v10 = a2;
  fixPoint = fixpoint;
  if (Fb5 || Fd8)
  {
    free(fixpoint);
    if (v10)
      free(v10);
    free(latSign);
    free((void*)lonsign);
    result = 0i64;
  }
  else
  {
    //p6B0 = malloc(0x6B0ui64);
    v30 = 0;
    v28_1 = 0i64;
    v28_2 = 0i64;
    v29_1 = 0i64;
    v29_2 = 0i64;
    v26_1 = 0i64;
    v26_2 = 0i64;
    v27_1 = 0i64;
    v27_2 = 0i64;
    //if (!byte_1F9BB42AE10) magdec.bgl
      //sub_1F9BB1B1F80(file);
    v31 = 1;
    v14 = new sV14();// S2D30((__int64)p6B0, 0, String, 0i64, (__int64)&v26_1,(__int64)&v26_2, 0);
    fixPoint1 = fixPoint;
    do
    {
      v16 = *fixPoint1;
      fixPoint1[(_QWORD)v14 - (_QWORD)fixPoint] = *fixPoint1;
      ++fixPoint1;
    } while (v16);
    if (v10)
    {
      v17 = (char*)v10;
      fixPoint1 = (char*)(v14 - (_QWORD)v10);
      do
      {
        v18 = *v17;
        v17[(_QWORD)fixPoint1 + 21] = *v17;
        ++v17;
      } while (v18);
    }
    lonSign = lonsign[0];
    latSign1 = latSign[0];
    Lat2 = lat2;
    Lat2 = (lat2 * 0.01666666666666667 + Lat1) * 0.0174532925199433;
    v14->radlat = Lat2; //96
    Lon2 = lon2;
    Lon2 = (lon2 * 0.01666666666666667 + lon1) * 0.0174532925199433;
    v14->radlon = Lon2; //104
    if (latSign1 != 'N')
      v14->radlat = -Lat2;
    if (lonSign != 'W')
      v14->radlon = -Lon2;
    v14->a14 = 0i64;
    longitude = v14->radlon;
    latitude = v14->radlat;
    //S2070(lonSign, fixPoint1);
    v14->a15 = -1;
    v14->a16 = 1;
    v14->sinlat = sin(v14->radlat);
    v14->coslat = cos(v14->radlat);
    v14->sinlon = sin(v14->radlon);
    v14->coslon = cos(v14->radlon);
    free(fixPoint);
    if (v10)
      free(v10);
    free(latSign);
    free(lonsign);
    result = v14;
  }
  return result;
}

sV14* F4 = 0;
sV14* F3 = 0;

__int64 SA4E0(sV14* a1)
{
  __int64 result; // rax@3
  sV14* v2; // rax@4

  if (a1)
  {
    if (!F4)
    {
      F4 = a1;
      result = 0i64;
      F3 = a1;
      return result;
    }
    v2 = F3;
    F3 = a1;
    v2->n = a1; //584
  }
  return 0i64;
}

int
yyparse(void)
{
  int yychar;

  /* The semantic value of the lookahead symbol.  */
  /* Default value used for initialization, for pacifying older GCCs
     or non-GCC compilers.  */

     //YY_INITIAL_VALUE(static YYSTYPE yyval_default;)
       //YYSTYPE yylval YY_INITIAL_VALUE(= yyval_default);

     /* Location data for the lookahead symbol.  */
  static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
    = { 1, 1, 1, 1 }
# endif
  ;
  YYLTYPE yylloc = yyloc_default;
  /* Number of syntax errors so far.  */
  int yynerrs;


  int yystate;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;

  /* The stacks and their tools:
     'yyss': related to states.
     'yyvs': related to semantic values.
     'yyls': related to locations.

     Refer to the stacks through separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

     /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16* yyss;
  yytype_int16* yyssp;
  yytype_int16 yys;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE* yyvs;
  YYSTYPE* yyvsp;
  YYSTYPE* yyv;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE* yyls;
  YYLTYPE* yylsp;

  /* The token stack.  */
  int yytsa[YYINITDEPTH];
  int* yyts;
  int* yytsp;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  int yypacktoken;
  YYLTYPE yyloc;


#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N),  yytsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
     //int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yyts = yytsa;
  yystacksize = 200;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char* yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N), yytsp -= (N))

  int yylen = 0;

  yyssp = yyss = yyssa;
  yyv = yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yytsp = yyts = yytsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF((stderr, "Старт парсинга\n"));

  yystate = 0;
  //*yyssp = yystate;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

  /*------------------------------------------------------------.
  | yynewstate -- Push a new state, which is found in yystate.  |
  `------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyvsp = yyv;

yysetstate:
  YYDPRINTF((stderr, "Вход в состояние %d\n", yystate));
  YY_ASSERT(0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16)yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
  {
    /* Get the current used size of the for stacks, in elements.  */
    YYSIZE_T yysize = (YYSIZE_T)(yyssp - yyss + 1);

# if defined yyoverflow
    {
      /* Give user a chance to reallocate the stack.  Use copies of
         these so that the &'s don't force the real ones into
         memory.  */
      YYSTYPE* yyvs1 = yyvs;
      yytype_int16* yyss1 = yyss;
      YYLTYPE* yyls1 = yyls;
      YYLTYPE* yyts1 = yyts;

      /* Each stack pointer address is followed by the size of the
         data in use in that stack, in bytes.  This used to be a
         conditional around just the two extra args, but that might
         be undefined if yyoverflow is a macro.  */
      yyoverflow(YY_("memory exhausted"),
        &yyss1, yysize * sizeof(*yyssp),
        &yyvs1, yysize * sizeof(*yyvsp),
        &yyls1, yysize * sizeof(*yylsp),
        &yyts1, yysize * sizeof(*yytsp),
        &yystacksize);
      yyss = yyss1;
      yyvs = yyvs1;
      yyls = yyls1;
      yyts = yyts1;
    }
# else /* defined YYSTACK_RELOCATE */
    /* Extend the stack our own way.  */
    if (YYMAXDEPTH <= yystacksize)
      goto yyexhaustedlab;
    yystacksize *= 2;
    if (YYMAXDEPTH < yystacksize)
      yystacksize = YYMAXDEPTH;

    {
      yytype_int16* yyss1 = yyss;
      union yyalloc* yyptr =
        (union yyalloc*) YYSTACK_ALLOC(YYSTACK_BYTES(yystacksize));
      if (!yyptr)
        goto yyexhaustedlab;
      YYSTACK_RELOCATE(yyss_alloc, yyss);
      YYSTACK_RELOCATE(yyvs_alloc, yyvs);
      YYSTACK_RELOCATE(yyls_alloc, yyls);
      YYSTACK_RELOCATE(yyts_alloc, yyts);
# undef YYSTACK_RELOCATE
      if (yyss1 != yyssa)
        YYSTACK_FREE(yyss1);
    }
# endif

    yyssp = yyss + yysize - 1;
    yyvsp = yyvs + yysize - 1;
    yylsp = yyls + yysize - 1;
    yytsp = yyts + yysize - 1;

    YYDPRINTF((stderr, "Размер стека увеличен до %lu\n",
      (unsigned long)yystacksize));

    if (yyss + yystacksize - 1 <= yyssp)
      YYABORT;
  }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;


  goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

     /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn != 0)
    goto yyreduce;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar < YYEOF)
  {
    YYDPRINTF((stderr, "Чтение токена: "));
    yychar = YYLEX;
    setlocale(LC_ALL, "russian");
  }

  if (yychar <= YYEOF)
  {
    yychar = 0;
    YYDPRINTF((stderr, "Токенов больше нет.\n"));
  }
  else
  {
    yytoken = yytranslate[yystate];
    YY_SYMBOL_PRINT("Следующий ", yychar, &yylval, &yylloc);
  }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn = yytoken + yychar;
  if (yytoken == 0 || 308 <= yyn || yycheck[yyn] != yychar)
    goto yydefault;
  yyn = yytable[yyn];


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT("Сдвиг", yychar, &yylval, &yylloc);

  *++yytsp = yychar;
  yyssp++;
  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    * ++yyv = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
    * ++yylsp = yylloc;

  goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if ((yyn == 0) || ((yyn + yychar) > 308) || (yycheck[yyn + yychar] != yychar))
    goto yyerrlab;
  yyn = yytable[yyn + yychar];


  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1 - yylen];
  yypacktoken = yytsp[1 - yylen];

  /* Default location.  */
  YYLLOC_DEFAULT(yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT(yyn);
  int yyt = yyn - 1;
  if ((yyt == 1) ||
    (yyt == 8) ||
    (yyt == 13) ||
    (yyt == 15) ||
    (yyt == 18) ||
    (yyt == 21) ||
    (yyt == 25) ||
    (yyt == 30) ||
    (yyt == 56) ||
    (yyt == 58) ||
    (yyt == 62) ||
    (yyt == 76) ||
    (yyt == 79) ||
    (yyt >= 90) 
    )
  {
    yyval = { 0 };
  }
  switch (yyt)
  {
  case 0: {
    yyval.point = sidStar((std::vector<FixPoint>*) yyvsp[1 - yylen].point, (std::vector<Sid>*)yyvsp[5 - yylen].point, (std::vector<Sid>*)yyvsp[6 - yylen].point, (std::vector<Sid>*)yyvsp[7 - yylen].point);
    //EVALUATE: FIX_POINTS RNWS RNWARRAY ENDRNWS SID_POINTS STAR_POINTS APPROACH_POINTS GATE_POINTS
    break;
  }
  case 65: case 67: case 69: case 71: case 78: case 82: case 84: case 88: {
    yyval = { 0 };
    break;
  }
  case 2: case 4: case 8: case 10: case 13: case 16: case 20: case 22: case 25: case 27: case 30: case 32: case 35: case 39: case 42: case 45: case 47: case 49: case 51: {
    yyval = { 0 };
    break;
  }
  case 3: case 9: case 14: case 15: case 26: case 31: {
    yyval = yyvsp[2 - yylen]; //-1=2-3
    break;
  }
  case 5: {
    //yyval.sym = S9400(/*__int64(yyvsp[2 - yylen].sym)*/0); //0=2-2
    break;
  }
  case 11: {
    yyval.point = addApproach((std::vector<Sid>*)yyvsp[1 - yylen].point, (Sid*)yyvsp[2 - yylen].point);
    //APPROACHARRAY: APPROACHARRAY FULLAPPROACH
    break;
  }
  case 12: {
    yyval.point = fullApproach(yyvsp[2 - yylen].string, (std::vector<RoutePoint>*)yyvsp[3 - yylen].point, yyvsp[5 - yylen].string, (std::vector<RoutePoint>*)yyvsp[6 - yylen].point, (std::vector<RnwSid1>*)yyvsp[7 - yylen].point);
    //FULLAPPROACH: APPROACH NAME SIDPOINTS RNW RNWNUM APPROACHPOINTS TRANSITIONARRAY	
    break;
  }
  case 17: {
    yyval.point = addFix((std::vector<FixPoint>*)yyvsp[1 - yylen].point, (FixPoint*)yyvsp[2 - yylen].point);
    //FIXARRAY: FIXARRAY FULLFIX
    //yyval.int64 = SA4E0(yyvsp[2 - yylen].fixpoint); //0=2-2
    break;
  }
  case 19: {
    /*double x0 = yyvsp[10 - yylen].flo; // 0=10-10
    double x1 = yyvsp[9 - yylen].flo; // -1=9-10
    char* c1 = yyvsp[8 - yylen].string; // -2=8-10
    double x2 = yyvsp[6 - yylen].flo; // -4=6-10
    char* c2 = yyvsp[5 - yylen].string; // -5=5-10
    char* c3 = yyvsp[3 - yylen].string; // -7=3-10
    char* c4 = yyvsp[2 - yylen].string; // -8=2-10
    double x3 = yyvsp[7 - yylen].flo; // -3=7-10*/
    //yyval.fixpoint = S9DB0(c4, c3, c2, x2, x3, c1, x1, x0);
    yyval.point = fullFix(yyvsp[2 - yylen].string, yyvsp[5 - yylen].string, yyvsp[6 - yylen].flo,
      yyvsp[7 - yylen].flo, yyvsp[8 - yylen].string, yyvsp[9 - yylen].flo, yyvsp[10 - yylen].flo);
  //FULLFIX: FIX NAME FIXNAME LATLON DIGIN VAL VAL DIGIE VAL VAL
    break;
  }
  case 28: {
    yyval.point = addStar((std::vector<Sid>*)yyvsp[1 - yylen].point, (Sid*)yyvsp[2 - yylen].point);
    //STARARRAY: STARARRAY FULLSTAR
    break;
  }
  case 29: {
    yyval.point = fullStar(yyvsp[2 - yylen].string, (RoutePoint*)yyvsp[3 - yylen].point, (std::vector<RoutePoint>*)yyvsp[4 - yylen].point, (std::vector<RnwSid1>*)yyvsp[5 - yylen].point, (std::vector<std::string>*)yyvsp[6 - yylen].point);
    //FULLSTAR: STAR NAME ROUTEPOINT SIDPOINTS TRANSITIONARRAY SIDSTARRNWARRAY
    break;
  }
  case 33: {
    yyval.point = addSid((std::vector<Sid>*)yyvsp[1 - yylen].point, (Sid*)yyvsp[2 - yylen].point);
    //SIDARRAY: SIDARRAY TT19
    break;
  }
  case 34: {
    yyval.point = tt19(yyvsp[2 - yylen].string, (std::vector<RoutePoint>*)yyvsp[3 - yylen].point, (Sid*)yyvsp[4 - yylen].point, (std::vector<RnwSid1>*)yyvsp[5 - yylen].point);
    //TT19: SID1 NAME SIDPOINTS TT41 TRANSITIONARRAY	
    break;
  }
  case 36: {
    yyval.point = addTransition((std::vector<RnwSid1>*)yyvsp[1 - yylen].point, (RnwSid1*)yyvsp[2 - yylen].point);
    //TRANSITIONARRAY: TRANSITIONARRAY FULLTRANSITION
    break;
  }
  case 37: {
    yyval.point = fullTransition(yyvsp[2-yylen].string, (std::vector<RoutePoint>*)yyvsp[3 - yylen].point);
    //FULLTRANSITION: TRANSITION NAME TRANSITIONPOINTS
    break;
  }
  case 38: {
    yyval.point = tt41((Sid*)yyvsp[1-yylen].point, (Sid*)yyvsp[2-yylen].point);
    //TT41: FULLSID FULLSIDARRAY	
    break;
  }
  case 40: {
    yyval.point = addFullSid((Sid*)yyvsp[1-yylen].point,(Sid*)yyvsp[2-yylen].point);
    //FULLSIDARRAY: FULLSIDARRAY FULLSID
    break;
  }
  case 41: {
    yyval.point = fullSid(yyvsp[1-yylen].string, (std::vector<std::string>*)yyvsp[2-yylen].point, (RoutePoint*)yyvsp[3 - yylen].point, (std::vector<RoutePoint>*)yyvsp[4 - yylen].point);
    //FULLSID: RNWPOINT SIDSTARRNWARRAY ROUTEPOINT ROUTEARRAY	
    break;
  }
  case 43: {
    yyval.point = addSSRnw((std::vector<std::string>*)yyvsp[1 - yylen].point, yyvsp[2 - yylen].string);
      //SIDSTARRNWARRAY: SIDSTARRNWARRAY RNWPOINT
      break;
  }
  case 44: {
    yyval.string = yyvsp[2 - yylen].string;
    break;
  }
  case 46: {
    yyval.point = addSidPoints((std::vector<RoutePoint>*)yyvsp[1 - yylen].point, (RoutePoint*)yyvsp[2 - yylen].point);
    //SIDPOINTS: SIDPOINTS ROUTEPOINT	
    break;
  }
  case 48: {
    yyval.point = addApproachPoints((std::vector<RoutePoint>*)yyvsp[1 - yylen].point, (RoutePoint*)yyvsp[2 - yylen].point);
    //APPROACHPOINTS: APPROACHPOINTS ROUTEPOINT
    break;
  }
  case 50: {
    yyval.point = addSidPoints((std::vector<RoutePoint>*)yyvsp[1 - yylen].point, (RoutePoint*)yyvsp[2 - yylen].point);
    //TRANSITIONPOINTS: TRANSITIONPOINTS ROUTEPOINT
    break;
  }
  case 52: {
    yyval.point = addSidPoints((std::vector<RoutePoint>*)yyvsp[1 - yylen].point, (RoutePoint*)yyvsp[2 - yylen].point);
    //ROUTEARRAY: ROUTEARRAY ROUTEPOINT	
    break;
  }
  case 53: {
    yyval = yyvsp[1 - yylen];
    //ROUTEPOINT: ROUTEPOINTFIX
    break;
  }
  case 54: {
    yyval.point = routePoint(yyvsp[1 - yylen].flo, true, 0.0, 0.0, 0, "", 0.0);
    //ROUTEPOINT: TRKVAL VECTORS
    break;
  }
  case 55: {
    yyval.point = routePoint(yyvsp[1 - yylen].flo, false, yyvsp[3-yylen].flo, 0.0, 0, "", yyvsp[4-yylen].flo);
    //ROUTEPOINT: TRKVAL UNTIL VAL SPEEDVAL	
    break;
  }
  case 57: {
    yyval.point = routePoint(yyvsp[1 - yylen].flo, false, yyvsp[3 - yylen].flo, 0.0, 0, yyvsp[5 - yylen].string, yyvsp[6 - yylen].flo);
    //ROUTEPOINT: TRKVAL UNTIL VAL FROM FIXPOINT SPEEDVAL
    break;
  }
  case 59: {
    yyval.point = routePoint(yyvsp[1 - yylen].flo, false, 0.0, yyvsp[4-yylen].flo, yyvsp[5-yylen].int64, yyvsp[6 - yylen].string, yyvsp[7 - yylen].flo);
    //ROUTEPOINT: TRKVAL INTERCEPT RADIAL VAL FROMTO FIXPOINT SPEEDVAL
    break;
  }
  case 60: {
    yyval.point = holdAt(yyvsp[2 - yylen].string, yyvsp[3 - yylen].int64, yyvsp[6 - yylen].flo, yyvsp[7 - yylen].flo, yyvsp[8 - yylen].flo, yyvsp[9 - yylen].flo);
    //ROUTEPOINT: HOLDAT FIXPOINT DIRECTHEAD TURN INBOUNDCOURSE VAL ALTVAL SPEEDVAL LEGVAL
    break;
  }
  case 61: {
    yyval.point = routePointFix(yyvsp[1 - yylen].int64, yyvsp[2 - yylen].int64, yyvsp[4 - yylen].int64,
      yyvsp[5 - yylen].string, yyvsp[6 - yylen].flo, yyvsp[6 - yylen].flo, yyvsp[7 - yylen].flo);
    //ROUTEPOINTFIX: TURNDIRECT DIRECTBOOL FIX OVERFLYBOOL NAME VAL SPEEDVAL
    break;
  }
  case 63: {
    yyval.point = routePointFix(yyvsp[1 - yylen].int64, yyvsp[2 - yylen].int64, yyvsp[4 - yylen].int64,
      yyvsp[5 - yylen].string, yyvsp[6 - yylen].flo, yyvsp[7 - yylen].flo, yyvsp[8 - yylen].flo);
    //ROUTEPOINTFIX: TURNDIRECT DIRECTBOOL FIX OVERFLYBOOL NAME AOBVAL AOAVAL SPEEDVAL
    break;
  }
  case 64: {
    yyval.string = yyvsp[2 - yylen].string;
    //FIXPOINT: FIX NAME	
    break;
  }
  case 66: case 68: case 70: case 72: case 73: {
    yyval.flo = yyvsp[2 - yylen].flo;
    break;
  }
  case 74: {
    yyval.int64 = 1000;
    break;
  }
  case 75: {
    yyval.flo = -yyvsp[2 - yylen].flo;
    break;
  }
  case 76: {
    yyval.int64 = -1000;
    break;
  }
  case 79: {
    yyval.flo = 0.0;
    break;
  }
  case 80: {
    yyval.flo = yyvsp[2 - yylen].flo;
    break;
  }
  case 81: {
    yyval.flo = -yyvsp[2 - yylen].flo;
    break;
  }
  case 85: {
    yyval.int64 = yyvsp[2 - yylen].int64;
    break;
  }
  case 87: {
    yyval.int64 = 2;
    break;
  }
  case 77: case 83: case 86: case 89: {
    yyval.int64 = 1;
    break;
  }
  default: break;
  }
  YY_SYMBOL_PRINT("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  //YYPOPSTACK (yylen);
  yys = *yyssp;
  yyvsp -= yylen;
  yyssp -= yylen;
  yylsp -= yylen;
  yytsp -= yylen;
  yylen = 0;
  yyn = yyr1[yyn];
  yyv = yyvsp;
  YY_STACK_PRINT(yyss, yyssp);
  if ((yys != 0) || (yyn != 0))
  {
    goto N1;
  }
  *++yyvsp = yyval;
  yyv++;
  *++yytsp = yyn;
  *++yylsp = yyloc;
  *++yyssp = 4;
  yystate = 4;
  if (yychar < YYEOF) {
    YYDPRINTF((stderr, "Повторное получение токена: "));
    yychar = YYLEX;
    setlocale(LC_ALL, "russian");
    if (yychar <= YYEOF) {
      goto yyerrlab;
    }
    yyv = yyvsp;

  }
  if (yychar != YYEOF) {
    goto yysetstate;
  }
  return 0;
  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
N1:


  yystate = yypgoto[yyn] + *yyssp;
  if (0 != (yystate - *yyssp) && yystate <= 308 && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn];

  *++yyv = yyval;
  *++yytsp = yyn;
  *++yylsp = yyloc;
  *++yyssp = yystate;;
  goto yynewstate;


  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
  {
    ++yynerrs;
#if ! YYERROR_VERBOSE
    yyerror(YY_("syntax error"));
#else
    {
      YYSIZE_T yysize = 0;//yysyntax_error(0, yystate, yychar);
      if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
      {
        YYSIZE_T yyalloc = 2 * yysize;
        if (!(yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
          yyalloc = YYSTACK_ALLOC_MAXIMUM;
        if (yymsg != yymsgbuf)
          YYSTACK_FREE(yymsg);
        yymsg = (char*)YYSTACK_ALLOC(yyalloc);
        if (yymsg)
          yymsg_alloc = yyalloc;
        else
        {
          yymsg = yymsgbuf;
          yymsg_alloc = sizeof yymsgbuf;
        }
      }

      if (0 < yysize && yysize <= yymsg_alloc)
      {
        (void)yysyntax_error(yymsg, yystate, yychar);
        yyerror(yymsg);
      }
      else
      {
        yyerror(YY_("Ошибка синтаксиса"));
        if (yysize != 0)
          goto yyexhaustedlab;
      }
    }
#endif
  }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
  {
    /* If just tried and failed to reuse lookahead token after an
 error, discard it.  */

    if (yychar <= YYEOF)
    {
      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        YYABORT;
    }
    else
    {
      yydestruct("Error: discarding",
        yytoken, &yylval, &yylloc);
      yychar = YYEMPTY;
    }
  }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
    goto yyerrorlab;

  yyerror_range[0] = yylsp[1 - yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK(yylen);
  yylen = 0;
  YY_STACK_PRINT(yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
  {
    yyn = yypact[yystate];
    if (yyn != YYPACT_NINF)
    {
      yyn += YYTERROR;
      if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
      {
        yyn = yytable[yyn];
        if (0 < yyn)
          break;
      }
    }

    /* Pop the current state because it cannot handle the error token.  */
    if (yyssp == yyss)
      YYABORT;

    yyerror_range[0] = *yylsp;
    yydestruct("Error: popping",
      yystos[yystate], yyvsp, yylsp);
    YYPOPSTACK(1);
    yystate = *yyssp;
    YY_STACK_PRINT(yyss, yyssp);
  }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT(yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
  /*-------------------------------------------------.
  | yyexhaustedlab -- memory exhaustion comes here.  |
  `-------------------------------------------------*/
yyexhaustedlab:
  yyerror(YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    yydestruct("Cleanup: discarding lookahead",
      yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK(yylen);
  YY_STACK_PRINT(yyss, yyssp);
  while (yyssp != yyss)
  {
    yydestruct("Cleanup: popping",
      yystos[*yyssp], yyvsp, yylsp);
    YYPOPSTACK(1);
  }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE(yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE(yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID(yyresult);
}



/* Line 1684 of yacc.c  */
//#line 72 "C:\\Users\\igor8\\source\\repos\\PMDG_AUTO\\AirportData\\sidstar_1.y"

