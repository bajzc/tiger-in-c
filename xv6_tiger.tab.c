/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* First part of user prologue.  */
#line 1 "tiger.y"

#define YY_NO_UNISTD_H
#define YY_NO_STDIO_H
#define YY_NO_STDLIB_H
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "symbol.h"
#include "util.h"

int yylex(void); /* function prototype */

void yyerror(char *s);

A_exp absyn_root = NULL;

#line 88 "xv6_tiger.tab.c"

#ifndef YY_CAST
#ifdef __cplusplus
#define YY_CAST(Type, Val) static_cast<Type>(Val)
#define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type>(Val)
#else
#define YY_CAST(Type, Val) ((Type) (Val))
#define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#endif
#endif
#ifndef YY_NULLPTR
#if defined __cplusplus
#if 201103L <= __cplusplus
#define YY_NULLPTR nullptr
#else
#define YY_NULLPTR 0
#endif
#else
#define YY_NULLPTR ((void *) 0)
#endif
#endif

#include "xv6_tiger.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t {
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0, /* "end of file"  */
  YYSYMBOL_YYerror = 1, /* error  */
  YYSYMBOL_YYUNDEF = 2, /* "invalid token"  */
  YYSYMBOL_ID = 3, /* ID  */
  YYSYMBOL_STRING = 4, /* STRING  */
  YYSYMBOL_INT = 5, /* INT  */
  YYSYMBOL_COMMA = 6, /* COMMA  */
  YYSYMBOL_COLON = 7, /* COLON  */
  YYSYMBOL_SEMICOLON = 8, /* SEMICOLON  */
  YYSYMBOL_LPAREN = 9, /* LPAREN  */
  YYSYMBOL_RPAREN = 10, /* RPAREN  */
  YYSYMBOL_LBRACK = 11, /* LBRACK  */
  YYSYMBOL_RBRACK = 12, /* RBRACK  */
  YYSYMBOL_LBRACE = 13, /* LBRACE  */
  YYSYMBOL_RBRACE = 14, /* RBRACE  */
  YYSYMBOL_DOT = 15, /* DOT  */
  YYSYMBOL_PLUS = 16, /* PLUS  */
  YYSYMBOL_MINUS = 17, /* MINUS  */
  YYSYMBOL_TIMES = 18, /* TIMES  */
  YYSYMBOL_DIVIDE = 19, /* DIVIDE  */
  YYSYMBOL_EQ = 20, /* EQ  */
  YYSYMBOL_NEQ = 21, /* NEQ  */
  YYSYMBOL_LT = 22, /* LT  */
  YYSYMBOL_LE = 23, /* LE  */
  YYSYMBOL_GT = 24, /* GT  */
  YYSYMBOL_GE = 25, /* GE  */
  YYSYMBOL_AND = 26, /* AND  */
  YYSYMBOL_OR = 27, /* OR  */
  YYSYMBOL_ASSIGN = 28, /* ASSIGN  */
  YYSYMBOL_ARRAY = 29, /* ARRAY  */
  YYSYMBOL_IF = 30, /* IF  */
  YYSYMBOL_THEN = 31, /* THEN  */
  YYSYMBOL_ELSE = 32, /* ELSE  */
  YYSYMBOL_WHILE = 33, /* WHILE  */
  YYSYMBOL_FOR = 34, /* FOR  */
  YYSYMBOL_TO = 35, /* TO  */
  YYSYMBOL_DO = 36, /* DO  */
  YYSYMBOL_LET = 37, /* LET  */
  YYSYMBOL_IN = 38, /* IN  */
  YYSYMBOL_END = 39, /* END  */
  YYSYMBOL_OF = 40, /* OF  */
  YYSYMBOL_BREAK = 41, /* BREAK  */
  YYSYMBOL_NIL = 42, /* NIL  */
  YYSYMBOL_FUNCTION = 43, /* FUNCTION  */
  YYSYMBOL_VAR = 44, /* VAR  */
  YYSYMBOL_TYPE = 45, /* TYPE  */
  YYSYMBOL_UMINUS = 46, /* UMINUS  */
  YYSYMBOL_YYACCEPT = 47, /* $accept  */
  YYSYMBOL_program = 48, /* program  */
  YYSYMBOL_exp = 49, /* exp  */
  YYSYMBOL_expseq = 50, /* expseq  */
  YYSYMBOL_record_list = 51, /* record_list  */
  YYSYMBOL_exp_list = 52, /* exp_list  */
  YYSYMBOL_exp_sequence = 53, /* exp_sequence  */
  YYSYMBOL_decs = 54, /* decs  */
  YYSYMBOL_dec = 55, /* dec  */
  YYSYMBOL_tydecs = 56, /* tydecs  */
  YYSYMBOL_tydec = 57, /* tydec  */
  YYSYMBOL_ty = 58, /* ty  */
  YYSYMBOL_tyfields = 59, /* tyfields  */
  YYSYMBOL_vardec = 60, /* vardec  */
  YYSYMBOL_fundecs = 61, /* fundecs  */
  YYSYMBOL_fundec = 62, /* fundec  */
  YYSYMBOL_lvalue = 63 /* lvalue  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


#ifdef short
#undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
#include <limits.h> /* INFRINGES ON USER NAME SPACE */
#if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#define YY_STDINT_H
#endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
#undef UINT_LEAST8_MAX
#undef UINT_LEAST16_MAX
#define UINT_LEAST8_MAX 255
#define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H &&                  \
       UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H &&                 \
       UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
#if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#define YYPTRDIFF_T __PTRDIFF_TYPE__
#define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
#elif defined PTRDIFF_MAX
#ifndef ptrdiff_t
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#endif
#define YYPTRDIFF_T ptrdiff_t
#define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
#else
#define YYPTRDIFF_T long
#define YYPTRDIFF_MAXIMUM LONG_MAX
#endif
#endif
#define YYSIZE_T uint
#ifndef YYSIZE_T
#ifdef __SIZE_TYPE__
#define YYSIZE_T __SIZE_TYPE__
#elif defined size_t
#define YYSIZE_T size_t
#elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#define YYSIZE_T size_t
#else
#define YYSIZE_T unsigned
#endif
#endif

#define YYSIZE_MAXIMUM                                                         \
  YY_CAST(YYPTRDIFF_T,                                                         \
          (YYPTRDIFF_MAXIMUM < YY_CAST(YYSIZE_T, -1) ? YYPTRDIFF_MAXIMUM       \
                                                     : YY_CAST(YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST(YYPTRDIFF_T, sizeof(X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
#if defined YYENABLE_NLS && YYENABLE_NLS
#if ENABLE_NLS
#include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#define YY_(Msgid) dgettext("bison-runtime", Msgid)
#endif
#endif
#ifndef YY_
#define YY_(Msgid) Msgid
#endif
#endif


#ifndef YY_ATTRIBUTE_PURE
#if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#define YY_ATTRIBUTE_PURE __attribute__((__pure__))
#else
#define YY_ATTRIBUTE_PURE
#endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
#if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#define YY_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
#define YY_ATTRIBUTE_UNUSED
#endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if !defined lint || defined __GNUC__
#define YY_USE(E) ((void) (E))
#else
#define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && !defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
#if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                                    \
  _Pragma("GCC diagnostic push")                                               \
      _Pragma("GCC diagnostic ignored \"-Wuninitialized\"")
#else
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                                    \
  _Pragma("GCC diagnostic push")                                               \
      _Pragma("GCC diagnostic ignored \"-Wuninitialized\"")                    \
          _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#endif
#define YY_IGNORE_MAYBE_UNINITIALIZED_END _Pragma("GCC diagnostic pop")
#else
#define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
#define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && !defined __ICC && 6 <= __GNUC__
#define YY_IGNORE_USELESS_CAST_BEGIN                                           \
  _Pragma("GCC diagnostic push")                                               \
      _Pragma("GCC diagnostic ignored \"-Wuseless-cast\"")
#define YY_IGNORE_USELESS_CAST_END _Pragma("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
#define YY_IGNORE_USELESS_CAST_BEGIN
#define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

#ifdef YYSTACK_USE_ALLOCA
#if YYSTACK_USE_ALLOCA
#ifdef __GNUC__
#define YYSTACK_ALLOC __builtin_alloca
#elif defined __BUILTIN_VA_ARG_INCR
#include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#elif defined _AIX
#define YYSTACK_ALLOC __alloca
#elif defined _MSC_VER
#include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#define alloca _alloca
#else
#define YYSTACK_ALLOC alloca
#if !defined _ALLOCA_H && !defined EXIT_SUCCESS
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
/* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#endif
#endif
#endif
#endif

#ifdef YYSTACK_ALLOC
/* Pacify GCC's 'empty if-body' warning.  */
#define YYSTACK_FREE(Ptr)                                                      \
  do { /* empty */                                                             \
    ;                                                                          \
  } while (0)
#ifndef YYSTACK_ALLOC_MAXIMUM
/* The OS might guarantee only one guard page at the bottom of the stack,
   and a page size can be as small as 4096 bytes.  So we cannot safely
   invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
   to allow for a few compiler-allocated temporary stack slots.  */
#define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#endif
#else
#define YYSTACK_ALLOC YYMALLOC
#define YYSTACK_FREE YYFREE
#ifndef YYSTACK_ALLOC_MAXIMUM
#define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#endif
#if (defined __cplusplus && !defined EXIT_SUCCESS &&                           \
     !((defined YYMALLOC || defined malloc) &&                                 \
       (defined YYFREE || defined free)))
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#endif
#ifndef YYMALLOC
#define YYMALLOC malloc
#if !defined malloc && !defined EXIT_SUCCESS
void *malloc(YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#ifndef YYFREE
#define YYFREE free
#if !defined free && !defined EXIT_SUCCESS
void free(void *); /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#endif
#endif /* !defined yyoverflow */

#if (!defined yyoverflow &&                                                    \
     (!defined __cplusplus ||                                                  \
      (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL &&                     \
       defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc {
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
#define YYSTACK_GAP_MAXIMUM (YYSIZEOF(union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
#define YYSTACK_BYTES(N)                                                       \
  ((N) * (YYSIZEOF(yy_state_t) + YYSIZEOF(YYSTYPE) + YYSIZEOF(YYLTYPE)) +      \
   2 * YYSTACK_GAP_MAXIMUM)

#define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
#define YYSTACK_RELOCATE(Stack_alloc, Stack)                                   \
  do {                                                                         \
    YYPTRDIFF_T yynewbytes;                                                    \
    YYCOPY(&yyptr->Stack_alloc, Stack, yysize);                                \
    Stack = &yyptr->Stack_alloc;                                               \
    yynewbytes = yystacksize * YYSIZEOF(*Stack) + YYSTACK_GAP_MAXIMUM;         \
    yyptr += yynewbytes / YYSIZEOF(*yyptr);                                    \
  } while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
#ifndef YYCOPY
#if defined __GNUC__ && 1 < __GNUC__
#define YYCOPY(Dst, Src, Count)                                                \
  __builtin_memcpy(Dst, Src, YY_CAST(YYSIZE_T, (Count)) * sizeof(*(Src)))
#else
#define YYCOPY(Dst, Src, Count)                                                \
  do {                                                                         \
    YYPTRDIFF_T yyi;                                                           \
    for (yyi = 0; yyi < (Count); yyi++)                                        \
      (Dst)[yyi] = (Src)[yyi];                                                 \
  } while (0)
#endif
#endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL 35
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 399

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 17
/* YYNRULES -- Number of rules.  */
#define YYNRULES 67
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 143

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK 301


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                       \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                                            \
       ? YY_CAST(yysymbol_kind_t, yytranslate[YYX])                            \
       : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] = {
    0,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] = {
    0,   81,  81,  86,  88,  91,  93,  95,  97,  100, 103, 105, 108, 110,
    112, 114, 117, 119, 121, 123, 125, 127, 130, 132, 135, 137, 140, 143,
    146, 148, 151, 154, 157, 160, 163, 168, 169, 171, 175, 177, 181, 183,
    187, 189, 194, 195, 199, 201, 203, 207, 209, 213, 217, 219, 221, 226,
    227, 229, 233, 235, 239, 241, 245, 247, 251, 253, 255, 257};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST(yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name(yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] = {"\"end of file\"",
                                      "error",
                                      "\"invalid token\"",
                                      "ID",
                                      "STRING",
                                      "INT",
                                      "COMMA",
                                      "COLON",
                                      "SEMICOLON",
                                      "LPAREN",
                                      "RPAREN",
                                      "LBRACK",
                                      "RBRACK",
                                      "LBRACE",
                                      "RBRACE",
                                      "DOT",
                                      "PLUS",
                                      "MINUS",
                                      "TIMES",
                                      "DIVIDE",
                                      "EQ",
                                      "NEQ",
                                      "LT",
                                      "LE",
                                      "GT",
                                      "GE",
                                      "AND",
                                      "OR",
                                      "ASSIGN",
                                      "ARRAY",
                                      "IF",
                                      "THEN",
                                      "ELSE",
                                      "WHILE",
                                      "FOR",
                                      "TO",
                                      "DO",
                                      "LET",
                                      "IN",
                                      "END",
                                      "OF",
                                      "BREAK",
                                      "NIL",
                                      "FUNCTION",
                                      "VAR",
                                      "TYPE",
                                      "UMINUS",
                                      "$accept",
                                      "program",
                                      "exp",
                                      "expseq",
                                      "record_list",
                                      "exp_list",
                                      "exp_sequence",
                                      "decs",
                                      "dec",
                                      "tydecs",
                                      "tydec",
                                      "ty",
                                      "tyfields",
                                      "vardec",
                                      "fundecs",
                                      "fundec",
                                      "lvalue",
                                      YY_NULLPTR};

static const char *yysymbol_name(yysymbol_kind_t yysymbol) {
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-112)

#define yypact_value_is_default(Yyn) ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] = {
    133,  4,    -112, -112, 78,   133, 133,  133,  11,   -25,  -112, -112,
    23,   341,  -3,   118,  133,  7,   -112, 216,  20,   -112, 297,  174,
    3,    29,   30,   31,   13,   -25, -112, -10,  -112, -112, 6,    -112,
    133,  133,  133,  133,  133,  133, 133,  133,  133,  133,  133,  133,
    133,  49,   133,  -112, 53,   43,  313,  34,   -112, 41,   133,  -112,
    -112, 133,  133,  133,  47,   -4,  37,   133,  -112, -112, -112, 10,
    10,   -112, -112, 364,  364,  364, 364,  364,  364,  374,  353,  329,
    -112, 341,  133,  -112, 24,   133, -112, 236,  -112, 280,  341,  248,
    57,   62,   133,  -2,   268,  27,  -112, -112, 133,  162,  133,  133,
    61,   74,   58,   341,  -112, 57,  45,   -112, 133,  -112, 341,  87,
    341,  195,  88,   2,    133,  79,  89,   -112, -112, 133,  90,   91,
    133,  341,  -112, -112, 341,  57,  77,   341,  -112, 133,  341};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] = {
    0,  64, 8,  7,  0,  0,  0,  0,  0,  44, 32, 4,  0,  2,  3,  0,  0,  0,
    6,  0,  0,  9,  0,  0,  0,  0,  0,  0,  0,  44, 46, 49, 47, 48, 60, 1,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, 40, 0,
    0,  0,  25, 0,  0,  34, 5,  0,  0,  0,  0,  0,  0,  35, 45, 50, 61, 12,
    13, 14, 15, 16, 17, 19, 21, 18, 20, 22, 23, 0,  65, 27, 0,  11, 67, 0,
    24, 42, 43, 28, 30, 0,  55, 0,  0,  0,  36, 0,  66, 41, 0,  38, 0,  0,
    0,  0,  0,  58, 52, 55, 0,  51, 35, 33, 26, 0,  29, 0,  0,  0,  0,  0,
    0,  37, 39, 0,  56, 0,  0,  59, 53, 54, 31, 55, 0,  62, 57, 0,  63};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] = {-112, -112, 0,    -17,  -19,  15,
                                      44,   76,   -112, 72,   -112, -112,
                                      -111, -112, 75,   -112, -112};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] = {0,  12, 52,  101, 57, 53, 20, 28, 29,
                                        30, 31, 115, 109, 32, 33, 34, 14};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] = {
    13,  112, 125, 97,  19,  21,  22,  23,  48,  131, 55,  113, 49,  15,  24,
    16,  54,  17,  25,  26,  27,  56,  132, 35,  98,  50,  140, 114, 38,  39,
    60,  63,  64,  65,  66,  27,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  25,  85,  67,  84,  87,  89,  90,  96,  99,  91,  86,
    108, 93,  94,  95,  104, 110, 117, 100, 122, 36,  37,  38,  39,  40,  41,
    42,  43,  44,  45,  46,  47,  1,   2,   3,   123, 126, 124, 4,   18,  105,
    55,  130, 135, 134, 138, 5,   137, 141, 111, 127, 128, 103, 92,  69,  118,
    68,  120, 121, 6,   70,  0,   7,   8,   0,   0,   9,   100, 0,   0,   10,
    11,  1,   2,   3,   133, 0,   0,   4,   51,  136, 0,   0,   139, 0,   0,
    5,   1,   2,   3,   0,   0,   142, 4,   0,   0,   0,   0,   0,   6,   0,
    5,   7,   8,   0,   0,   9,   0,   0,   0,   10,  11,  0,   0,   6,   0,
    0,   7,   8,   119, 0,   9,   0,   0,   0,   10,  11,  0,   0,   36,  37,
    38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  36,  37,  38,  39,  40,
    41,  42,  43,  44,  45,  46,  47,  0,   0,   0,   0,   0,   0,   0,   0,
    62,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  0,   58,
    0,   59,  0,   0,   0,   0,   129, 36,  37,  38,  39,  40,  41,  42,  43,
    44,  45,  46,  47,  58,  0,   0,   0,   0,   0,   0,   0,   36,  37,  38,
    39,  40,  41,  42,  43,  44,  45,  46,  47,  36,  37,  38,  39,  40,  41,
    42,  43,  44,  45,  46,  47,  116, 0,   0,   0,   0,   0,   0,   107, 36,
    37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  36,  37,  38,  39,
    40,  41,  42,  43,  44,  45,  46,  47,  0,   0,   0,   0,   106, 36,  37,
    38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  88,  0,   0,   61,  36,
    37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  102, 0,   0,   0,
    36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  36,  37,  38,
    39,  40,  41,  42,  43,  44,  45,  46,  47,  36,  37,  38,  39,  40,  41,
    42,  43,  44,  45,  46,  36,  37,  38,  39,  -1,  -1,  -1,  -1,  -1,  -1,
    36,  37,  38,  39,  40,  41,  42,  43,  44,  45};

static const yytype_int16 yycheck[] = {
    0,  3,  113, 7,  4,   5,   6,  7,  11, 7,   3,   13, 15, 9,   3,   11,  16,
    13, 43, 44,  45, 14,  20,  0,  28, 28, 137, 29,  18, 19, 10,  28,  3,   3,
    3,  45, 36,  37, 38,  39,  40, 41, 42, 43,  44,  45, 46, 47,  48,  43,  50,
    38, 3,  10,  20, 14,  9,   20, 58, 6,  3,   61,  62, 63, 40,  3,   39,  67,
    7,  16, 17,  18, 19,  20,  21, 22, 23, 24,  25,  26, 27, 3,   4,   5,   10,
    40, 28, 9,   10, 89,  3,   3,  3,  14, 3,   17,  6,  20, 98,  116, 119, 86,
    58, 31, 104, 29, 106, 107, 30, 34, -1, 33,  34,  -1, -1, 37,  116, -1,  -1,
    41, 42, 3,   4,  5,   124, -1, -1, 9,  10,  129, -1, -1, 132, -1,  -1,  17,
    3,  4,  5,   -1, -1,  141, 9,  -1, -1, -1,  -1,  -1, 30, -1,  17,  33,  34,
    -1, -1, 37,  -1, -1,  -1,  41, 42, -1, -1,  30,  -1, -1, 33,  34,  6,   -1,
    37, -1, -1,  -1, 41,  42,  -1, -1, 16, 17,  18,  19, 20, 21,  22,  23,  24,
    25, 26, 27,  16, 17,  18,  19, 20, 21, 22,  23,  24, 25, 26,  27,  -1,  -1,
    -1, -1, -1,  -1, -1,  -1,  36, 16, 17, 18,  19,  20, 21, 22,  23,  24,  25,
    26, 27, -1,  8,  -1,  10,  -1, -1, -1, -1,  36,  16, 17, 18,  19,  20,  21,
    22, 23, 24,  25, 26,  27,  8,  -1, -1, -1,  -1,  -1, -1, -1,  16,  17,  18,
    19, 20, 21,  22, 23,  24,  25, 26, 27, 16,  17,  18, 19, 20,  21,  22,  23,
    24, 25, 26,  27, 8,   -1,  -1, -1, -1, -1,  -1,  35, 16, 17,  18,  19,  20,
    21, 22, 23,  24, 25,  26,  27, 16, 17, 18,  19,  20, 21, 22,  23,  24,  25,
    26, 27, -1,  -1, -1,  -1,  32, 16, 17, 18,  19,  20, 21, 22,  23,  24,  25,
    26, 27, 12,  -1, -1,  31,  16, 17, 18, 19,  20,  21, 22, 23,  24,  25,  26,
    27, 12, -1,  -1, -1,  16,  17, 18, 19, 20,  21,  22, 23, 24,  25,  26,  27,
    16, 17, 18,  19, 20,  21,  22, 23, 24, 25,  26,  27, 16, 17,  18,  19,  20,
    21, 22, 23,  24, 25,  26,  16, 17, 18, 19,  20,  21, 22, 23,  24,  25,  16,
    17, 18, 19,  20, 21,  22,  23, 24, 25};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] = {
    0,  3,  4,  5,  9,  17, 30, 33, 34, 37, 41, 42, 48, 49, 63, 9,  11, 13,
    10, 49, 53, 49, 49, 49, 3,  43, 44, 45, 54, 55, 56, 57, 60, 61, 62, 0,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 11, 15, 28, 10, 49, 52,
    49, 3,  14, 51, 8,  10, 10, 31, 36, 28, 3,  3,  3,  38, 54, 56, 61, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 3,  49, 6,  10, 12, 20,
    14, 49, 53, 49, 49, 49, 9,  7,  28, 20, 49, 50, 12, 52, 40, 49, 32, 35,
    3,  59, 3,  49, 3,  13, 29, 58, 8,  39, 49, 6,  49, 49, 7,  10, 28, 59,
    40, 50, 51, 36, 3,  7,  20, 49, 14, 3,  49, 6,  3,  49, 59, 20, 49};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] = {
    0,  47, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
    49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 55, 56, 56,
    57, 58, 58, 58, 59, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 63, 63};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
 */
static const yytype_int8 yyr2[] = {
    0, 2, 1, 1, 1, 3, 2, 1, 1, 2, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 4, 3, 6, 3, 4, 6, 4, 8, 1, 5, 3, 0, 1, 3, 3, 5, 1, 3, 3, 3, 0, 2,
    1, 1, 1, 1, 2, 4, 1, 3, 3, 0, 3, 5, 4, 6, 1, 2, 7, 9, 1, 3, 4, 4};


enum { YYENOMEM = -2 };

#define yyerrok (yyerrstatus = 0)
#define yyclearin (yychar = YYEMPTY)

#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab
#define YYNOMEM goto yyexhaustedlab


#define YYRECOVERING() (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                                 \
  do                                                                           \
    if (yychar == YYEMPTY) {                                                   \
      yychar = (Token);                                                        \
      yylval = (Value);                                                        \
      YYPOPSTACK(yylen);                                                       \
      yystate = *yyssp;                                                        \
      goto yybackup;                                                           \
    } else {                                                                   \
      yyerror(YY_("syntax error: cannot back up"));                            \
      YYERROR;                                                                 \
    }                                                                          \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(Current, Rhs, N)                                        \
  do                                                                           \
    if (N) {                                                                   \
      (Current).first_line = YYRHSLOC(Rhs, 1).first_line;                      \
      (Current).first_column = YYRHSLOC(Rhs, 1).first_column;                  \
      (Current).last_line = YYRHSLOC(Rhs, N).last_line;                        \
      (Current).last_column = YYRHSLOC(Rhs, N).last_column;                    \
    } else {                                                                   \
      (Current).first_line = (Current).last_line = YYRHSLOC(Rhs, 0).last_line; \
      (Current).first_column = (Current).last_column =                         \
          YYRHSLOC(Rhs, 0).last_column;                                        \
    }                                                                          \
  while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

#ifndef YYFPRINTF
#include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#define YYFPRINTF fprintf
#endif

#define YYDPRINTF(Args)                                                        \
  do {                                                                         \
    if (yydebug)                                                               \
      YYFPRINTF Args;                                                          \
  } while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YYLOCATION_PRINT

#if defined YY_LOCATION_PRINT

/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YYLOCATION_PRINT(File, Loc) YY_LOCATION_PRINT(File, *(Loc))

#elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int yy_location_print_(int yyo, YYLTYPE const *const yylocp) {
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line) {
    res += YYFPRINTF(yyo, "%d", yylocp->first_line);
    if (0 <= yylocp->first_column)
      res += YYFPRINTF(yyo, ".%d", yylocp->first_column);
  }
  if (0 <= yylocp->last_line) {
    if (yylocp->first_line < yylocp->last_line) {
      res += YYFPRINTF(yyo, "-%d", yylocp->last_line);
      if (0 <= end_col)
        res += YYFPRINTF(yyo, ".%d", end_col);
    } else if (0 <= end_col && yylocp->first_column < end_col)
      res += YYFPRINTF(yyo, "-%d", end_col);
  }
  return res;
}

#define YYLOCATION_PRINT yy_location_print_

/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YY_LOCATION_PRINT(File, Loc) YYLOCATION_PRINT(File, &(Loc))

#else

#define YYLOCATION_PRINT(File, Loc) ((void) 0)
/* Temporary convenience wrapper in case some people defined the
   undocumented and private YY_LOCATION_PRINT macros.  */
#define YY_LOCATION_PRINT YYLOCATION_PRINT

#endif
#endif /* !defined YYLOCATION_PRINT */


#define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                          \
  do {                                                                         \
    if (yydebug) {                                                             \
      YYFPRINTF(stderr, "%s ", Title);                                         \
      yy_symbol_print(stderr, Kind, Value, Location);                          \
      YYFPRINTF(stderr, "\n");                                                 \
    }                                                                          \
  } while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void yy_symbol_value_print(int yyo, yysymbol_kind_t yykind,
                                  YYSTYPE const *const yyvaluep,
                                  YYLTYPE const *const yylocationp) {
  int yyoutput = yyo;
  YY_USE(yyoutput);
  YY_USE(yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE(yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void yy_symbol_print(int yyo, yysymbol_kind_t yykind,
                            YYSTYPE const *const yyvaluep,
                            YYLTYPE const *const yylocationp) {
  YYFPRINTF(yyo, "%s %s (", yykind < YYNTOKENS ? "token" : "nterm",
            yysymbol_name(yykind));

  YYLOCATION_PRINT(yyo, yylocationp);
  YYFPRINTF(yyo, ": ");
  yy_symbol_value_print(yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF(yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void yy_stack_print(yy_state_t *yybottom, yy_state_t *yytop) {
  YYFPRINTF(stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++) {
    int yybot = *yybottom;
    YYFPRINTF(stderr, " %d", yybot);
  }
  YYFPRINTF(stderr, "\n");
}

#define YY_STACK_PRINT(Bottom, Top)                                            \
  do {                                                                         \
    if (yydebug)                                                               \
      yy_stack_print((Bottom), (Top));                                         \
  } while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void yy_reduce_print(yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                            int yyrule) {
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF(stderr, "Reducing stack by rule %d (line %d):\n", yyrule - 1,
            yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++) {
    YYFPRINTF(stderr, "   $%d = ", yyi + 1);
    yy_symbol_print(stderr, YY_ACCESSING_SYMBOL(+yyssp[yyi + 1 - yynrhs]),
                    &yyvsp[(yyi + 1) - (yynrhs)],
                    &(yylsp[(yyi + 1) - (yynrhs)]));
    YYFPRINTF(stderr, "\n");
  }
}

#define YY_REDUCE_PRINT(Rule)                                                  \
  do {                                                                         \
    if (yydebug)                                                               \
      yy_reduce_print(yyssp, yyvsp, yylsp, Rule);                              \
  } while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
#define YYDPRINTF(Args) ((void) 0)
#define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
#define YY_STACK_PRINT(Bottom, Top)
#define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void yydestruct(const char *yymsg, yysymbol_kind_t yykind,
                       YYSTYPE *yyvaluep, YYLTYPE *yylocationp) {
  YY_USE(yyvaluep);
  YY_USE(yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT(yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE(yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
    = {1, 1, 1, 1}
#endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int yyparse(void) {
  yy_state_fast_t yystate = 0;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus = 0;

  /* Refer to the stacks through separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* Their size.  */
  YYPTRDIFF_T yystacksize = YYINITDEPTH;

  /* The state stack: array, bottom, top.  */
  // yy_state_t yyssa[YYINITDEPTH];
  yy_state_t *yyssa = malloc(sizeof(yy_state_t) * YYINITDEPTH);
  yy_state_t *yyss = yyssa;
  yy_state_t *yyssp = yyss;

  /* The semantic value stack: array, bottom, top.  */
  YYSTYPE *yyvsa = malloc(sizeof(YYSTYPE) * YYINITDEPTH);
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp = yyvs;

  /* The location stack: array, bottom, top.  */
  YYLTYPE *yylsa = malloc(sizeof(YYLTYPE) * YYINITDEPTH);
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];


#define YYPOPSTACK(N) (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF((stderr, "Entering state %d\n", yystate));
  YY_ASSERT(0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST(yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT(yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
  {
    /* Get the current used size of the three stacks, in elements.  */
    YYPTRDIFF_T yysize = yyssp - yyss + 1;

#if defined yyoverflow
    {
      /* Give user a chance to reallocate the stack.  Use copies of
         these so that the &'s don't force the real ones into
         memory.  */
      yy_state_t *yyss1 = yyss;
      YYSTYPE *yyvs1 = yyvs;
      YYLTYPE *yyls1 = yyls;

      /* Each stack pointer address is followed by the size of the
         data in use in that stack, in bytes.  This used to be a
         conditional around just the two extra args, but that might
         be undefined if yyoverflow is a macro.  */
      yyoverflow(YY_("memory exhausted"), &yyss1, yysize * YYSIZEOF(*yyssp),
                 &yyvs1, yysize * YYSIZEOF(*yyvsp), &yyls1,
                 yysize * YYSIZEOF(*yylsp), &yystacksize);
      yyss = yyss1;
      yyvs = yyvs1;
      yyls = yyls1;
    }
#else /* defined YYSTACK_RELOCATE */
    /* Extend the stack our own way.  */
    if (YYMAXDEPTH <= yystacksize)
      YYNOMEM;
    yystacksize *= 2;
    if (YYMAXDEPTH < yystacksize)
      yystacksize = YYMAXDEPTH;

    {
      yy_state_t *yyss1 = yyss;
      union yyalloc *yyptr =
          YY_CAST(union yyalloc *,
                  YYSTACK_ALLOC(YY_CAST(YYSIZE_T, YYSTACK_BYTES(yystacksize))));
      if (!yyptr)
        YYNOMEM;
      YYSTACK_RELOCATE(yyss_alloc, yyss);
      YYSTACK_RELOCATE(yyvs_alloc, yyvs);
      YYSTACK_RELOCATE(yyls_alloc, yyls);
#undef YYSTACK_RELOCATE
      if (yyss1 != yyssa)
        YYSTACK_FREE(yyss1);
    }
#endif

    yyssp = yyss + yysize - 1;
    yyvsp = yyvs + yysize - 1;
    yylsp = yyls + yysize - 1;

    YY_IGNORE_USELESS_CAST_BEGIN
    YYDPRINTF(
        (stderr, "Stack size increased to %ld\n", YY_CAST(long, yystacksize)));
    YY_IGNORE_USELESS_CAST_END

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
  if (yypact_value_is_default(yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY) {
    YYDPRINTF((stderr, "Reading a token\n"));
    yychar = yylex();
  }

  if (yychar <= YYEOF) {
    yychar = YYEOF;
    yytoken = YYSYMBOL_YYEOF;
    YYDPRINTF((stderr, "Now at end of input.\n"));
  } else if (yychar == YYerror) {
    /* The scanner already issued an error message, process directly
       to error recovery.  But do not keep the error token as
       lookahead, it is too special and may lead us to an endless
       loop in error recovery. */
    yychar = YYUNDEF;
    yytoken = YYSYMBOL_YYerror;
    yyerror_range[1] = yylloc;
    goto yyerrlab1;
  } else {
    yytoken = YYTRANSLATE(yychar);
    YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
  }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0) {
    if (yytable_value_is_error(yyn))
      goto yyerrlab;
    yyn = -yyn;
    goto yyreduce;
  }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1 - yylen];

  /* Default location. */
  YYLLOC_DEFAULT(yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT(yyn);
  switch (yyn) {
    case 2: /* program: exp  */
#line 82 "tiger.y"
    {
      (yyval.exp) = absyn_root = (yyvsp[0].exp);
    }
#line 1392 "xv6_tiger.tab.c"
    break;

    case 3: /* exp: lvalue  */
#line 87 "tiger.y"
    {
      (yyval.exp) = A_VarExp((yyloc), (yyvsp[0].var));
    }
#line 1398 "xv6_tiger.tab.c"
    break;

    case 4: /* exp: NIL  */
#line 89 "tiger.y"
    {
      (yyval.exp) = A_NilExp((yyloc));
    }
#line 1404 "xv6_tiger.tab.c"
    break;

    case 5: /* exp: LPAREN exp_sequence RPAREN  */
#line 92 "tiger.y"
    {
      (yyval.exp) = A_SeqExp((yylsp[-2]), (yyvsp[-1].exp_list));
    }
#line 1410 "xv6_tiger.tab.c"
    break;

    case 6: /* exp: LPAREN RPAREN  */
#line 94 "tiger.y"
    {
      (yyval.exp) = NULL;
    }
#line 1416 "xv6_tiger.tab.c"
    break;

    case 7: /* exp: INT  */
#line 96 "tiger.y"
    {
      (yyval.exp) = A_IntExp((yyloc), (yyvsp[0].ival));
    }
#line 1422 "xv6_tiger.tab.c"
    break;

    case 8: /* exp: STRING  */
#line 98 "tiger.y"
    {
      (yyval.exp) = A_StringExp((yyloc), (yyvsp[0].sval));
    }
#line 1428 "xv6_tiger.tab.c"
    break;

    case 9: /* exp: MINUS exp  */
#line 101 "tiger.y"
    {
      (yyval.exp) =
          A_OpExp((yyloc), A_minusOp, A_IntExp((yyloc), 0), (yyvsp[0].exp));
    }
#line 1434 "xv6_tiger.tab.c"
    break;

    case 10: /* exp: ID LPAREN RPAREN  */
#line 104 "tiger.y"
    {
      (yyval.exp) = A_CallExp((yyloc), S_Symbol((yyvsp[-2].sval)), NULL);
    }
#line 1440 "xv6_tiger.tab.c"
    break;

    case 11: /* exp: ID LPAREN exp_list RPAREN  */
#line 106 "tiger.y"
    {
      (yyval.exp) =
          A_CallExp((yyloc), S_Symbol((yyvsp[-3].sval)), (yyvsp[-1].exp_list));
    }
#line 1446 "xv6_tiger.tab.c"
    break;

    case 12: /* exp: exp PLUS exp  */
#line 109 "tiger.y"
    {
      (yyval.exp) = A_OpExp((yyloc), A_plusOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1452 "xv6_tiger.tab.c"
    break;

    case 13: /* exp: exp MINUS exp  */
#line 111 "tiger.y"
    {
      (yyval.exp) =
          A_OpExp((yyloc), A_minusOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1458 "xv6_tiger.tab.c"
    break;

    case 14: /* exp: exp TIMES exp  */
#line 113 "tiger.y"
    {
      (yyval.exp) =
          A_OpExp((yyloc), A_timesOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1464 "xv6_tiger.tab.c"
    break;

    case 15: /* exp: exp DIVIDE exp  */
#line 115 "tiger.y"
    {
      (yyval.exp) =
          A_OpExp((yyloc), A_divideOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1470 "xv6_tiger.tab.c"
    break;

    case 16: /* exp: exp EQ exp  */
#line 118 "tiger.y"
    {
      (yyval.exp) = A_OpExp((yyloc), A_eqOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1476 "xv6_tiger.tab.c"
    break;

    case 17: /* exp: exp NEQ exp  */
#line 120 "tiger.y"
    {
      (yyval.exp) = A_OpExp((yyloc), A_neqOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1482 "xv6_tiger.tab.c"
    break;

    case 18: /* exp: exp GT exp  */
#line 122 "tiger.y"
    {
      (yyval.exp) = A_OpExp((yyloc), A_gtOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1488 "xv6_tiger.tab.c"
    break;

    case 19: /* exp: exp LT exp  */
#line 124 "tiger.y"
    {
      (yyval.exp) = A_OpExp((yyloc), A_ltOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1494 "xv6_tiger.tab.c"
    break;

    case 20: /* exp: exp GE exp  */
#line 126 "tiger.y"
    {
      (yyval.exp) = A_OpExp((yyloc), A_geOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1500 "xv6_tiger.tab.c"
    break;

    case 21: /* exp: exp LE exp  */
#line 128 "tiger.y"
    {
      (yyval.exp) = A_OpExp((yyloc), A_leOp, (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1506 "xv6_tiger.tab.c"
    break;

    case 22: /* exp: exp AND exp  */
#line 131 "tiger.y"
    {
      (yyval.exp) = A_IfExp((yyloc), (yyvsp[-2].exp), (yyvsp[0].exp),
                            A_IntExp((yylsp[0]), 0));
    }
#line 1512 "xv6_tiger.tab.c"
    break;

    case 23: /* exp: exp OR exp  */
#line 133 "tiger.y"
    {
      (yyval.exp) = A_IfExp((yyloc), (yyvsp[-2].exp), A_IntExp((yylsp[-1]), 1),
                            (yyvsp[0].exp));
    }
#line 1518 "xv6_tiger.tab.c"
    break;

    case 24: /* exp: ID LBRACE record_list RBRACE  */
#line 136 "tiger.y"
    {
      (yyval.exp) = A_RecordExp((yyloc), S_Symbol((yyvsp[-3].sval)),
                                (yyvsp[-1].record_list));
    }
#line 1524 "xv6_tiger.tab.c"
    break;

    case 25: /* exp: ID LBRACE RBRACE  */
#line 138 "tiger.y"
    {
      (yyval.exp) = A_RecordExp((yyloc), S_Symbol((yyvsp[-2].sval)), NULL);
    }
#line 1530 "xv6_tiger.tab.c"
    break;

    case 26: /* exp: ID LBRACK exp RBRACK OF exp  */
#line 141 "tiger.y"
    {
      (yyval.exp) = A_ArrayExp((yyloc), S_Symbol((yyvsp[-5].sval)),
                               (yyvsp[-3].exp), (yyvsp[0].exp));
    }
#line 1536 "xv6_tiger.tab.c"
    break;

    case 27: /* exp: lvalue ASSIGN exp  */
#line 144 "tiger.y"
    {
      (yyval.exp) = A_AssignExp((yyloc), (yyvsp[-2].var), (yyvsp[0].exp));
    }
#line 1542 "xv6_tiger.tab.c"
    break;

    case 28: /* exp: IF exp THEN exp  */
#line 147 "tiger.y"
    {
      (yyval.exp) = A_IfExp((yyloc), (yyvsp[-2].exp), (yyvsp[0].exp), NULL);
    }
#line 1548 "xv6_tiger.tab.c"
    break;

    case 29: /* exp: IF exp THEN exp ELSE exp  */
#line 149 "tiger.y"
    {
      (yyval.exp) =
          A_IfExp((yyloc), (yyvsp[-4].exp), (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1554 "xv6_tiger.tab.c"
    break;

    case 30: /* exp: WHILE exp DO exp  */
#line 152 "tiger.y"
    {
      (yyval.exp) = A_WhileExp((yyloc), (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1560 "xv6_tiger.tab.c"
    break;

    case 31: /* exp: FOR ID ASSIGN exp TO exp DO exp  */
#line 155 "tiger.y"
    {
      (yyval.exp) = A_ForExp((yyloc), S_Symbol((yyvsp[-6].sval)),
                             (yyvsp[-4].exp), (yyvsp[-2].exp), (yyvsp[0].exp));
    }
#line 1566 "xv6_tiger.tab.c"
    break;

    case 32: /* exp: BREAK  */
#line 158 "tiger.y"
    {
      (yyval.exp) = A_BreakExp((yyloc));
    }
#line 1572 "xv6_tiger.tab.c"
    break;

    case 33: /* exp: LET decs IN expseq END  */
#line 161 "tiger.y"
    {
      (yyval.exp) = A_LetExp((yyloc), (yyvsp[-3].decs),
                             A_SeqExp((yylsp[-1]), (yyvsp[-1].exp_list)));
    }
#line 1578 "xv6_tiger.tab.c"
    break;

    case 34: /* exp: LPAREN exp RPAREN  */
#line 164 "tiger.y"
    {
      (yyval.exp) = (yyvsp[-1].exp);
    }
#line 1584 "xv6_tiger.tab.c"
    break;

    case 35: /* expseq: %empty  */
#line 168 "tiger.y"
    {
      (yyval.exp_list) = NULL;
    }
#line 1590 "xv6_tiger.tab.c"
    break;

    case 36: /* expseq: exp  */
#line 170 "tiger.y"
    {
      (yyval.exp_list) = A_ExpList((yyvsp[0].exp), NULL);
    }
#line 1596 "xv6_tiger.tab.c"
    break;

    case 37: /* expseq: exp SEMICOLON expseq  */
#line 172 "tiger.y"
    {
      (yyval.exp_list) = A_ExpList((yyvsp[-2].exp), (yyvsp[0].exp_list));
    }
#line 1602 "xv6_tiger.tab.c"
    break;

    case 38: /* record_list: ID EQ exp  */
#line 176 "tiger.y"
    {
      (yyval.record_list) = A_EfieldList(
          A_Efield(S_Symbol((yyvsp[-2].sval)), (yyvsp[0].exp)), NULL);
    }
#line 1608 "xv6_tiger.tab.c"
    break;

    case 39: /* record_list: ID EQ exp COMMA record_list  */
#line 178 "tiger.y"
    {
      (yyval.record_list) =
          A_EfieldList(A_Efield(S_Symbol((yyvsp[-4].sval)), (yyvsp[-2].exp)),
                       (yyvsp[0].record_list));
    }
#line 1614 "xv6_tiger.tab.c"
    break;

    case 40: /* exp_list: exp  */
#line 182 "tiger.y"
    {
      (yyval.exp_list) = A_ExpList((yyvsp[0].exp), NULL);
    }
#line 1620 "xv6_tiger.tab.c"
    break;

    case 41: /* exp_list: exp COMMA exp_list  */
#line 184 "tiger.y"
    {
      (yyval.exp_list) = A_ExpList((yyvsp[-2].exp), (yyvsp[0].exp_list));
    }
#line 1626 "xv6_tiger.tab.c"
    break;

    case 42: /* exp_sequence: exp SEMICOLON exp  */
#line 188 "tiger.y"
    {
      (yyval.exp_list) =
          A_ExpList((yyvsp[-2].exp), A_ExpList((yyvsp[0].exp), NULL));
    }
#line 1632 "xv6_tiger.tab.c"
    break;

    case 43: /* exp_sequence: exp SEMICOLON exp_sequence  */
#line 190 "tiger.y"
    {
      (yyval.exp_list) = A_ExpList((yyvsp[-2].exp), (yyvsp[0].exp_list));
    }
#line 1638 "xv6_tiger.tab.c"
    break;

    case 44: /* decs: %empty  */
#line 194 "tiger.y"
    {
      (yyval.decs) = NULL;
    }
#line 1644 "xv6_tiger.tab.c"
    break;

    case 45: /* decs: dec decs  */
#line 196 "tiger.y"
    {
      (yyval.decs) = A_DecList((yyvsp[-1].dec), (yyvsp[0].decs));
    }
#line 1650 "xv6_tiger.tab.c"
    break;

    case 46: /* dec: tydecs  */
#line 200 "tiger.y"
    {
      (yyval.dec) = A_TypeDec((yyloc), (yyvsp[0].tydecs));
    }
#line 1656 "xv6_tiger.tab.c"
    break;

    case 47: /* dec: vardec  */
#line 202 "tiger.y"
    {
      (yyval.dec) = (yyvsp[0].dec);
    }
#line 1662 "xv6_tiger.tab.c"
    break;

    case 48: /* dec: fundecs  */
#line 204 "tiger.y"
    {
      (yyval.dec) = A_FunctionDec((yyloc), (yyvsp[0].fundecs));
    }
#line 1668 "xv6_tiger.tab.c"
    break;

    case 49: /* tydecs: tydec  */
#line 208 "tiger.y"
    {
      (yyval.tydecs) = A_NametyList((yyvsp[0].tydec), NULL);
    }
#line 1674 "xv6_tiger.tab.c"
    break;

    case 50: /* tydecs: tydec tydecs  */
#line 210 "tiger.y"
    {
      (yyval.tydecs) = A_NametyList((yyvsp[-1].tydec), (yyvsp[0].tydecs));
    }
#line 1680 "xv6_tiger.tab.c"
    break;

    case 51: /* tydec: TYPE ID EQ ty  */
#line 214 "tiger.y"
    {
      (yyval.tydec) = A_Namety(S_Symbol((yyvsp[-2].sval)), (yyvsp[0].ty));
    }
#line 1686 "xv6_tiger.tab.c"
    break;

    case 52: /* ty: ID  */
#line 218 "tiger.y"
    {
      (yyval.ty) = A_NameTy((yyloc), S_Symbol((yyvsp[0].sval)));
    }
#line 1692 "xv6_tiger.tab.c"
    break;

    case 53: /* ty: LBRACE tyfields RBRACE  */
#line 220 "tiger.y"
    {
      (yyval.ty) = A_RecordTy((yyloc), (yyvsp[-1].tyfields));
    }
#line 1698 "xv6_tiger.tab.c"
    break;

    case 54: /* ty: ARRAY OF ID  */
#line 222 "tiger.y"
    {
      (yyval.ty) = A_ArrayTy((yyloc), S_Symbol((yyvsp[0].sval)));
    }
#line 1704 "xv6_tiger.tab.c"
    break;

    case 55: /* tyfields: %empty  */
#line 226 "tiger.y"
    {
      (yyval.tyfields) = NULL;
    }
#line 1710 "xv6_tiger.tab.c"
    break;

    case 56: /* tyfields: ID COLON ID  */
#line 228 "tiger.y"
    {
      (yyval.tyfields) =
          A_FieldList(A_Field((yylsp[-2]), S_Symbol((yyvsp[-2].sval)),
                              S_Symbol((yyvsp[0].sval))),
                      NULL);
    }
#line 1716 "xv6_tiger.tab.c"
    break;

    case 57: /* tyfields: ID COLON ID COMMA tyfields  */
#line 230 "tiger.y"
    {
      (yyval.tyfields) =
          A_FieldList(A_Field((yylsp[-4]), S_Symbol((yyvsp[-4].sval)),
                              S_Symbol((yyvsp[-2].sval))),
                      (yyvsp[0].tyfields));
    }
#line 1722 "xv6_tiger.tab.c"
    break;

    case 58: /* vardec: VAR ID ASSIGN exp  */
#line 234 "tiger.y"
    {
      (yyval.dec) =
          A_VarDec((yyloc), S_Symbol((yyvsp[-2].sval)), NULL, (yyvsp[0].exp));
    }
#line 1728 "xv6_tiger.tab.c"
    break;

    case 59: /* vardec: VAR ID COLON ID ASSIGN exp  */
#line 236 "tiger.y"
    {
      (yyval.dec) = A_VarDec((yyloc), S_Symbol((yyvsp[-4].sval)),
                             S_Symbol((yyvsp[-2].sval)), (yyvsp[0].exp));
    }
#line 1734 "xv6_tiger.tab.c"
    break;

    case 60: /* fundecs: fundec  */
#line 240 "tiger.y"
    {
      (yyval.fundecs) = A_FundecList((yyvsp[0].fundec), NULL);
    }
#line 1740 "xv6_tiger.tab.c"
    break;

    case 61: /* fundecs: fundec fundecs  */
#line 242 "tiger.y"
    {
      (yyval.fundecs) = A_FundecList((yyvsp[-1].fundec), (yyvsp[0].fundecs));
    }
#line 1746 "xv6_tiger.tab.c"
    break;

    case 62: /* fundec: FUNCTION ID LPAREN tyfields RPAREN EQ exp  */
#line 246 "tiger.y"
    {
      (yyval.fundec) = A_Fundec((yyloc), S_Symbol((yyvsp[-5].sval)),
                                (yyvsp[-3].tyfields), NULL, (yyvsp[0].exp));
    }
#line 1752 "xv6_tiger.tab.c"
    break;

    case 63: /* fundec: FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp  */
#line 248 "tiger.y"
    {
      (yyval.fundec) =
          A_Fundec((yyloc), S_Symbol((yyvsp[-7].sval)), (yyvsp[-5].tyfields),
                   S_Symbol((yyvsp[-2].sval)), (yyvsp[0].exp));
    }
#line 1758 "xv6_tiger.tab.c"
    break;

    case 64: /* lvalue: ID  */
#line 252 "tiger.y"
    {
      (yyval.var) = A_SimpleVar((yyloc), S_Symbol((yyvsp[0].sval)));
    }
#line 1764 "xv6_tiger.tab.c"
    break;

    case 65: /* lvalue: lvalue DOT ID  */
#line 254 "tiger.y"
    {
      (yyval.var) =
          A_FieldVar((yyloc), (yyvsp[-2].var), S_Symbol((yyvsp[0].sval)));
    }
#line 1770 "xv6_tiger.tab.c"
    break;

    case 66: /* lvalue: lvalue LBRACK exp RBRACK  */
#line 256 "tiger.y"
    {
      (yyval.var) = A_SubscriptVar((yyloc), (yyvsp[-3].var), (yyvsp[-1].exp));
    }
#line 1776 "xv6_tiger.tab.c"
    break;

    case 67: /* lvalue: ID LBRACK exp RBRACK  */
#line 258 "tiger.y"
    {
      (yyval.var) = A_SubscriptVar(
          (yyloc), A_SimpleVar((yylsp[-3]), S_Symbol((yyvsp[-3].sval))),
          (yyvsp[-1].exp));
    }
#line 1782 "xv6_tiger.tab.c"
    break;


#line 1786 "xv6_tiger.tab.c"

    default: break;
  }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT("-> $$ =", YY_CAST(yysymbol_kind_t, yyr1[yyn]), &yyval,
                  &yyloc);

  YYPOPSTACK(yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
                   ? yytable[yyi]
                   : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE(yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus) {
    ++yynerrs;
    yyerror(YY_("syntax error"));
  }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3) {
    /* If just tried and failed to reuse lookahead token after an
       error, discard it.  */

    if (yychar <= YYEOF) {
      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        YYABORT;
    } else {
      yydestruct("Error: discarding", yytoken, &yylval, &yylloc);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3; /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;) {
    yyn = yypact[yystate];
    if (!yypact_value_is_default(yyn)) {
      yyn += YYSYMBOL_YYerror;
      if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror) {
        yyn = yytable[yyn];
        if (0 < yyn)
          break;
      }
    }

    /* Pop the current state because it cannot handle the error token.  */
    if (yyssp == yyss)
      YYABORT;

    yyerror_range[1] = *yylsp;
    yydestruct("Error: popping", YY_ACCESSING_SYMBOL(yystate), yyvsp, yylsp);
    YYPOPSTACK(1);
    yystate = *yyssp;
    YY_STACK_PRINT(yyss, yyssp);
  }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT(*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT("Shifting", YY_ACCESSING_SYMBOL(yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror(YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY) {
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = YYTRANSLATE(yychar);
    yydestruct("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);
  }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK(yylen);
  YY_STACK_PRINT(yyss, yyssp);
  while (yyssp != yyss) {
    yydestruct("Cleanup: popping", YY_ACCESSING_SYMBOL(+*yyssp), yyvsp, yylsp);
    YYPOPSTACK(1);
  }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE(yyss);
#endif

  return yyresult;
}

#line 259 "tiger.y"

void yyerror(char *s) { EM_error(yylloc, "%s", s); }
