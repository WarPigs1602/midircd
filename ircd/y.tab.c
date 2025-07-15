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
#line 22 "./ircd_parser.y"


#include "config.h"
#include "s_conf.h"
#include "class.h"
#include "client.h"
#include "crule.h"
#include "ircd_features.h"
#include "fileio.h"
#include "gline.h"
#include "hash.h"
#include "ircd.h"
#include "ircd_alloc.h"
#include "ircd_chattr.h"
#include "ircd_log.h"
#include "ircd_reply.h"
#include "ircd_snprintf.h"
#include "ircd_string.h"
#include "ircd_tls.h"
#include "list.h"
#include "listener.h"
#include "match.h"
#include "motd.h"
#include "numeric.h"
#include "numnicks.h"
#include "opercmds.h"
#include "parse.h"
#include "res.h"
#include "s_auth.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "s_debug.h"
#include "s_misc.h"
#include "send.h"
#include "struct.h"
#include "sys.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_STRINGS 80 /* Maximum number of feature params. */
#define USE_IPV4 (1 << 16)
#define USE_IPV6 (1 << 17)

  extern struct LocalConf   localConf;
  extern struct DenyConf*   denyConfList;
  extern struct CRuleConf*  cruleConfList;
  extern struct ServerConf* serverConfList;
  extern struct s_map*      GlobalServiceMapList;
  extern struct qline*      GlobalQuarantineList;
  extern struct sline*      GlobalSList;

  int yylex(void);
  /* Now all the globals we need :/... */
  int tping, tconn, maxlinks, sendq, port, invert, stringno, flags;
  char *name, *pass, *host, *ip, *username, *origin, *hub_limit;
  char *tls_certfile, *tls_ciphers, *tls_fingerprint, *tls_keyfile;
  struct SLink *hosts;
  char *stringlist[MAX_STRINGS];
  struct ListenerFlags listen_flags;
  struct ConnectionClass *c_class;
  struct DenyConf *dconf;
  struct ServerConf *sconf;
  struct s_map *smap;
  struct sline *spoof;
  struct Privs privs;
  struct Privs privs_dirty;

static void parse_error(char *pattern,...) {
  static char error_buffer[1024];
  va_list vl;
  va_start(vl,pattern);
  ircd_vsnprintf(NULL, error_buffer, sizeof(error_buffer), pattern, vl);
  va_end(vl);
  yyerror(error_buffer);
}

static void free_slist(struct SLink **link) {
  struct SLink *next;
  while (*link != NULL) {
    next = (*link)->next;
    MyFree((*link)->value.cp);
    free_link(*link);
    *link = next;
  }
}


#line 161 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    QSTRING = 258,                 /* QSTRING  */
    NUMBER = 259,                  /* NUMBER  */
    GENERAL = 260,                 /* GENERAL  */
    ADMIN = 261,                   /* ADMIN  */
    LOCATION = 262,                /* LOCATION  */
    CONTACT = 263,                 /* CONTACT  */
    CONNECT = 264,                 /* CONNECT  */
    CLASS = 265,                   /* CLASS  */
    CHANNEL = 266,                 /* CHANNEL  */
    PINGFREQ = 267,                /* PINGFREQ  */
    CONNECTFREQ = 268,             /* CONNECTFREQ  */
    MAXLINKS = 269,                /* MAXLINKS  */
    MAXHOPS = 270,                 /* MAXHOPS  */
    SENDQ = 271,                   /* SENDQ  */
    NAME = 272,                    /* NAME  */
    HOST = 273,                    /* HOST  */
    IP = 274,                      /* IP  */
    USERNAME = 275,                /* USERNAME  */
    PASS = 276,                    /* PASS  */
    LOCAL = 277,                   /* LOCAL  */
    SECONDS = 278,                 /* SECONDS  */
    MINUTES = 279,                 /* MINUTES  */
    HOURS = 280,                   /* HOURS  */
    DAYS = 281,                    /* DAYS  */
    WEEKS = 282,                   /* WEEKS  */
    MONTHS = 283,                  /* MONTHS  */
    YEARS = 284,                   /* YEARS  */
    DECADES = 285,                 /* DECADES  */
    BYTES = 286,                   /* BYTES  */
    KBYTES = 287,                  /* KBYTES  */
    MBYTES = 288,                  /* MBYTES  */
    GBYTES = 289,                  /* GBYTES  */
    TBYTES = 290,                  /* TBYTES  */
    SERVER = 291,                  /* SERVER  */
    PORT = 292,                    /* PORT  */
    MASK = 293,                    /* MASK  */
    HUB = 294,                     /* HUB  */
    LEAF = 295,                    /* LEAF  */
    UWORLD = 296,                  /* UWORLD  */
    YES = 297,                     /* YES  */
    NO = 298,                      /* NO  */
    OPER = 299,                    /* OPER  */
    VHOST = 300,                   /* VHOST  */
    HIDDEN = 301,                  /* HIDDEN  */
    MOTD = 302,                    /* MOTD  */
    JUPE = 303,                    /* JUPE  */
    NICK = 304,                    /* NICK  */
    NUMERIC = 305,                 /* NUMERIC  */
    DESCRIPTION = 306,             /* DESCRIPTION  */
    CLIENT = 307,                  /* CLIENT  */
    KILL = 308,                    /* KILL  */
    CRULE = 309,                   /* CRULE  */
    REAL = 310,                    /* REAL  */
    REASON = 311,                  /* REASON  */
    TFILE = 312,                   /* TFILE  */
    RULE = 313,                    /* RULE  */
    ALL = 314,                     /* ALL  */
    FEATURES = 315,                /* FEATURES  */
    QUARANTINE = 316,              /* QUARANTINE  */
    PSEUDO = 317,                  /* PSEUDO  */
    PREPEND = 318,                 /* PREPEND  */
    USERMODE = 319,                /* USERMODE  */
    IAUTH = 320,                   /* IAUTH  */
    TIMEOUT = 321,                 /* TIMEOUT  */
    FAST = 322,                    /* FAST  */
    AUTOCONNECT = 323,             /* AUTOCONNECT  */
    PROGRAM = 324,                 /* PROGRAM  */
    SPOOFHOST = 325,               /* SPOOFHOST  */
    TOK_IPV4 = 326,                /* TOK_IPV4  */
    TOK_IPV6 = 327,                /* TOK_IPV6  */
    DNS = 328,                     /* DNS  */
    TLS = 329,                     /* TLS  */
    CERTFILE = 330,                /* CERTFILE  */
    CIPHERS = 331,                 /* CIPHERS  */
    FINGERPRINT = 332,             /* FINGERPRINT  */
    KEYFILE = 333,                 /* KEYFILE  */
    TPRIV_CHAN_LIMIT = 334,        /* TPRIV_CHAN_LIMIT  */
    TPRIV_MODE_LCHAN = 335,        /* TPRIV_MODE_LCHAN  */
    TPRIV_DEOP_LCHAN = 336,        /* TPRIV_DEOP_LCHAN  */
    TPRIV_WALK_LCHAN = 337,        /* TPRIV_WALK_LCHAN  */
    TPRIV_LOCAL_KILL = 338,        /* TPRIV_LOCAL_KILL  */
    TPRIV_REHASH = 339,            /* TPRIV_REHASH  */
    TPRIV_RESTART = 340,           /* TPRIV_RESTART  */
    TPRIV_DIE = 341,               /* TPRIV_DIE  */
    TPRIV_GLINE = 342,             /* TPRIV_GLINE  */
    TPRIV_LOCAL_GLINE = 343,       /* TPRIV_LOCAL_GLINE  */
    TPRIV_LOCAL_JUPE = 344,        /* TPRIV_LOCAL_JUPE  */
    TPRIV_LOCAL_BADCHAN = 345,     /* TPRIV_LOCAL_BADCHAN  */
    TPRIV_LOCAL_OPMODE = 346,      /* TPRIV_LOCAL_OPMODE  */
    TPRIV_OPMODE = 347,            /* TPRIV_OPMODE  */
    TPRIV_SET = 348,               /* TPRIV_SET  */
    TPRIV_WHOX = 349,              /* TPRIV_WHOX  */
    TPRIV_BADCHAN = 350,           /* TPRIV_BADCHAN  */
    TPRIV_SEE_CHAN = 351,          /* TPRIV_SEE_CHAN  */
    TPRIV_SHOW_INVIS = 352,        /* TPRIV_SHOW_INVIS  */
    TPRIV_SHOW_ALL_INVIS = 353,    /* TPRIV_SHOW_ALL_INVIS  */
    TPRIV_PROPAGATE = 354,         /* TPRIV_PROPAGATE  */
    TPRIV_UNLIMIT_QUERY = 355,     /* TPRIV_UNLIMIT_QUERY  */
    TPRIV_DISPLAY = 356,           /* TPRIV_DISPLAY  */
    TPRIV_SEE_OPERS = 357,         /* TPRIV_SEE_OPERS  */
    TPRIV_WIDE_GLINE = 358,        /* TPRIV_WIDE_GLINE  */
    TPRIV_FORCE_OPMODE = 359,      /* TPRIV_FORCE_OPMODE  */
    TPRIV_FORCE_LOCAL_OPMODE = 360, /* TPRIV_FORCE_LOCAL_OPMODE  */
    TPRIV_APASS_OPMODE = 361,      /* TPRIV_APASS_OPMODE  */
    TPRIV_CHANSERV = 362,          /* TPRIV_CHANSERV  */
    TPRIV_XTRA_OPER = 363,         /* TPRIV_XTRA_OPER  */
    TPRIV_NOIDLE = 364,            /* TPRIV_NOIDLE  */
    TPRIV_FREEFORM = 365,          /* TPRIV_FREEFORM  */
    TPRIV_PARANOID = 366,          /* TPRIV_PARANOID  */
    TPRIV_CHECK = 367,             /* TPRIV_CHECK  */
    TPRIV_LIST_CHAN = 368          /* TPRIV_LIST_CHAN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define QSTRING 258
#define NUMBER 259
#define GENERAL 260
#define ADMIN 261
#define LOCATION 262
#define CONTACT 263
#define CONNECT 264
#define CLASS 265
#define CHANNEL 266
#define PINGFREQ 267
#define CONNECTFREQ 268
#define MAXLINKS 269
#define MAXHOPS 270
#define SENDQ 271
#define NAME 272
#define HOST 273
#define IP 274
#define USERNAME 275
#define PASS 276
#define LOCAL 277
#define SECONDS 278
#define MINUTES 279
#define HOURS 280
#define DAYS 281
#define WEEKS 282
#define MONTHS 283
#define YEARS 284
#define DECADES 285
#define BYTES 286
#define KBYTES 287
#define MBYTES 288
#define GBYTES 289
#define TBYTES 290
#define SERVER 291
#define PORT 292
#define MASK 293
#define HUB 294
#define LEAF 295
#define UWORLD 296
#define YES 297
#define NO 298
#define OPER 299
#define VHOST 300
#define HIDDEN 301
#define MOTD 302
#define JUPE 303
#define NICK 304
#define NUMERIC 305
#define DESCRIPTION 306
#define CLIENT 307
#define KILL 308
#define CRULE 309
#define REAL 310
#define REASON 311
#define TFILE 312
#define RULE 313
#define ALL 314
#define FEATURES 315
#define QUARANTINE 316
#define PSEUDO 317
#define PREPEND 318
#define USERMODE 319
#define IAUTH 320
#define TIMEOUT 321
#define FAST 322
#define AUTOCONNECT 323
#define PROGRAM 324
#define SPOOFHOST 325
#define TOK_IPV4 326
#define TOK_IPV6 327
#define DNS 328
#define TLS 329
#define CERTFILE 330
#define CIPHERS 331
#define FINGERPRINT 332
#define KEYFILE 333
#define TPRIV_CHAN_LIMIT 334
#define TPRIV_MODE_LCHAN 335
#define TPRIV_DEOP_LCHAN 336
#define TPRIV_WALK_LCHAN 337
#define TPRIV_LOCAL_KILL 338
#define TPRIV_REHASH 339
#define TPRIV_RESTART 340
#define TPRIV_DIE 341
#define TPRIV_GLINE 342
#define TPRIV_LOCAL_GLINE 343
#define TPRIV_LOCAL_JUPE 344
#define TPRIV_LOCAL_BADCHAN 345
#define TPRIV_LOCAL_OPMODE 346
#define TPRIV_OPMODE 347
#define TPRIV_SET 348
#define TPRIV_WHOX 349
#define TPRIV_BADCHAN 350
#define TPRIV_SEE_CHAN 351
#define TPRIV_SHOW_INVIS 352
#define TPRIV_SHOW_ALL_INVIS 353
#define TPRIV_PROPAGATE 354
#define TPRIV_UNLIMIT_QUERY 355
#define TPRIV_DISPLAY 356
#define TPRIV_SEE_OPERS 357
#define TPRIV_WIDE_GLINE 358
#define TPRIV_FORCE_OPMODE 359
#define TPRIV_FORCE_LOCAL_OPMODE 360
#define TPRIV_APASS_OPMODE 361
#define TPRIV_CHANSERV 362
#define TPRIV_XTRA_OPER 363
#define TPRIV_NOIDLE 364
#define TPRIV_FREEFORM 365
#define TPRIV_PARANOID 366
#define TPRIV_CHECK 367
#define TPRIV_LIST_CHAN 368

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 206 "./ircd_parser.y"

 char *text;
 int num;

#line 445 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_QSTRING = 3,                    /* QSTRING  */
  YYSYMBOL_NUMBER = 4,                     /* NUMBER  */
  YYSYMBOL_GENERAL = 5,                    /* GENERAL  */
  YYSYMBOL_ADMIN = 6,                      /* ADMIN  */
  YYSYMBOL_LOCATION = 7,                   /* LOCATION  */
  YYSYMBOL_CONTACT = 8,                    /* CONTACT  */
  YYSYMBOL_CONNECT = 9,                    /* CONNECT  */
  YYSYMBOL_CLASS = 10,                     /* CLASS  */
  YYSYMBOL_CHANNEL = 11,                   /* CHANNEL  */
  YYSYMBOL_PINGFREQ = 12,                  /* PINGFREQ  */
  YYSYMBOL_CONNECTFREQ = 13,               /* CONNECTFREQ  */
  YYSYMBOL_MAXLINKS = 14,                  /* MAXLINKS  */
  YYSYMBOL_MAXHOPS = 15,                   /* MAXHOPS  */
  YYSYMBOL_SENDQ = 16,                     /* SENDQ  */
  YYSYMBOL_NAME = 17,                      /* NAME  */
  YYSYMBOL_HOST = 18,                      /* HOST  */
  YYSYMBOL_IP = 19,                        /* IP  */
  YYSYMBOL_USERNAME = 20,                  /* USERNAME  */
  YYSYMBOL_PASS = 21,                      /* PASS  */
  YYSYMBOL_LOCAL = 22,                     /* LOCAL  */
  YYSYMBOL_SECONDS = 23,                   /* SECONDS  */
  YYSYMBOL_MINUTES = 24,                   /* MINUTES  */
  YYSYMBOL_HOURS = 25,                     /* HOURS  */
  YYSYMBOL_DAYS = 26,                      /* DAYS  */
  YYSYMBOL_WEEKS = 27,                     /* WEEKS  */
  YYSYMBOL_MONTHS = 28,                    /* MONTHS  */
  YYSYMBOL_YEARS = 29,                     /* YEARS  */
  YYSYMBOL_DECADES = 30,                   /* DECADES  */
  YYSYMBOL_BYTES = 31,                     /* BYTES  */
  YYSYMBOL_KBYTES = 32,                    /* KBYTES  */
  YYSYMBOL_MBYTES = 33,                    /* MBYTES  */
  YYSYMBOL_GBYTES = 34,                    /* GBYTES  */
  YYSYMBOL_TBYTES = 35,                    /* TBYTES  */
  YYSYMBOL_SERVER = 36,                    /* SERVER  */
  YYSYMBOL_PORT = 37,                      /* PORT  */
  YYSYMBOL_MASK = 38,                      /* MASK  */
  YYSYMBOL_HUB = 39,                       /* HUB  */
  YYSYMBOL_LEAF = 40,                      /* LEAF  */
  YYSYMBOL_UWORLD = 41,                    /* UWORLD  */
  YYSYMBOL_YES = 42,                       /* YES  */
  YYSYMBOL_NO = 43,                        /* NO  */
  YYSYMBOL_OPER = 44,                      /* OPER  */
  YYSYMBOL_VHOST = 45,                     /* VHOST  */
  YYSYMBOL_HIDDEN = 46,                    /* HIDDEN  */
  YYSYMBOL_MOTD = 47,                      /* MOTD  */
  YYSYMBOL_JUPE = 48,                      /* JUPE  */
  YYSYMBOL_NICK = 49,                      /* NICK  */
  YYSYMBOL_NUMERIC = 50,                   /* NUMERIC  */
  YYSYMBOL_DESCRIPTION = 51,               /* DESCRIPTION  */
  YYSYMBOL_CLIENT = 52,                    /* CLIENT  */
  YYSYMBOL_KILL = 53,                      /* KILL  */
  YYSYMBOL_CRULE = 54,                     /* CRULE  */
  YYSYMBOL_REAL = 55,                      /* REAL  */
  YYSYMBOL_REASON = 56,                    /* REASON  */
  YYSYMBOL_TFILE = 57,                     /* TFILE  */
  YYSYMBOL_RULE = 58,                      /* RULE  */
  YYSYMBOL_ALL = 59,                       /* ALL  */
  YYSYMBOL_FEATURES = 60,                  /* FEATURES  */
  YYSYMBOL_QUARANTINE = 61,                /* QUARANTINE  */
  YYSYMBOL_PSEUDO = 62,                    /* PSEUDO  */
  YYSYMBOL_PREPEND = 63,                   /* PREPEND  */
  YYSYMBOL_USERMODE = 64,                  /* USERMODE  */
  YYSYMBOL_IAUTH = 65,                     /* IAUTH  */
  YYSYMBOL_TIMEOUT = 66,                   /* TIMEOUT  */
  YYSYMBOL_FAST = 67,                      /* FAST  */
  YYSYMBOL_AUTOCONNECT = 68,               /* AUTOCONNECT  */
  YYSYMBOL_PROGRAM = 69,                   /* PROGRAM  */
  YYSYMBOL_SPOOFHOST = 70,                 /* SPOOFHOST  */
  YYSYMBOL_TOK_IPV4 = 71,                  /* TOK_IPV4  */
  YYSYMBOL_TOK_IPV6 = 72,                  /* TOK_IPV6  */
  YYSYMBOL_DNS = 73,                       /* DNS  */
  YYSYMBOL_TLS = 74,                       /* TLS  */
  YYSYMBOL_CERTFILE = 75,                  /* CERTFILE  */
  YYSYMBOL_CIPHERS = 76,                   /* CIPHERS  */
  YYSYMBOL_FINGERPRINT = 77,               /* FINGERPRINT  */
  YYSYMBOL_KEYFILE = 78,                   /* KEYFILE  */
  YYSYMBOL_TPRIV_CHAN_LIMIT = 79,          /* TPRIV_CHAN_LIMIT  */
  YYSYMBOL_TPRIV_MODE_LCHAN = 80,          /* TPRIV_MODE_LCHAN  */
  YYSYMBOL_TPRIV_DEOP_LCHAN = 81,          /* TPRIV_DEOP_LCHAN  */
  YYSYMBOL_TPRIV_WALK_LCHAN = 82,          /* TPRIV_WALK_LCHAN  */
  YYSYMBOL_TPRIV_LOCAL_KILL = 83,          /* TPRIV_LOCAL_KILL  */
  YYSYMBOL_TPRIV_REHASH = 84,              /* TPRIV_REHASH  */
  YYSYMBOL_TPRIV_RESTART = 85,             /* TPRIV_RESTART  */
  YYSYMBOL_TPRIV_DIE = 86,                 /* TPRIV_DIE  */
  YYSYMBOL_TPRIV_GLINE = 87,               /* TPRIV_GLINE  */
  YYSYMBOL_TPRIV_LOCAL_GLINE = 88,         /* TPRIV_LOCAL_GLINE  */
  YYSYMBOL_TPRIV_LOCAL_JUPE = 89,          /* TPRIV_LOCAL_JUPE  */
  YYSYMBOL_TPRIV_LOCAL_BADCHAN = 90,       /* TPRIV_LOCAL_BADCHAN  */
  YYSYMBOL_TPRIV_LOCAL_OPMODE = 91,        /* TPRIV_LOCAL_OPMODE  */
  YYSYMBOL_TPRIV_OPMODE = 92,              /* TPRIV_OPMODE  */
  YYSYMBOL_TPRIV_SET = 93,                 /* TPRIV_SET  */
  YYSYMBOL_TPRIV_WHOX = 94,                /* TPRIV_WHOX  */
  YYSYMBOL_TPRIV_BADCHAN = 95,             /* TPRIV_BADCHAN  */
  YYSYMBOL_TPRIV_SEE_CHAN = 96,            /* TPRIV_SEE_CHAN  */
  YYSYMBOL_TPRIV_SHOW_INVIS = 97,          /* TPRIV_SHOW_INVIS  */
  YYSYMBOL_TPRIV_SHOW_ALL_INVIS = 98,      /* TPRIV_SHOW_ALL_INVIS  */
  YYSYMBOL_TPRIV_PROPAGATE = 99,           /* TPRIV_PROPAGATE  */
  YYSYMBOL_TPRIV_UNLIMIT_QUERY = 100,      /* TPRIV_UNLIMIT_QUERY  */
  YYSYMBOL_TPRIV_DISPLAY = 101,            /* TPRIV_DISPLAY  */
  YYSYMBOL_TPRIV_SEE_OPERS = 102,          /* TPRIV_SEE_OPERS  */
  YYSYMBOL_TPRIV_WIDE_GLINE = 103,         /* TPRIV_WIDE_GLINE  */
  YYSYMBOL_TPRIV_FORCE_OPMODE = 104,       /* TPRIV_FORCE_OPMODE  */
  YYSYMBOL_TPRIV_FORCE_LOCAL_OPMODE = 105, /* TPRIV_FORCE_LOCAL_OPMODE  */
  YYSYMBOL_TPRIV_APASS_OPMODE = 106,       /* TPRIV_APASS_OPMODE  */
  YYSYMBOL_TPRIV_CHANSERV = 107,           /* TPRIV_CHANSERV  */
  YYSYMBOL_TPRIV_XTRA_OPER = 108,          /* TPRIV_XTRA_OPER  */
  YYSYMBOL_TPRIV_NOIDLE = 109,             /* TPRIV_NOIDLE  */
  YYSYMBOL_TPRIV_FREEFORM = 110,           /* TPRIV_FREEFORM  */
  YYSYMBOL_TPRIV_PARANOID = 111,           /* TPRIV_PARANOID  */
  YYSYMBOL_TPRIV_CHECK = 112,              /* TPRIV_CHECK  */
  YYSYMBOL_TPRIV_LIST_CHAN = 113,          /* TPRIV_LIST_CHAN  */
  YYSYMBOL_114_ = 114,                     /* '+'  */
  YYSYMBOL_115_ = 115,                     /* '-'  */
  YYSYMBOL_116_ = 116,                     /* '*'  */
  YYSYMBOL_117_ = 117,                     /* '/'  */
  YYSYMBOL_118_ = 118,                     /* ';'  */
  YYSYMBOL_119_ = 119,                     /* '('  */
  YYSYMBOL_120_ = 120,                     /* ')'  */
  YYSYMBOL_121_ = 121,                     /* '{'  */
  YYSYMBOL_122_ = 122,                     /* '}'  */
  YYSYMBOL_123_ = 123,                     /* '='  */
  YYSYMBOL_YYACCEPT = 124,                 /* $accept  */
  YYSYMBOL_blocks = 125,                   /* blocks  */
  YYSYMBOL_block = 126,                    /* block  */
  YYSYMBOL_timespec = 127,                 /* timespec  */
  YYSYMBOL_factoredtimes = 128,            /* factoredtimes  */
  YYSYMBOL_factoredtime = 129,             /* factoredtime  */
  YYSYMBOL_timefactor = 130,               /* timefactor  */
  YYSYMBOL_sizespec = 131,                 /* sizespec  */
  YYSYMBOL_expr = 132,                     /* expr  */
  YYSYMBOL_jupeblock = 133,                /* jupeblock  */
  YYSYMBOL_jupeitems = 134,                /* jupeitems  */
  YYSYMBOL_jupeitem = 135,                 /* jupeitem  */
  YYSYMBOL_jupenick = 136,                 /* jupenick  */
  YYSYMBOL_generalblock = 137,             /* generalblock  */
  YYSYMBOL_138_1 = 138,                    /* $@1  */
  YYSYMBOL_generalitems = 139,             /* generalitems  */
  YYSYMBOL_generalitem = 140,              /* generalitem  */
  YYSYMBOL_generalnumeric = 141,           /* generalnumeric  */
  YYSYMBOL_generalname = 142,              /* generalname  */
  YYSYMBOL_generaldesc = 143,              /* generaldesc  */
  YYSYMBOL_generalvhost = 144,             /* generalvhost  */
  YYSYMBOL_generaldnsvhost = 145,          /* generaldnsvhost  */
  YYSYMBOL_generaldnsserver = 146,         /* generaldnsserver  */
  YYSYMBOL_generaltlscertfile = 147,       /* generaltlscertfile  */
  YYSYMBOL_generaltlskeyfile = 148,        /* generaltlskeyfile  */
  YYSYMBOL_adminblock = 149,               /* adminblock  */
  YYSYMBOL_150_2 = 150,                    /* $@2  */
  YYSYMBOL_adminitems = 151,               /* adminitems  */
  YYSYMBOL_adminitem = 152,                /* adminitem  */
  YYSYMBOL_adminlocation = 153,            /* adminlocation  */
  YYSYMBOL_admincontact = 154,             /* admincontact  */
  YYSYMBOL_classblock = 155,               /* classblock  */
  YYSYMBOL_156_3 = 156,                    /* $@3  */
  YYSYMBOL_classitems = 157,               /* classitems  */
  YYSYMBOL_classitem = 158,                /* classitem  */
  YYSYMBOL_classname = 159,                /* classname  */
  YYSYMBOL_classpingfreq = 160,            /* classpingfreq  */
  YYSYMBOL_classconnfreq = 161,            /* classconnfreq  */
  YYSYMBOL_classmaxlinks = 162,            /* classmaxlinks  */
  YYSYMBOL_classsendq = 163,               /* classsendq  */
  YYSYMBOL_classusermode = 164,            /* classusermode  */
  YYSYMBOL_connectblock = 165,             /* connectblock  */
  YYSYMBOL_166_4 = 166,                    /* $@4  */
  YYSYMBOL_connectitems = 167,             /* connectitems  */
  YYSYMBOL_connectitem = 168,              /* connectitem  */
  YYSYMBOL_connectname = 169,              /* connectname  */
  YYSYMBOL_connectpass = 170,              /* connectpass  */
  YYSYMBOL_connectclass = 171,             /* connectclass  */
  YYSYMBOL_connecthost = 172,              /* connecthost  */
  YYSYMBOL_connectport = 173,              /* connectport  */
  YYSYMBOL_connectvhost = 174,             /* connectvhost  */
  YYSYMBOL_connectleaf = 175,              /* connectleaf  */
  YYSYMBOL_connecthub = 176,               /* connecthub  */
  YYSYMBOL_connecthublimit = 177,          /* connecthublimit  */
  YYSYMBOL_connectmaxhops = 178,           /* connectmaxhops  */
  YYSYMBOL_connectauto = 179,              /* connectauto  */
  YYSYMBOL_connecttls = 180,               /* connecttls  */
  YYSYMBOL_uworldblock = 181,              /* uworldblock  */
  YYSYMBOL_uworlditems = 182,              /* uworlditems  */
  YYSYMBOL_uworlditem = 183,               /* uworlditem  */
  YYSYMBOL_uworldname = 184,               /* uworldname  */
  YYSYMBOL_operblock = 185,                /* operblock  */
  YYSYMBOL_operitems = 186,                /* operitems  */
  YYSYMBOL_operitem = 187,                 /* operitem  */
  YYSYMBOL_opername = 188,                 /* opername  */
  YYSYMBOL_operpass = 189,                 /* operpass  */
  YYSYMBOL_operhost = 190,                 /* operhost  */
  YYSYMBOL_operclass = 191,                /* operclass  */
  YYSYMBOL_priv = 192,                     /* priv  */
  YYSYMBOL_privtype = 193,                 /* privtype  */
  YYSYMBOL_yesorno = 194,                  /* yesorno  */
  YYSYMBOL_tlsfingerprint = 195,           /* tlsfingerprint  */
  YYSYMBOL_tlsciphers = 196,               /* tlsciphers  */
  YYSYMBOL_address_family = 197,           /* address_family  */
  YYSYMBOL_portblock = 198,                /* portblock  */
  YYSYMBOL_portitems = 199,                /* portitems  */
  YYSYMBOL_portitem = 200,                 /* portitem  */
  YYSYMBOL_portnumber = 201,               /* portnumber  */
  YYSYMBOL_portvhost = 202,                /* portvhost  */
  YYSYMBOL_portvhostnumber = 203,          /* portvhostnumber  */
  YYSYMBOL_portmask = 204,                 /* portmask  */
  YYSYMBOL_portserver = 205,               /* portserver  */
  YYSYMBOL_porthidden = 206,               /* porthidden  */
  YYSYMBOL_porttls = 207,                  /* porttls  */
  YYSYMBOL_clientblock = 208,              /* clientblock  */
  YYSYMBOL_209_5 = 209,                    /* $@5  */
  YYSYMBOL_clientitems = 210,              /* clientitems  */
  YYSYMBOL_clientitem = 211,               /* clientitem  */
  YYSYMBOL_clienthost = 212,               /* clienthost  */
  YYSYMBOL_clientip = 213,                 /* clientip  */
  YYSYMBOL_clientusername = 214,           /* clientusername  */
  YYSYMBOL_clientclass = 215,              /* clientclass  */
  YYSYMBOL_clientpass = 216,               /* clientpass  */
  YYSYMBOL_clientmaxlinks = 217,           /* clientmaxlinks  */
  YYSYMBOL_clientport = 218,               /* clientport  */
  YYSYMBOL_killblock = 219,                /* killblock  */
  YYSYMBOL_220_6 = 220,                    /* $@6  */
  YYSYMBOL_killitems = 221,                /* killitems  */
  YYSYMBOL_killitem = 222,                 /* killitem  */
  YYSYMBOL_killuhost = 223,                /* killuhost  */
  YYSYMBOL_killusername = 224,             /* killusername  */
  YYSYMBOL_killreal = 225,                 /* killreal  */
  YYSYMBOL_killreason = 226,               /* killreason  */
  YYSYMBOL_killreasonfile = 227,           /* killreasonfile  */
  YYSYMBOL_cruleblock = 228,               /* cruleblock  */
  YYSYMBOL_229_7 = 229,                    /* $@7  */
  YYSYMBOL_cruleitems = 230,               /* cruleitems  */
  YYSYMBOL_cruleitem = 231,                /* cruleitem  */
  YYSYMBOL_cruleserver = 232,              /* cruleserver  */
  YYSYMBOL_crulerule = 233,                /* crulerule  */
  YYSYMBOL_cruleall = 234,                 /* cruleall  */
  YYSYMBOL_motdblock = 235,                /* motdblock  */
  YYSYMBOL_motditems = 236,                /* motditems  */
  YYSYMBOL_motditem = 237,                 /* motditem  */
  YYSYMBOL_motdhost = 238,                 /* motdhost  */
  YYSYMBOL_motdfile = 239,                 /* motdfile  */
  YYSYMBOL_featuresblock = 240,            /* featuresblock  */
  YYSYMBOL_featureitems = 241,             /* featureitems  */
  YYSYMBOL_featureitem = 242,              /* featureitem  */
  YYSYMBOL_243_8 = 243,                    /* $@8  */
  YYSYMBOL_stringlist = 244,               /* stringlist  */
  YYSYMBOL_extrastring = 245,              /* extrastring  */
  YYSYMBOL_quarantineblock = 246,          /* quarantineblock  */
  YYSYMBOL_quarantineitems = 247,          /* quarantineitems  */
  YYSYMBOL_quarantineitem = 248,           /* quarantineitem  */
  YYSYMBOL_pseudoblock = 249,              /* pseudoblock  */
  YYSYMBOL_250_9 = 250,                    /* $@9  */
  YYSYMBOL_pseudoitems = 251,              /* pseudoitems  */
  YYSYMBOL_pseudoitem = 252,               /* pseudoitem  */
  YYSYMBOL_pseudoname = 253,               /* pseudoname  */
  YYSYMBOL_pseudoprepend = 254,            /* pseudoprepend  */
  YYSYMBOL_pseudonick = 255,               /* pseudonick  */
  YYSYMBOL_pseudoflags = 256,              /* pseudoflags  */
  YYSYMBOL_iauthblock = 257,               /* iauthblock  */
  YYSYMBOL_iauthitems = 258,               /* iauthitems  */
  YYSYMBOL_iauthitem = 259,                /* iauthitem  */
  YYSYMBOL_iauthprogram = 260,             /* iauthprogram  */
  YYSYMBOL_261_10 = 261,                   /* $@10  */
  YYSYMBOL_spoofblock = 262,               /* spoofblock  */
  YYSYMBOL_263_11 = 263,                   /* $@11  */
  YYSYMBOL_spoofitems = 264,               /* spoofitems  */
  YYSYMBOL_spoofitem = 265,                /* spoofitem  */
  YYSYMBOL_spoofpassword = 266,            /* spoofpassword  */
  YYSYMBOL_spoofrealhost = 267,            /* spoofrealhost  */
  YYSYMBOL_spoofrealident = 268            /* spoofrealident  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
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
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  56
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   638

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  124
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  145
/* YYNRULES -- Number of rules.  */
#define YYNRULES  305
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  600

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   368


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     119,   120,   116,   114,     2,   115,     2,   117,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   118,
       2,   123,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   121,     2,   122,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   213,   213,   213,   214,   214,   214,   214,   215,   215,
     215,   215,   215,   216,   216,   216,   216,   216,   217,   217,
     217,   217,   221,   221,   223,   226,   228,   233,   234,   235,
     236,   237,   238,   239,   240,   243,   246,   249,   252,   255,
     258,   264,   268,   271,   274,   277,   284,   289,   290,   290,
     291,   292,   299,   298,   328,   328,   329,   329,   329,   329,
     330,   330,   331,   331,   333,   342,   354,   361,   378,   400,
     408,   413,   419,   418,   434,   434,   435,   435,   436,   445,
     451,   451,   476,   476,   477,   477,   477,   477,   478,   478,
     478,   479,   484,   488,   492,   496,   500,   507,   506,   554,
     554,   555,   555,   555,   555,   556,   556,   556,   556,   557,
     557,   557,   558,   558,   558,   559,   564,   569,   576,   581,
     585,   590,   594,   599,   604,   608,   609,   611,   615,   620,
     621,   621,   622,   623,   628,   672,   672,   673,   673,   673,
     673,   673,   674,   675,   680,   685,   701,   709,   719,   720,
     721,   722,   723,   724,   725,   726,   727,   728,   729,   730,
     731,   732,   733,   734,   735,   736,   737,   738,   739,   740,
     741,   742,   743,   744,   745,   746,   747,   748,   749,   750,
     751,   752,   753,   754,   755,   756,   757,   757,   759,   764,
     774,   775,   776,   777,   778,   782,   820,   820,   821,   821,
     821,   821,   821,   821,   822,   822,   823,   834,   844,   858,
     864,   867,   872,   875,   880,   883,   889,   888,   933,   933,
     934,   934,   934,   934,   934,   934,   934,   935,   948,   962,
     967,   974,   979,   983,   989,   988,  1008,  1008,  1009,  1009,
    1009,  1009,  1009,  1010,  1029,  1035,  1041,  1048,  1056,  1055,
    1088,  1088,  1089,  1089,  1089,  1091,  1100,  1106,  1109,  1114,
    1125,  1125,  1126,  1126,  1127,  1136,  1142,  1143,  1143,  1146,
    1145,  1156,  1156,  1157,  1165,  1166,  1166,  1167,  1177,  1176,
    1205,  1205,  1206,  1206,  1206,  1206,  1207,  1212,  1217,  1231,
    1236,  1246,  1246,  1247,  1249,  1248,  1258,  1257,  1306,  1306,
    1307,  1307,  1307,  1308,  1313,  1318
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "QSTRING", "NUMBER",
  "GENERAL", "ADMIN", "LOCATION", "CONTACT", "CONNECT", "CLASS", "CHANNEL",
  "PINGFREQ", "CONNECTFREQ", "MAXLINKS", "MAXHOPS", "SENDQ", "NAME",
  "HOST", "IP", "USERNAME", "PASS", "LOCAL", "SECONDS", "MINUTES", "HOURS",
  "DAYS", "WEEKS", "MONTHS", "YEARS", "DECADES", "BYTES", "KBYTES",
  "MBYTES", "GBYTES", "TBYTES", "SERVER", "PORT", "MASK", "HUB", "LEAF",
  "UWORLD", "YES", "NO", "OPER", "VHOST", "HIDDEN", "MOTD", "JUPE", "NICK",
  "NUMERIC", "DESCRIPTION", "CLIENT", "KILL", "CRULE", "REAL", "REASON",
  "TFILE", "RULE", "ALL", "FEATURES", "QUARANTINE", "PSEUDO", "PREPEND",
  "USERMODE", "IAUTH", "TIMEOUT", "FAST", "AUTOCONNECT", "PROGRAM",
  "SPOOFHOST", "TOK_IPV4", "TOK_IPV6", "DNS", "TLS", "CERTFILE", "CIPHERS",
  "FINGERPRINT", "KEYFILE", "TPRIV_CHAN_LIMIT", "TPRIV_MODE_LCHAN",
  "TPRIV_DEOP_LCHAN", "TPRIV_WALK_LCHAN", "TPRIV_LOCAL_KILL",
  "TPRIV_REHASH", "TPRIV_RESTART", "TPRIV_DIE", "TPRIV_GLINE",
  "TPRIV_LOCAL_GLINE", "TPRIV_LOCAL_JUPE", "TPRIV_LOCAL_BADCHAN",
  "TPRIV_LOCAL_OPMODE", "TPRIV_OPMODE", "TPRIV_SET", "TPRIV_WHOX",
  "TPRIV_BADCHAN", "TPRIV_SEE_CHAN", "TPRIV_SHOW_INVIS",
  "TPRIV_SHOW_ALL_INVIS", "TPRIV_PROPAGATE", "TPRIV_UNLIMIT_QUERY",
  "TPRIV_DISPLAY", "TPRIV_SEE_OPERS", "TPRIV_WIDE_GLINE",
  "TPRIV_FORCE_OPMODE", "TPRIV_FORCE_LOCAL_OPMODE", "TPRIV_APASS_OPMODE",
  "TPRIV_CHANSERV", "TPRIV_XTRA_OPER", "TPRIV_NOIDLE", "TPRIV_FREEFORM",
  "TPRIV_PARANOID", "TPRIV_CHECK", "TPRIV_LIST_CHAN", "'+'", "'-'", "'*'",
  "'/'", "';'", "'('", "')'", "'{'", "'}'", "'='", "$accept", "blocks",
  "block", "timespec", "factoredtimes", "factoredtime", "timefactor",
  "sizespec", "expr", "jupeblock", "jupeitems", "jupeitem", "jupenick",
  "generalblock", "$@1", "generalitems", "generalitem", "generalnumeric",
  "generalname", "generaldesc", "generalvhost", "generaldnsvhost",
  "generaldnsserver", "generaltlscertfile", "generaltlskeyfile",
  "adminblock", "$@2", "adminitems", "adminitem", "adminlocation",
  "admincontact", "classblock", "$@3", "classitems", "classitem",
  "classname", "classpingfreq", "classconnfreq", "classmaxlinks",
  "classsendq", "classusermode", "connectblock", "$@4", "connectitems",
  "connectitem", "connectname", "connectpass", "connectclass",
  "connecthost", "connectport", "connectvhost", "connectleaf",
  "connecthub", "connecthublimit", "connectmaxhops", "connectauto",
  "connecttls", "uworldblock", "uworlditems", "uworlditem", "uworldname",
  "operblock", "operitems", "operitem", "opername", "operpass", "operhost",
  "operclass", "priv", "privtype", "yesorno", "tlsfingerprint",
  "tlsciphers", "address_family", "portblock", "portitems", "portitem",
  "portnumber", "portvhost", "portvhostnumber", "portmask", "portserver",
  "porthidden", "porttls", "clientblock", "$@5", "clientitems",
  "clientitem", "clienthost", "clientip", "clientusername", "clientclass",
  "clientpass", "clientmaxlinks", "clientport", "killblock", "$@6",
  "killitems", "killitem", "killuhost", "killusername", "killreal",
  "killreason", "killreasonfile", "cruleblock", "$@7", "cruleitems",
  "cruleitem", "cruleserver", "crulerule", "cruleall", "motdblock",
  "motditems", "motditem", "motdhost", "motdfile", "featuresblock",
  "featureitems", "featureitem", "$@8", "stringlist", "extrastring",
  "quarantineblock", "quarantineitems", "quarantineitem", "pseudoblock",
  "$@9", "pseudoitems", "pseudoitem", "pseudoname", "pseudoprepend",
  "pseudonick", "pseudoflags", "iauthblock", "iauthitems", "iauthitem",
  "iauthprogram", "$@10", "spoofblock", "$@11", "spoofitems", "spoofitem",
  "spoofpassword", "spoofrealhost", "spoofrealident", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-447)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     129,  -104,  -447,  -447,  -447,  -447,   -93,   -76,   -67,   -54,
     -52,  -447,  -447,  -447,   -41,   -15,    74,   -13,   111,    37,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,    -2,
       8,    27,    34,   122,   140,   238,    -9,   120,    53,    54,
      63,   183,   185,    79,   126,    80,  -447,  -447,    14,    75,
     444,   340,    84,    85,    86,    87,   123,   -66,  -447,   125,
     122,  -447,  -447,  -447,  -447,  -447,  -447,  -447,   127,   130,
     140,  -447,   128,   135,   147,   148,  -447,  -447,  -447,   172,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,   132,  -447,  -447,  -447,  -447,
    -447,  -447,   149,  -447,   152,   153,   156,    -9,  -447,  -447,
     154,   157,   120,  -447,   243,    48,    -6,  -447,     2,  -447,
     158,     3,  -447,  -447,   159,   161,   126,  -447,  -447,   162,
     164,   165,   166,   -18,   -20,   168,    14,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,   169,   170,     0,  -447,  -447,
    -447,   171,   173,   174,   175,   176,   177,   -74,   184,   178,
     180,   -37,   182,   444,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,   186,   187,
     188,   190,   191,   192,   194,   340,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,    58,    49,   270,    49,    90,   232,    98,
     189,  -447,   271,   240,  -447,   281,   292,   302,   303,   236,
     242,  -447,   109,   305,   358,   245,  -447,   361,   247,  -447,
     244,   246,   248,   249,   250,   251,   252,   254,   243,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,   255,   256,   257,   258,
     259,   261,    48,  -447,  -447,  -447,  -447,  -447,   262,   263,
     264,   267,    -6,  -447,  -447,  -447,   268,   266,  -447,   363,
     272,  -447,    12,  -447,   274,  -447,    42,   365,   367,   373,
     391,   273,   275,   276,   277,   279,  -447,   392,   398,   284,
    -447,   400,     7,   402,   403,   404,   405,  -447,   407,  -447,
     408,   136,   150,   290,  -447,     7,     7,     7,     7,   409,
     410,   296,  -447,   297,   298,   345,   347,   451,   338,   454,
     342,   346,   455,   348,   349,  -447,   350,  -447,   351,   352,
     353,   354,   460,  -447,  -447,  -447,   355,   356,   357,  -447,
     359,  -447,   473,     7,   475,   476,   477,   479,     7,   368,
    -447,   482,   484,   485,   487,   488,   374,  -447,   490,   491,
     155,   377,  -447,   493,  -447,   379,  -447,   375,   376,   378,
     382,   380,    12,  -447,  -447,  -447,  -447,   493,  -447,   381,
     383,   384,   386,    42,  -447,  -447,  -447,   385,   387,   393,
     395,   506,    49,   507,   511,  -447,   397,   399,  -447,   401,
    -447,     7,   -22,   406,   411,   412,   413,   414,   415,   416,
     417,   418,   419,  -447,   420,     7,  -447,    -4,   421,    29,
     422,     1,   423,   424,  -447,  -447,  -447,  -447,  -447,   425,
    -447,    13,  -447,  -447,   426,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,   427,  -447,  -447,  -447,  -447,   428,    47,   429,
     430,   431,   432,   151,  -447,   433,   434,   435,   436,   437,
    -447,   438,   439,   440,   441,  -447,  -447,     9,  -447,  -447,
     513,   517,   518,  -447,   442,  -447,    10,   519,   520,   522,
     443,  -447,  -447,  -447,  -447,  -447,   445,   523,   446,   447,
    -447,  -447,  -447,   -44,     7,     7,     7,     7,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,    -4,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,   448,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,   449,   450,   452,  -447,  -447,   453,
     456,   457,  -447,  -447,   458,  -447,  -447,  -447,    55,    55,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     0,    52,    72,    97,    80,     0,     0,     0,     0,
       0,   216,   234,   248,     0,     0,     0,     0,     0,     0,
       3,    11,     5,     4,     6,     7,     8,     9,    10,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     2,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   205,     0,
     197,   198,   199,   200,   201,   202,   203,   204,     0,     0,
     131,   132,     0,     0,     0,     0,   176,   159,   152,     0,
     148,   149,   150,   151,   153,   154,   155,   156,   157,   158,
     160,   166,   161,   162,   163,   164,   165,   167,   168,   169,
     170,   171,   172,   173,   174,   177,   178,   179,   180,   181,
     182,   183,   185,   184,   175,     0,   135,   137,   138,   139,
     140,   141,     0,   142,     0,     0,     0,   261,   262,   263,
       0,     0,    49,    50,     0,     0,     0,   269,     0,   268,
       0,     0,   276,   278,     0,     0,   292,   293,   296,     0,
       0,     0,     0,     0,     0,     0,    55,    56,    57,    59,
      58,    60,    61,    62,    63,     0,     0,     0,    75,    76,
      77,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,     0,     0,
       0,     0,     0,     0,     0,    83,    84,    85,    86,    87,
      88,    89,    90,     0,   190,     0,   190,     0,     0,     0,
       0,   196,     0,     0,   130,     0,     0,     0,     0,     0,
       0,   136,     0,     0,     0,     0,   260,     0,     0,    48,
       0,     0,     0,     0,     0,     0,     0,     0,   219,   220,
     221,   222,   223,   224,   225,   226,     0,     0,     0,     0,
       0,     0,   237,   238,   240,   239,   242,   241,     0,     0,
       0,     0,   251,   252,   253,   254,     0,     0,   267,     0,
       0,   275,     0,   294,     0,   291,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    54,     0,     0,     0,
      74,     0,     0,     0,     0,     0,     0,   122,     0,   121,
       0,     0,     0,     0,    99,     0,     0,     0,     0,     0,
       0,     0,    82,     0,     0,   191,   192,     0,     0,     0,
       0,     0,     0,     0,     0,   195,     0,   129,     0,     0,
       0,     0,     0,   134,   186,   187,     0,     0,     0,   259,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
     218,     0,     0,     0,     0,     0,     0,   236,     0,     0,
       0,     0,   250,     0,   266,     0,   274,     0,     0,     0,
       0,     0,   281,   282,   283,   284,   285,     0,   290,     0,
       0,     0,     0,   299,   300,   301,   302,     0,     0,     0,
       0,     0,   190,     0,     0,    53,     0,     0,    73,     0,
      41,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    98,     0,    23,    25,    22,     0,     0,
       0,    35,     0,     0,    81,   210,   211,   193,   194,     0,
     209,     0,   212,   213,     0,   214,   215,   133,   146,   143,
     145,   144,     0,   147,   264,   265,    51,     0,     0,     0,
       0,     0,     0,     0,   217,     0,     0,     0,     0,     0,
     235,     0,     0,     0,     0,   249,   273,     0,   272,   277,
       0,     0,     0,   289,     0,   280,     0,     0,     0,     0,
       0,   298,    65,    67,    64,    66,     0,     0,     0,     0,
      78,    79,   117,     0,     0,     0,     0,     0,   124,   115,
     118,   116,   119,   123,   120,   125,   126,   127,   128,    92,
      24,     0,    27,    28,    29,    30,    31,    32,    33,    34,
      26,    93,    94,    95,    36,    37,    38,    39,    40,    91,
      96,   206,     0,   207,   189,   188,   230,   232,   227,   228,
     229,   231,   233,   243,   244,   245,   246,   247,   255,   256,
     257,   258,   270,   271,     0,     0,     0,   279,   295,     0,
       0,     0,   297,    69,     0,    70,    71,    46,    42,    43,
      44,    45,   208,   286,   288,   287,   304,   305,   303,    68
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -447,  -447,   508,   202,  -447,   134,  -447,  -447,  -312,  -447,
     459,  -447,  -447,  -447,  -447,   396,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,   461,  -447,
    -447,  -447,  -447,   362,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,   388,  -447,  -447,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,  -447,   492,  -447,
    -447,  -447,  -447,   462,  -447,  -447,  -447,  -447,   -59,  -447,
    -447,   -57,   -56,  -225,  -447,   503,  -447,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,   320,  -447,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,  -447,   307,  -447,  -447,
    -447,  -447,  -447,  -447,  -447,  -447,   300,  -447,  -447,  -447,
    -447,  -447,   463,  -447,  -447,  -447,  -447,  -447,   464,  -447,
     193,  -446,  -447,  -447,   465,  -447,  -447,   196,  -447,  -447,
    -447,  -447,  -447,  -447,   466,  -447,  -447,  -447,  -447,  -447,
     181,  -447,  -447,  -447,  -447
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    19,    20,   434,   435,   436,   540,   440,   437,    21,
     141,   142,   143,    22,    39,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,    23,    40,   177,   178,   179,
     180,    24,    42,   214,   215,   216,   217,   218,   219,   220,
     221,    25,    41,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,    26,    79,    80,
      81,    27,   125,   126,   127,   128,   129,   130,   131,   132,
     356,   133,    68,   337,    28,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    29,    48,   257,   258,   259,   260,
     261,   262,   263,   264,   265,    30,    49,   271,   272,   273,
     274,   275,   276,   277,    31,    50,   281,   282,   283,   284,
     285,    32,   136,   137,   138,   139,    33,   148,   149,   286,
     487,   488,    34,   151,   152,    35,   292,   391,   392,   393,
     394,   395,   396,    36,   155,   156,   157,   397,    37,   296,
     402,   403,   404,   405,   406
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     422,   339,   222,   206,   207,   147,   150,   175,   176,   134,
     228,   420,   486,   486,    38,   439,   441,   552,   301,   532,
     533,   534,   535,   536,   537,   538,   539,   302,    43,   387,
     278,   159,   544,   545,   546,   547,   548,    56,     1,   228,
     239,   573,     2,     3,   317,    44,     4,     5,   135,   318,
     573,   468,   279,   280,    45,   303,   473,   229,   304,   160,
     399,   388,   400,   401,   161,   162,   266,    46,   267,    47,
     514,   515,   516,   517,     6,   389,   587,    53,     7,   390,
      51,     8,   175,   176,     9,    10,   322,   163,   164,    11,
      12,    13,   514,   515,   516,   517,   518,    14,    15,    16,
     333,   334,    17,   268,   269,   270,    52,    18,    54,   513,
     514,   515,   516,   517,    55,   514,   515,   516,   517,    58,
     335,   336,   309,   531,   287,   290,   421,   572,   578,    59,
       1,   553,   340,   341,     2,     3,   206,   207,     4,     5,
     343,   344,    82,   514,   515,   516,   517,   542,    60,    83,
      84,   354,   355,    85,    86,    61,   222,    78,    62,    63,
      64,   514,   515,   516,   517,   557,     6,    65,    66,   140,
       7,   516,   517,     8,   144,   145,     9,    10,   429,   430,
      87,    11,    12,    13,   146,    88,   147,   507,   150,    14,
      15,    16,   431,   432,    17,   154,    67,   483,   484,    18,
     153,   158,   588,   589,   590,   591,    89,   223,   224,   225,
     226,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   227,   230,    82,   239,
     232,   235,   233,   250,   240,    83,    84,   251,   236,    85,
      86,   252,   253,   254,   255,   514,   515,   516,   517,   562,
     237,   238,   242,   338,   346,   243,   244,   247,   245,   248,
     256,   289,   293,   294,   348,   297,    87,   298,   299,   300,
     305,    88,   307,   308,   311,   349,   312,   313,   314,   315,
     316,   320,   319,   321,   323,   350,   351,   345,   357,   325,
     326,   327,    89,   328,   329,   330,   331,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   208,   209,   210,   342,   211,   212,   347,   352,
     353,   358,    86,   359,   360,   361,   385,   362,   407,   363,
     408,   364,   365,   366,   367,   368,   369,   409,   371,   372,
     373,   374,   375,   376,   384,   378,   379,   380,    87,   381,
     386,   383,   398,    88,   410,   416,   411,   415,   412,   413,
     414,   417,   418,   419,   213,   423,   424,   425,   433,   426,
     427,   428,   442,   443,   444,   445,   446,   447,   448,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   181,   449,   450,   451,   454,   182,
     452,   183,   184,   462,   453,   185,   455,   456,   457,   458,
     459,   460,   461,   463,   464,   465,   467,   466,   469,   470,
     471,   186,   472,   187,   188,   475,   474,   476,   477,   189,
     478,   479,   480,   481,   482,   485,   486,   489,   490,   491,
     493,   492,   494,   502,   497,   503,   498,   499,   500,   506,
     508,   504,   190,   505,   509,   510,   574,   511,   191,   512,
     575,   576,   579,   580,   519,   581,   584,    57,   438,   520,
     521,   522,   523,   524,   525,   526,   527,   528,   529,   541,
     543,   549,   550,   551,   554,   555,   556,   558,   559,   560,
     561,   563,   564,   565,   566,   567,   568,   569,   570,   571,
     577,   582,   306,   583,   585,   586,   592,   593,   594,   530,
     595,   596,   234,   231,   597,   598,   599,   332,   370,   377,
       0,   324,   382,     0,   501,     0,     0,   241,   495,     0,
     496,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     246,   249,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   288,     0,     0,     0,   291,     0,     0,     0,
       0,     0,   295,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   310
};

static const yytype_int16 yycheck[] =
{
     312,   226,    61,    60,    60,     3,     3,     7,     8,    18,
      76,     4,     3,     3,   118,   327,   328,     4,    36,    23,
      24,    25,    26,    27,    28,    29,    30,    45,   121,    17,
      36,    17,    31,    32,    33,    34,    35,     0,     1,    76,
      77,   487,     5,     6,   118,   121,     9,    10,    57,   123,
     496,   363,    58,    59,   121,    75,   368,   123,    78,    45,
      18,    49,    20,    21,    50,    51,    18,   121,    20,   121,
     114,   115,   116,   117,    37,    63,   120,     3,    41,    67,
     121,    44,     7,     8,    47,    48,   123,    73,    74,    52,
      53,    54,   114,   115,   116,   117,   118,    60,    61,    62,
      42,    43,    65,    55,    56,    57,   121,    70,   121,   421,
     114,   115,   116,   117,     3,   114,   115,   116,   117,   121,
      71,    72,   122,   435,   122,   122,   119,   118,   118,   121,
       1,   118,    42,    43,     5,     6,   193,   193,     9,    10,
      42,    43,    10,   114,   115,   116,   117,   118,   121,    17,
      18,    42,    43,    21,    22,   121,   215,    17,    36,    37,
      38,   114,   115,   116,   117,   118,    37,    45,    46,    49,
      41,   116,   117,    44,   121,   121,    47,    48,    42,    43,
      48,    52,    53,    54,   121,    53,     3,   412,     3,    60,
      61,    62,    42,    43,    65,    69,    74,    42,    43,    70,
     121,   121,   514,   515,   516,   517,    74,   123,   123,   123,
     123,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   123,   122,    10,    77,
     123,   123,   122,    10,   122,    17,    18,    14,   123,    21,
      22,    18,    19,    20,    21,   114,   115,   116,   117,   118,
     123,   123,   123,     3,     3,   123,   123,   123,   122,   122,
      37,   123,   123,   122,     3,   123,    48,   123,   123,   123,
     122,    53,   123,   123,   123,     3,   123,   123,   123,   123,
     123,   123,   118,   123,   122,     3,     3,   118,     3,   123,
     123,   123,    74,   123,   123,   123,   122,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    12,    13,    14,   123,    16,    17,   118,   123,
     118,     3,    22,   118,     3,   118,     3,   123,     3,   123,
       3,   123,   123,   123,   123,   123,   122,     4,   123,   123,
     123,   123,   123,   122,   118,   123,   123,   123,    48,   122,
     118,   123,   118,    53,     3,     3,   123,   118,   123,   123,
     123,     3,   118,     3,    64,     3,     3,     3,   118,     4,
       3,     3,     3,     3,   118,   118,   118,    72,    71,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,    10,     4,   118,     3,     3,    15,
     118,    17,    18,     3,   118,    21,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,     3,   118,     3,     3,
       3,    37,     3,    39,    40,     3,   118,     3,     3,    45,
       3,     3,   118,     3,     3,   118,     3,   118,   123,   123,
     118,   123,   122,   118,   123,   118,   123,   123,   122,     3,
       3,   118,    68,   118,     3,   118,     3,   118,    74,   118,
       3,     3,     3,     3,   118,     3,     3,    19,   326,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   166,   118,   118,   118,   118,   118,   118,   435,
     118,   118,    80,    70,   118,   118,   118,   215,   258,   272,
      -1,   193,   282,    -1,   403,    -1,    -1,   125,   392,    -1,
     397,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     137,   142,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   148,    -1,    -1,    -1,   151,    -1,    -1,    -1,
      -1,    -1,   156,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   177
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     1,     5,     6,     9,    10,    37,    41,    44,    47,
      48,    52,    53,    54,    60,    61,    62,    65,    70,   125,
     126,   133,   137,   149,   155,   165,   181,   185,   198,   208,
     219,   228,   235,   240,   246,   249,   257,   262,   118,   138,
     150,   166,   156,   121,   121,   121,   121,   121,   209,   220,
     229,   121,   121,     3,   121,     3,     0,   126,   121,   121,
     121,   121,    36,    37,    38,    45,    46,    74,   196,   199,
     200,   201,   202,   203,   204,   205,   206,   207,    17,   182,
     183,   184,    10,    17,    18,    21,    22,    48,    53,    74,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   186,   187,   188,   189,   190,
     191,   192,   193,   195,    18,    57,   236,   237,   238,   239,
      49,   134,   135,   136,   121,   121,   121,     3,   241,   242,
       3,   247,   248,   121,    69,   258,   259,   260,   121,    17,
      45,    50,    51,    73,    74,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,     7,     8,   151,   152,   153,
     154,    10,    15,    17,    18,    21,    37,    39,    40,    45,
      68,    74,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   195,   196,    12,    13,
      14,    16,    17,    64,   157,   158,   159,   160,   161,   162,
     163,   164,   192,   123,   123,   123,   123,   123,    76,   123,
     122,   199,   123,   122,   182,   123,   123,   123,   123,    77,
     122,   187,   123,   123,   123,   122,   236,   123,   122,   134,
      10,    14,    18,    19,    20,    21,    37,   210,   211,   212,
     213,   214,   215,   216,   217,   218,    18,    20,    55,    56,
      57,   221,   222,   223,   224,   225,   226,   227,    36,    58,
      59,   230,   231,   232,   233,   234,   243,   122,   242,   123,
     122,   248,   250,   123,   122,   258,   263,   123,   123,   123,
     123,    36,    45,    75,    78,   122,   139,   123,   123,   122,
     152,   123,   123,   123,   123,   123,   123,   118,   123,   118,
     123,   123,   123,   122,   167,   123,   123,   123,   123,   123,
     123,   122,   157,    42,    43,    71,    72,   197,     3,   197,
      42,    43,   123,    42,    43,   118,     3,   118,     3,     3,
       3,     3,   123,   118,    42,    43,   194,     3,     3,   118,
       3,   118,   123,   123,   123,   123,   123,   123,   123,   122,
     210,   123,   123,   123,   123,   123,   122,   221,   123,   123,
     123,   122,   230,   123,   118,     3,   118,    17,    49,    63,
      67,   251,   252,   253,   254,   255,   256,   261,   118,    18,
      20,    21,   264,   265,   266,   267,   268,     3,     3,     4,
       3,   123,   123,   123,   123,   118,     3,     3,   118,     3,
       4,   119,   132,     3,     3,     3,     4,     3,     3,    42,
      43,    42,    43,   118,   127,   128,   129,   132,   127,   132,
     131,   132,     3,     3,   118,   118,   118,    72,    71,     4,
     118,     3,   118,   118,     3,   118,   118,   118,   118,   118,
     118,   118,     3,   118,   118,   118,   118,     3,   132,     3,
       3,     3,     3,   132,   118,     3,     3,     3,     3,     3,
     118,     3,     3,    42,    43,   118,     3,   244,   245,   118,
     123,   123,   123,   118,   122,   251,   244,   123,   123,   123,
     122,   264,   118,   118,   118,   118,     3,   197,     3,     3,
     118,   118,   118,   132,   114,   115,   116,   117,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     129,   132,    23,    24,    25,    26,    27,    28,    29,    30,
     130,   118,   118,   118,    31,    32,    33,    34,    35,   118,
     118,   118,     4,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   245,     3,     3,     3,   118,   118,     3,
       3,     3,   118,   118,     3,   118,   118,   120,   132,   132,
     132,   132,   118,   118,   118,   118,   118,   118,   118,   118
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   124,   125,   125,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   127,   127,   128,   128,   129,   130,   130,   130,
     130,   130,   130,   130,   130,   131,   131,   131,   131,   131,
     131,   132,   132,   132,   132,   132,   132,   133,   134,   134,
     135,   136,   138,   137,   139,   139,   140,   140,   140,   140,
     140,   140,   140,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   150,   149,   151,   151,   152,   152,   153,   154,
     156,   155,   157,   157,   158,   158,   158,   158,   158,   158,
     158,   159,   160,   161,   162,   163,   164,   166,   165,   167,
     167,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   179,   180,   180,   181,
     182,   182,   183,   184,   185,   186,   186,   187,   187,   187,
     187,   187,   187,   188,   189,   190,   191,   192,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   194,   194,   195,   196,
     197,   197,   197,   197,   197,   198,   199,   199,   200,   200,
     200,   200,   200,   200,   200,   200,   201,   202,   203,   204,
     205,   205,   206,   206,   207,   207,   209,   208,   210,   210,
     211,   211,   211,   211,   211,   211,   211,   212,   213,   214,
     215,   216,   217,   218,   220,   219,   221,   221,   222,   222,
     222,   222,   222,   223,   224,   225,   226,   227,   229,   228,
     230,   230,   231,   231,   231,   232,   233,   234,   234,   235,
     236,   236,   237,   237,   238,   239,   240,   241,   241,   243,
     242,   244,   244,   245,   246,   247,   247,   248,   250,   249,
     251,   251,   252,   252,   252,   252,   253,   254,   255,   256,
     257,   258,   258,   259,   261,   260,   263,   262,   264,   264,
     265,   265,   265,   266,   267,   268
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     1,     3,     3,     3,     3,     3,     5,     2,     1,
       1,     4,     0,     6,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     4,     4,     4,     6,     5,
       5,     5,     0,     6,     2,     1,     1,     1,     4,     4,
       0,     6,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     4,     4,     4,     4,     4,     0,     6,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     4,     4,     4,     4,
       4,     2,     2,     4,     4,     4,     4,     4,     4,     5,
       2,     1,     1,     4,     5,     1,     2,     1,     1,     1,
       1,     1,     1,     4,     4,     4,     4,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     5,
       0,     1,     1,     2,     2,     5,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     5,     5,     6,     4,
       4,     4,     4,     4,     4,     4,     0,     6,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     4,     4,
       4,     4,     4,     4,     0,     6,     2,     1,     1,     1,
       1,     1,     1,     4,     4,     4,     4,     4,     0,     6,
       2,     1,     1,     1,     1,     4,     4,     4,     4,     5,
       2,     1,     1,     1,     4,     4,     5,     2,     1,     0,
       5,     2,     1,     1,     5,     2,     1,     4,     0,     7,
       2,     1,     1,     1,     1,     1,     4,     4,     4,     2,
       5,     2,     1,     1,     0,     5,     0,     7,     2,     1,
       1,     1,     1,     4,     4,     4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

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
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 24: /* factoredtimes: factoredtimes factoredtime  */
#line 224 "./ircd_parser.y"
{
  (yyval.num) = (yyvsp[-1].num) + (yyvsp[0].num);
}
#line 2163 "y.tab.c"
    break;

  case 26: /* factoredtime: expr timefactor  */
#line 229 "./ircd_parser.y"
{
  (yyval.num) = (yyvsp[-1].num) * (yyvsp[0].num);
}
#line 2171 "y.tab.c"
    break;

  case 27: /* timefactor: SECONDS  */
#line 233 "./ircd_parser.y"
                    { (yyval.num) = 1; }
#line 2177 "y.tab.c"
    break;

  case 28: /* timefactor: MINUTES  */
#line 234 "./ircd_parser.y"
          { (yyval.num) = 60; }
#line 2183 "y.tab.c"
    break;

  case 29: /* timefactor: HOURS  */
#line 235 "./ircd_parser.y"
        { (yyval.num) = 60 * 60; }
#line 2189 "y.tab.c"
    break;

  case 30: /* timefactor: DAYS  */
#line 236 "./ircd_parser.y"
       { (yyval.num) = 60 * 60 * 24; }
#line 2195 "y.tab.c"
    break;

  case 31: /* timefactor: WEEKS  */
#line 237 "./ircd_parser.y"
        { (yyval.num) = 60 * 60 * 24 * 7; }
#line 2201 "y.tab.c"
    break;

  case 32: /* timefactor: MONTHS  */
#line 238 "./ircd_parser.y"
         { (yyval.num) = 60 * 60 * 24 * 7 * 4; }
#line 2207 "y.tab.c"
    break;

  case 33: /* timefactor: YEARS  */
#line 239 "./ircd_parser.y"
        { (yyval.num) = 60 * 60 * 24 * 365; }
#line 2213 "y.tab.c"
    break;

  case 34: /* timefactor: DECADES  */
#line 240 "./ircd_parser.y"
          { (yyval.num) = 60 * 60 * 24 * 365 * 10; }
#line 2219 "y.tab.c"
    break;

  case 35: /* sizespec: expr  */
#line 243 "./ircd_parser.y"
                        {
			(yyval.num) = (yyvsp[0].num);
		}
#line 2227 "y.tab.c"
    break;

  case 36: /* sizespec: expr BYTES  */
#line 246 "./ircd_parser.y"
                              { 
			(yyval.num) = (yyvsp[-1].num);
		}
#line 2235 "y.tab.c"
    break;

  case 37: /* sizespec: expr KBYTES  */
#line 249 "./ircd_parser.y"
                              {
			(yyval.num) = (yyvsp[-1].num) * 1024;
		}
#line 2243 "y.tab.c"
    break;

  case 38: /* sizespec: expr MBYTES  */
#line 252 "./ircd_parser.y"
                              {
			(yyval.num) = (yyvsp[-1].num) * 1024 * 1024;
		}
#line 2251 "y.tab.c"
    break;

  case 39: /* sizespec: expr GBYTES  */
#line 255 "./ircd_parser.y"
                              {
			(yyval.num) = (yyvsp[-1].num) * 1024 * 1024 * 1024;
		}
#line 2259 "y.tab.c"
    break;

  case 40: /* sizespec: expr TBYTES  */
#line 258 "./ircd_parser.y"
                              {
			(yyval.num) = (yyvsp[-1].num) * 1024 * 1024 * 1024;
		}
#line 2267 "y.tab.c"
    break;

  case 41: /* expr: NUMBER  */
#line 265 "./ircd_parser.y"
                { 
			(yyval.num) = (yyvsp[0].num);
		}
#line 2275 "y.tab.c"
    break;

  case 42: /* expr: expr '+' expr  */
#line 268 "./ircd_parser.y"
                                { 
			(yyval.num) = (yyvsp[-2].num) + (yyvsp[0].num);
		}
#line 2283 "y.tab.c"
    break;

  case 43: /* expr: expr '-' expr  */
#line 271 "./ircd_parser.y"
                                { 
			(yyval.num) = (yyvsp[-2].num) - (yyvsp[0].num);
		}
#line 2291 "y.tab.c"
    break;

  case 44: /* expr: expr '*' expr  */
#line 274 "./ircd_parser.y"
                                { 
			(yyval.num) = (yyvsp[-2].num) * (yyvsp[0].num);
		}
#line 2299 "y.tab.c"
    break;

  case 45: /* expr: expr '/' expr  */
#line 277 "./ircd_parser.y"
                                { 
			(yyval.num) = (yyvsp[-2].num) / (yyvsp[0].num);
		}
#line 2307 "y.tab.c"
    break;

  case 46: /* expr: '(' expr ')'  */
#line 284 "./ircd_parser.y"
                               {
			(yyval.num) = (yyvsp[-1].num);
		}
#line 2315 "y.tab.c"
    break;

  case 51: /* jupenick: NICK '=' QSTRING ';'  */
#line 293 "./ircd_parser.y"
{
  addNickJupes((yyvsp[-1].text));
  MyFree((yyvsp[-1].text));
}
#line 2324 "y.tab.c"
    break;

  case 52: /* $@1: %empty  */
#line 299 "./ircd_parser.y"
{
    /* Zero out the vhost addresses, in case they were removed. */
    memset(&VirtualHost_v4.addr, 0, sizeof(VirtualHost_v4.addr));
    memset(&VirtualHost_v6.addr, 0, sizeof(VirtualHost_v6.addr));
}
#line 2334 "y.tab.c"
    break;

  case 53: /* generalblock: GENERAL $@1 '{' generalitems '}' ';'  */
#line 303 "./ircd_parser.y"
                           {
  if (localConf.name == NULL)
    parse_error("Your General block must contain a name.");
  if (localConf.numeric == 0)
    parse_error("Your General block must contain a numeric (between 1 and 4095).");
  MyFree(ircd_tls_keyfile);
  MyFree(ircd_tls_certfile);
  if (tls_certfile && !tls_keyfile)
  {
    parse_error("General block has tls certfile but no tls keyfile; disabling TLS");
    MyFree(tls_certfile);
  }
  else if (tls_keyfile && !tls_certfile)
  {
    parse_error("General block has tls keyfile but no tls certfile; disabling TLS");
    MyFree(tls_keyfile);
  }
  else if (tls_certfile && tls_keyfile)
  {
    ircd_tls_certfile = tls_certfile;
    ircd_tls_keyfile = tls_keyfile;
    tls_certfile = NULL;
    tls_keyfile = NULL;
  }	
}
#line 2364 "y.tab.c"
    break;

  case 64: /* generalnumeric: NUMERIC '=' NUMBER ';'  */
#line 334 "./ircd_parser.y"
{
  if (localConf.numeric == 0)
    localConf.numeric = (yyvsp[-1].num);
  else if (localConf.numeric != (yyvsp[-1].num))
    parse_error("Redefinition of server numeric %i (%i)", (yyvsp[-1].num),
    		localConf.numeric);
}
#line 2376 "y.tab.c"
    break;

  case 65: /* generalname: NAME '=' QSTRING ';'  */
#line 343 "./ircd_parser.y"
{
  if (localConf.name == NULL)
    localConf.name = (yyvsp[-1].text);
  else {
    if (strcmp(localConf.name, (yyvsp[-1].text)))
      parse_error("Redefinition of server name %s (%s)", (yyvsp[-1].text),
                  localConf.name);
    MyFree((yyvsp[-1].text));
  }
}
#line 2391 "y.tab.c"
    break;

  case 66: /* generaldesc: DESCRIPTION '=' QSTRING ';'  */
#line 355 "./ircd_parser.y"
{
  MyFree(localConf.description);
  localConf.description = (yyvsp[-1].text);
  ircd_strncpy(cli_info(&me), (yyvsp[-1].text), REALLEN);
}
#line 2401 "y.tab.c"
    break;

  case 67: /* generalvhost: VHOST '=' QSTRING ';'  */
#line 362 "./ircd_parser.y"
{
  struct irc_in_addr addr;
  char *vhost = (yyvsp[-1].text);

  if (!strcmp(vhost, "*")) {
    /* This traditionally meant bind to all interfaces and connect
     * from the default. */
  } else if (!ircd_aton(&addr, vhost))
    parse_error("Invalid virtual host '%s'.", vhost);
  else if (irc_in_addr_is_ipv4(&addr))
    memcpy(&VirtualHost_v4.addr, &addr, sizeof(addr));
  else
    memcpy(&VirtualHost_v6.addr, &addr, sizeof(addr));
  MyFree(vhost);
}
#line 2421 "y.tab.c"
    break;

  case 68: /* generaldnsvhost: DNS VHOST '=' address_family QSTRING ';'  */
#line 379 "./ircd_parser.y"
{
  struct irc_in_addr addr;
  int families = (yyvsp[-2].num);
  char *vhost = (yyvsp[-1].text);

  if (!strcmp(vhost, "*")) {
    /* Let the operating system assign the default. */
  } else if (!ircd_aton(&addr, vhost))
    parse_error("Invalid DNS virtual host '%s'.", vhost);
  else
  {
    if ((families & USE_IPV4)
        || (!families && irc_in_addr_is_ipv4(&addr)))
      memcpy(&VirtualHost_dns_v4.addr, &addr, sizeof(addr));
    if ((families & USE_IPV6)
        || (!families && !irc_in_addr_is_ipv4(&addr)))
      memcpy(&VirtualHost_dns_v6.addr, &addr, sizeof(addr));
  }
  MyFree(vhost);
}
#line 2446 "y.tab.c"
    break;

  case 69: /* generaldnsserver: DNS SERVER '=' QSTRING ';'  */
#line 401 "./ircd_parser.y"
{
  char *server = (yyvsp[-1].text);

  add_nameserver(server);
  MyFree(server);
}
#line 2457 "y.tab.c"
    break;

  case 70: /* generaltlscertfile: TLS CERTFILE '=' QSTRING ';'  */
#line 409 "./ircd_parser.y"
{
  tls_certfile = (yyvsp[-1].text);
}
#line 2465 "y.tab.c"
    break;

  case 71: /* generaltlskeyfile: TLS KEYFILE '=' QSTRING ';'  */
#line 414 "./ircd_parser.y"
{
  tls_keyfile = (yyvsp[-1].text);
}
#line 2473 "y.tab.c"
    break;

  case 72: /* $@2: %empty  */
#line 419 "./ircd_parser.y"
{
  MyFree(localConf.location1);
  MyFree(localConf.location2);
  MyFree(localConf.contact);
  localConf.location1 = localConf.location2 = localConf.contact = NULL;
}
#line 2484 "y.tab.c"
    break;

  case 73: /* adminblock: ADMIN $@2 '{' adminitems '}' ';'  */
#line 426 "./ircd_parser.y"
{
  if (localConf.location1 == NULL)
    DupString(localConf.location1, "");
  if (localConf.location2 == NULL)
    DupString(localConf.location2, "");
  if (localConf.contact == NULL)
    DupString(localConf.contact, "");
}
#line 2497 "y.tab.c"
    break;

  case 78: /* adminlocation: LOCATION '=' QSTRING ';'  */
#line 437 "./ircd_parser.y"
{
  if (localConf.location1 == NULL)
    localConf.location1 = (yyvsp[-1].text);
  else if (localConf.location2 == NULL)
    localConf.location2 = (yyvsp[-1].text);
  else /* Otherwise just drop it. -A1kmm */
    MyFree((yyvsp[-1].text));
}
#line 2510 "y.tab.c"
    break;

  case 79: /* admincontact: CONTACT '=' QSTRING ';'  */
#line 446 "./ircd_parser.y"
{
 MyFree(localConf.contact);
 localConf.contact = (yyvsp[-1].text);
}
#line 2519 "y.tab.c"
    break;

  case 80: /* $@3: %empty  */
#line 451 "./ircd_parser.y"
                  {
  tping = 90;
}
#line 2527 "y.tab.c"
    break;

  case 81: /* classblock: CLASS $@3 '{' classitems '}' ';'  */
#line 454 "./ircd_parser.y"
{
  if (name != NULL)
  {
    struct ConnectionClass *c_class;
    add_class(name, tping, tconn, maxlinks, sendq);
    c_class = find_class(name);
    MyFree(c_class->default_umode);
    c_class->default_umode = pass;
    memcpy(&c_class->privs, &privs, sizeof(c_class->privs));
    memcpy(&c_class->privs_dirty, &privs_dirty, sizeof(c_class->privs_dirty));
  }
  else {
   parse_error("Missing name in class block");
  }
  name = NULL;
  pass = NULL;
  tconn = 0;
  maxlinks = 0;
  sendq = 0;
  memset(&privs, 0, sizeof(privs));
  memset(&privs_dirty, 0, sizeof(privs_dirty));
}
#line 2554 "y.tab.c"
    break;

  case 91: /* classname: NAME '=' QSTRING ';'  */
#line 480 "./ircd_parser.y"
{
  MyFree(name);
  name = (yyvsp[-1].text);
}
#line 2563 "y.tab.c"
    break;

  case 92: /* classpingfreq: PINGFREQ '=' timespec ';'  */
#line 485 "./ircd_parser.y"
{
  tping = (yyvsp[-1].num);
}
#line 2571 "y.tab.c"
    break;

  case 93: /* classconnfreq: CONNECTFREQ '=' timespec ';'  */
#line 489 "./ircd_parser.y"
{
  tconn = (yyvsp[-1].num);
}
#line 2579 "y.tab.c"
    break;

  case 94: /* classmaxlinks: MAXLINKS '=' expr ';'  */
#line 493 "./ircd_parser.y"
{
  maxlinks = (yyvsp[-1].num);
}
#line 2587 "y.tab.c"
    break;

  case 95: /* classsendq: SENDQ '=' sizespec ';'  */
#line 497 "./ircd_parser.y"
{
  sendq = (yyvsp[-1].num);
}
#line 2595 "y.tab.c"
    break;

  case 96: /* classusermode: USERMODE '=' QSTRING ';'  */
#line 501 "./ircd_parser.y"
{
  MyFree(pass);
  pass = (yyvsp[-1].text);
}
#line 2604 "y.tab.c"
    break;

  case 97: /* $@4: %empty  */
#line 507 "./ircd_parser.y"
{
 maxlinks = 65535;
 flags = CONF_AUTOCONNECT;
}
#line 2613 "y.tab.c"
    break;

  case 98: /* connectblock: CONNECT $@4 '{' connectitems '}' ';'  */
#line 511 "./ircd_parser.y"
{
 struct ConfItem *aconf = NULL;
 if (name == NULL)
  parse_error("Missing name in connect block");
 else if (pass == NULL)
  parse_error("Missing password in connect block");
 else if (strlen(pass) > PASSWDLEN)
  parse_error("Password too long in connect block");
 else if (host == NULL)
  parse_error("Missing host in connect block");
 else if (strchr(host, '*') || strchr(host, '?'))
  parse_error("Invalid host '%s' in connect block", host);
 else if (c_class == NULL)
  parse_error("Missing or non-existent class in connect block");
 else {
   aconf = make_conf(CONF_SERVER);
   aconf->name = name;
   aconf->origin_name = origin;
   aconf->passwd = pass;
   aconf->conn_class = c_class;
   aconf->address.port = port;
   aconf->host = host;
   aconf->tls_ciphers = tls_ciphers;
   aconf->tls_fingerprint = tls_fingerprint;
   aconf->maximum = maxlinks;
   aconf->hub_limit = hub_limit;
   aconf->flags = flags;
   lookup_confhost(aconf);
 }
 if (!aconf) {
   MyFree(name);
   MyFree(pass);
   MyFree(host);
   MyFree(origin);
   MyFree(hub_limit);
   MyFree(tls_ciphers);
   MyFree(tls_fingerprint);
 }
 name = pass = host = origin = hub_limit = NULL;
 c_class = NULL;
 tls_ciphers = tls_fingerprint = NULL; 
 port = flags = 0;
}
#line 2661 "y.tab.c"
    break;

  case 115: /* connectname: NAME '=' QSTRING ';'  */
#line 560 "./ircd_parser.y"
{
 MyFree(name);
 name = (yyvsp[-1].text);
}
#line 2670 "y.tab.c"
    break;

  case 116: /* connectpass: PASS '=' QSTRING ';'  */
#line 565 "./ircd_parser.y"
{
 MyFree(pass);
 pass = (yyvsp[-1].text);
}
#line 2679 "y.tab.c"
    break;

  case 117: /* connectclass: CLASS '=' QSTRING ';'  */
#line 570 "./ircd_parser.y"
{
 c_class = find_class((yyvsp[-1].text));
 if (!c_class)
  parse_error("No such connection class '%s' for Connect block", (yyvsp[-1].text));
 MyFree((yyvsp[-1].text));
}
#line 2690 "y.tab.c"
    break;

  case 118: /* connecthost: HOST '=' QSTRING ';'  */
#line 577 "./ircd_parser.y"
{
 MyFree(host);
 host = (yyvsp[-1].text);
}
#line 2699 "y.tab.c"
    break;

  case 119: /* connectport: PORT '=' NUMBER ';'  */
#line 582 "./ircd_parser.y"
{
 port = (yyvsp[-1].num);
}
#line 2707 "y.tab.c"
    break;

  case 120: /* connectvhost: VHOST '=' QSTRING ';'  */
#line 586 "./ircd_parser.y"
{
 MyFree(origin);
 origin = (yyvsp[-1].text);
}
#line 2716 "y.tab.c"
    break;

  case 121: /* connectleaf: LEAF ';'  */
#line 591 "./ircd_parser.y"
{
 maxlinks = 0;
}
#line 2724 "y.tab.c"
    break;

  case 122: /* connecthub: HUB ';'  */
#line 595 "./ircd_parser.y"
{
 MyFree(hub_limit);
 DupString(hub_limit, "*");
}
#line 2733 "y.tab.c"
    break;

  case 123: /* connecthublimit: HUB '=' QSTRING ';'  */
#line 600 "./ircd_parser.y"
{
 MyFree(hub_limit);
 hub_limit = (yyvsp[-1].text);
}
#line 2742 "y.tab.c"
    break;

  case 124: /* connectmaxhops: MAXHOPS '=' expr ';'  */
#line 605 "./ircd_parser.y"
{
  maxlinks = (yyvsp[-1].num);
}
#line 2750 "y.tab.c"
    break;

  case 125: /* connectauto: AUTOCONNECT '=' YES ';'  */
#line 608 "./ircd_parser.y"
                                     { flags |= CONF_AUTOCONNECT; }
#line 2756 "y.tab.c"
    break;

  case 126: /* connectauto: AUTOCONNECT '=' NO ';'  */
#line 609 "./ircd_parser.y"
                          { flags &= ~CONF_AUTOCONNECT; }
#line 2762 "y.tab.c"
    break;

  case 127: /* connecttls: TLS '=' YES ';'  */
#line 612 "./ircd_parser.y"
{
  flags |= CONF_CONNECT_TLS;
}
#line 2770 "y.tab.c"
    break;

  case 128: /* connecttls: TLS '=' NO ';'  */
#line 616 "./ircd_parser.y"
{
  flags &= ~CONF_CONNECT_TLS;
}
#line 2778 "y.tab.c"
    break;

  case 133: /* uworldname: NAME '=' QSTRING ';'  */
#line 624 "./ircd_parser.y"
{
  make_conf(CONF_UWORLD)->host = (yyvsp[-1].text);
}
#line 2786 "y.tab.c"
    break;

  case 134: /* operblock: OPER '{' operitems '}' ';'  */
#line 629 "./ircd_parser.y"
{
  struct ConfItem *aconf = NULL;
  struct SLink *link;

  if (name == NULL)
    parse_error("Missing name in operator block");
  else if (strlen(name) > ACCOUNTLEN)
    parse_error("Operator name in operator block is too long");
  else if ((name[0] == NOOPERNAMECHARACTER) && (name[1] == '\0'))
    parse_error("Operator name can not be '%c'",NOOPERNAMECHARACTER);
  else if (pass == NULL)
    parse_error("Missing password in operator block");
  /* Do not check password length because it may be crypted. */
  else if (hosts == NULL)
    parse_error("Missing host(s) in operator block");
  else if (c_class == NULL)
    parse_error("Invalid or missing class in operator block");
  else if (!FlagHas(&privs_dirty, PRIV_PROPAGATE)
           && !FlagHas(&c_class->privs_dirty, PRIV_PROPAGATE))
    parse_error("Operator block for %s and class %s have no LOCAL setting", name, c_class->cc_name);
  else for (link = hosts; link != NULL; link = link->next) {
    aconf = make_conf(CONF_OPERATOR);
    DupString(aconf->name, name);
    DupString(aconf->passwd, pass);
    conf_parse_userhost(aconf, link->value.cp);
    aconf->conn_class = c_class;
    memcpy(&aconf->privs, &privs, sizeof(aconf->privs));
    memcpy(&aconf->privs_dirty, &privs_dirty, sizeof(aconf->privs_dirty));
    if (tls_fingerprint)
    {
      aconf->tls_fingerprint = tls_fingerprint;
      tls_fingerprint = NULL;
    }	
  }
  MyFree(name);
  MyFree(pass);
  MyFree(tls_fingerprint);
  free_slist(&hosts);
  name = pass = NULL;
  c_class = NULL;
  memset(&privs, 0, sizeof(privs));
  memset(&privs_dirty, 0, sizeof(privs_dirty));
}
#line 2834 "y.tab.c"
    break;

  case 143: /* opername: NAME '=' QSTRING ';'  */
#line 676 "./ircd_parser.y"
{
  MyFree(name);
  name = (yyvsp[-1].text);
}
#line 2843 "y.tab.c"
    break;

  case 144: /* operpass: PASS '=' QSTRING ';'  */
#line 681 "./ircd_parser.y"
{
  MyFree(pass);
  pass = (yyvsp[-1].text);
}
#line 2852 "y.tab.c"
    break;

  case 145: /* operhost: HOST '=' QSTRING ';'  */
#line 686 "./ircd_parser.y"
{
 struct SLink *link;
 link = make_link();
 if (!strchr((yyvsp[-1].text), '@'))
 {
   int uh_len;
   link->value.cp = (char*) MyMalloc((uh_len = strlen((yyvsp[-1].text))+3));
   ircd_snprintf(0, link->value.cp, uh_len, "*@%s", (yyvsp[-1].text));
 }
 else
   DupString(link->value.cp, (yyvsp[-1].text));
 MyFree((yyvsp[-1].text));
 link->next = hosts;
 hosts = link;
}
#line 2872 "y.tab.c"
    break;

  case 146: /* operclass: CLASS '=' QSTRING ';'  */
#line 702 "./ircd_parser.y"
{
 c_class = find_class((yyvsp[-1].text));
 if (!c_class)
  parse_error("No such connection class '%s' for Operator block", (yyvsp[-1].text));
 MyFree((yyvsp[-1].text));
}
#line 2883 "y.tab.c"
    break;

  case 147: /* priv: privtype '=' yesorno ';'  */
#line 710 "./ircd_parser.y"
{
  FlagSet(&privs_dirty, (yyvsp[-3].num));
  if (((yyvsp[-1].num) == 1) ^ invert)
    FlagSet(&privs, (yyvsp[-3].num));
  else
    FlagClr(&privs, (yyvsp[-3].num));
  invert = 0;
}
#line 2896 "y.tab.c"
    break;

  case 148: /* privtype: TPRIV_CHAN_LIMIT  */
#line 719 "./ircd_parser.y"
                           { (yyval.num) = PRIV_CHAN_LIMIT; }
#line 2902 "y.tab.c"
    break;

  case 149: /* privtype: TPRIV_MODE_LCHAN  */
#line 720 "./ircd_parser.y"
                           { (yyval.num) = PRIV_MODE_LCHAN; }
#line 2908 "y.tab.c"
    break;

  case 150: /* privtype: TPRIV_DEOP_LCHAN  */
#line 721 "./ircd_parser.y"
                           { (yyval.num) = PRIV_DEOP_LCHAN; }
#line 2914 "y.tab.c"
    break;

  case 151: /* privtype: TPRIV_WALK_LCHAN  */
#line 722 "./ircd_parser.y"
                           { (yyval.num) = PRIV_WALK_LCHAN; }
#line 2920 "y.tab.c"
    break;

  case 152: /* privtype: KILL  */
#line 723 "./ircd_parser.y"
               { (yyval.num) = PRIV_KILL; }
#line 2926 "y.tab.c"
    break;

  case 153: /* privtype: TPRIV_LOCAL_KILL  */
#line 724 "./ircd_parser.y"
                           { (yyval.num) = PRIV_LOCAL_KILL; }
#line 2932 "y.tab.c"
    break;

  case 154: /* privtype: TPRIV_REHASH  */
#line 725 "./ircd_parser.y"
                       { (yyval.num) = PRIV_REHASH; }
#line 2938 "y.tab.c"
    break;

  case 155: /* privtype: TPRIV_RESTART  */
#line 726 "./ircd_parser.y"
                        { (yyval.num) = PRIV_RESTART; }
#line 2944 "y.tab.c"
    break;

  case 156: /* privtype: TPRIV_DIE  */
#line 727 "./ircd_parser.y"
                    { (yyval.num) = PRIV_DIE; }
#line 2950 "y.tab.c"
    break;

  case 157: /* privtype: TPRIV_GLINE  */
#line 728 "./ircd_parser.y"
                      { (yyval.num) = PRIV_GLINE; }
#line 2956 "y.tab.c"
    break;

  case 158: /* privtype: TPRIV_LOCAL_GLINE  */
#line 729 "./ircd_parser.y"
                            { (yyval.num) = PRIV_LOCAL_GLINE; }
#line 2962 "y.tab.c"
    break;

  case 159: /* privtype: JUPE  */
#line 730 "./ircd_parser.y"
               { (yyval.num) = PRIV_JUPE; }
#line 2968 "y.tab.c"
    break;

  case 160: /* privtype: TPRIV_LOCAL_JUPE  */
#line 731 "./ircd_parser.y"
                           { (yyval.num) = PRIV_LOCAL_JUPE; }
#line 2974 "y.tab.c"
    break;

  case 161: /* privtype: TPRIV_LOCAL_OPMODE  */
#line 732 "./ircd_parser.y"
                             { (yyval.num) = PRIV_LOCAL_OPMODE; }
#line 2980 "y.tab.c"
    break;

  case 162: /* privtype: TPRIV_OPMODE  */
#line 733 "./ircd_parser.y"
                       { (yyval.num) = PRIV_OPMODE; }
#line 2986 "y.tab.c"
    break;

  case 163: /* privtype: TPRIV_SET  */
#line 734 "./ircd_parser.y"
                    { (yyval.num) = PRIV_SET; }
#line 2992 "y.tab.c"
    break;

  case 164: /* privtype: TPRIV_WHOX  */
#line 735 "./ircd_parser.y"
                     { (yyval.num) = PRIV_WHOX; }
#line 2998 "y.tab.c"
    break;

  case 165: /* privtype: TPRIV_BADCHAN  */
#line 736 "./ircd_parser.y"
                        { (yyval.num) = PRIV_BADCHAN; }
#line 3004 "y.tab.c"
    break;

  case 166: /* privtype: TPRIV_LOCAL_BADCHAN  */
#line 737 "./ircd_parser.y"
                              { (yyval.num) = PRIV_LOCAL_BADCHAN; }
#line 3010 "y.tab.c"
    break;

  case 167: /* privtype: TPRIV_SEE_CHAN  */
#line 738 "./ircd_parser.y"
                         { (yyval.num) = PRIV_SEE_CHAN; }
#line 3016 "y.tab.c"
    break;

  case 168: /* privtype: TPRIV_SHOW_INVIS  */
#line 739 "./ircd_parser.y"
                           { (yyval.num) = PRIV_SHOW_INVIS; }
#line 3022 "y.tab.c"
    break;

  case 169: /* privtype: TPRIV_SHOW_ALL_INVIS  */
#line 740 "./ircd_parser.y"
                               { (yyval.num) = PRIV_SHOW_ALL_INVIS; }
#line 3028 "y.tab.c"
    break;

  case 170: /* privtype: TPRIV_PROPAGATE  */
#line 741 "./ircd_parser.y"
                          { (yyval.num) = PRIV_PROPAGATE; }
#line 3034 "y.tab.c"
    break;

  case 171: /* privtype: TPRIV_UNLIMIT_QUERY  */
#line 742 "./ircd_parser.y"
                              { (yyval.num) = PRIV_UNLIMIT_QUERY; }
#line 3040 "y.tab.c"
    break;

  case 172: /* privtype: TPRIV_DISPLAY  */
#line 743 "./ircd_parser.y"
                        { (yyval.num) = PRIV_DISPLAY; }
#line 3046 "y.tab.c"
    break;

  case 173: /* privtype: TPRIV_SEE_OPERS  */
#line 744 "./ircd_parser.y"
                          { (yyval.num) = PRIV_SEE_OPERS; }
#line 3052 "y.tab.c"
    break;

  case 174: /* privtype: TPRIV_WIDE_GLINE  */
#line 745 "./ircd_parser.y"
                           { (yyval.num) = PRIV_WIDE_GLINE; }
#line 3058 "y.tab.c"
    break;

  case 175: /* privtype: TPRIV_LIST_CHAN  */
#line 746 "./ircd_parser.y"
                          { (yyval.num) = PRIV_LIST_CHAN; }
#line 3064 "y.tab.c"
    break;

  case 176: /* privtype: LOCAL  */
#line 747 "./ircd_parser.y"
                { (yyval.num) = PRIV_PROPAGATE; invert = 1; }
#line 3070 "y.tab.c"
    break;

  case 177: /* privtype: TPRIV_FORCE_OPMODE  */
#line 748 "./ircd_parser.y"
                             { (yyval.num) = PRIV_FORCE_OPMODE; }
#line 3076 "y.tab.c"
    break;

  case 178: /* privtype: TPRIV_FORCE_LOCAL_OPMODE  */
#line 749 "./ircd_parser.y"
                                   { (yyval.num) = PRIV_FORCE_LOCAL_OPMODE; }
#line 3082 "y.tab.c"
    break;

  case 179: /* privtype: TPRIV_APASS_OPMODE  */
#line 750 "./ircd_parser.y"
                             { (yyval.num) = PRIV_APASS_OPMODE; }
#line 3088 "y.tab.c"
    break;

  case 180: /* privtype: TPRIV_CHANSERV  */
#line 751 "./ircd_parser.y"
                         { (yyval.num) = PRIV_CHANSERV; }
#line 3094 "y.tab.c"
    break;

  case 181: /* privtype: TPRIV_XTRA_OPER  */
#line 752 "./ircd_parser.y"
                          { (yyval.num) = PRIV_XTRA_OPER; }
#line 3100 "y.tab.c"
    break;

  case 182: /* privtype: TPRIV_NOIDLE  */
#line 753 "./ircd_parser.y"
                       { (yyval.num) = PRIV_NOIDLE; }
#line 3106 "y.tab.c"
    break;

  case 183: /* privtype: TPRIV_FREEFORM  */
#line 754 "./ircd_parser.y"
                         { (yyval.num) = PRIV_FREEFORM; }
#line 3112 "y.tab.c"
    break;

  case 184: /* privtype: TPRIV_CHECK  */
#line 755 "./ircd_parser.y"
                      { (yyval.num) = PRIV_CHECK; }
#line 3118 "y.tab.c"
    break;

  case 185: /* privtype: TPRIV_PARANOID  */
#line 756 "./ircd_parser.y"
                         { (yyval.num) = PRIV_PARANOID; }
#line 3124 "y.tab.c"
    break;

  case 186: /* yesorno: YES  */
#line 757 "./ircd_parser.y"
             { (yyval.num) = 1; }
#line 3130 "y.tab.c"
    break;

  case 187: /* yesorno: NO  */
#line 757 "./ircd_parser.y"
                              { (yyval.num) = 0; }
#line 3136 "y.tab.c"
    break;

  case 188: /* tlsfingerprint: TLS FINGERPRINT '=' QSTRING ';'  */
#line 760 "./ircd_parser.y"
{
  MyFree(tls_fingerprint);
  tls_fingerprint = (yyvsp[-1].text);
}
#line 3145 "y.tab.c"
    break;

  case 189: /* tlsciphers: TLS CIPHERS '=' QSTRING ';'  */
#line 765 "./ircd_parser.y"
{
  MyFree(tls_ciphers);
  tls_ciphers = (yyvsp[-1].text);
}
#line 3154 "y.tab.c"
    break;

  case 190: /* address_family: %empty  */
#line 774 "./ircd_parser.y"
               { (yyval.num) = 0; }
#line 3160 "y.tab.c"
    break;

  case 191: /* address_family: TOK_IPV4  */
#line 775 "./ircd_parser.y"
               { (yyval.num) = USE_IPV4; }
#line 3166 "y.tab.c"
    break;

  case 192: /* address_family: TOK_IPV6  */
#line 776 "./ircd_parser.y"
               { (yyval.num) = USE_IPV6; }
#line 3172 "y.tab.c"
    break;

  case 193: /* address_family: TOK_IPV4 TOK_IPV6  */
#line 777 "./ircd_parser.y"
                        { (yyval.num) = USE_IPV4 | USE_IPV6; }
#line 3178 "y.tab.c"
    break;

  case 194: /* address_family: TOK_IPV6 TOK_IPV4  */
#line 778 "./ircd_parser.y"
                        { (yyval.num) = USE_IPV6 | USE_IPV4; }
#line 3184 "y.tab.c"
    break;

  case 195: /* portblock: PORT '{' portitems '}' ';'  */
#line 782 "./ircd_parser.y"
                                      {
  struct ListenerFlags flags_here;
  struct SLink *link;
  if (hosts == NULL) {
    struct SLink *link;
    link = make_link();
    DupString(link->value.cp, "*");
    link->flags = 0;
    link->next = hosts;
    hosts = link;
  }
  if (!FlagHas(&listen_flags, LISTEN_TLS))
    MyFree(tls_ciphers);  
  for (link = hosts; link != NULL; link = link->next) {
    memcpy(&flags_here, &listen_flags, sizeof(&flags_here));
    switch (link->flags & (USE_IPV4 | USE_IPV6)) {
    case USE_IPV4:
      FlagSet(&flags_here, LISTEN_IPV4);
      break;
    case USE_IPV6:
      FlagSet(&flags_here, LISTEN_IPV6);
      break;
    default: /* 0 or USE_IPV4|USE_IPV6 */
      FlagSet(&flags_here, LISTEN_IPV4);
      FlagSet(&flags_here, LISTEN_IPV6);
      break;
    }
    if (link->flags & 65535)
      port = link->flags & 65535;
    add_listener(port, link->value.cp, pass, tls_ciphers, &flags_here);
  }
  free_slist(&hosts);
  MyFree(pass);
  MyFree(tls_ciphers);
  memset(&listen_flags, 0, sizeof(listen_flags));
  pass = NULL;
  port = 0;
}
#line 3227 "y.tab.c"
    break;

  case 206: /* portnumber: PORT '=' address_family NUMBER ';'  */
#line 824 "./ircd_parser.y"
{
  if ((yyvsp[-1].num) < 1 || (yyvsp[-1].num) > 65535) {
    parse_error("Port %d is out of range", port);
  } else {
    port = (yyvsp[-2].num) | (yyvsp[-1].num);
    if (hosts && (0 == (hosts->flags & 65535)))
      hosts->flags = (hosts->flags & ~65535) | port;
  }
}
#line 3241 "y.tab.c"
    break;

  case 207: /* portvhost: VHOST '=' address_family QSTRING ';'  */
#line 835 "./ircd_parser.y"
{
  struct SLink *link;
  link = make_link();
  link->value.cp = (yyvsp[-1].text);
  link->flags = (yyvsp[-2].num) | port;
  link->next = hosts;
  hosts = link;
}
#line 3254 "y.tab.c"
    break;

  case 208: /* portvhostnumber: VHOST '=' address_family QSTRING NUMBER ';'  */
#line 845 "./ircd_parser.y"
{
  if ((yyvsp[-1].num) < 1 || (yyvsp[-1].num) > 65535) {
    parse_error("Port %d is out of range", port);
  } else {
    struct SLink *link;
    link = make_link();
    link->value.cp = (yyvsp[-2].text);
    link->flags = (yyvsp[-3].num) | (yyvsp[-1].num);
    link->next = hosts;
    hosts = link;
  }
}
#line 3271 "y.tab.c"
    break;

  case 209: /* portmask: MASK '=' QSTRING ';'  */
#line 859 "./ircd_parser.y"
{
  MyFree(pass);
  pass = (yyvsp[-1].text);
}
#line 3280 "y.tab.c"
    break;

  case 210: /* portserver: SERVER '=' YES ';'  */
#line 865 "./ircd_parser.y"
{
  FlagSet(&listen_flags, LISTEN_SERVER);
}
#line 3288 "y.tab.c"
    break;

  case 211: /* portserver: SERVER '=' NO ';'  */
#line 868 "./ircd_parser.y"
{
  FlagClr(&listen_flags, LISTEN_SERVER);
}
#line 3296 "y.tab.c"
    break;

  case 212: /* porthidden: HIDDEN '=' YES ';'  */
#line 873 "./ircd_parser.y"
{
  FlagSet(&listen_flags, LISTEN_HIDDEN);
}
#line 3304 "y.tab.c"
    break;

  case 213: /* porthidden: HIDDEN '=' NO ';'  */
#line 876 "./ircd_parser.y"
{
  FlagClr(&listen_flags, LISTEN_HIDDEN);
}
#line 3312 "y.tab.c"
    break;

  case 214: /* porttls: TLS '=' YES ';'  */
#line 881 "./ircd_parser.y"
{
  FlagSet(&listen_flags, LISTEN_TLS);
}
#line 3320 "y.tab.c"
    break;

  case 215: /* porttls: TLS '=' NO ';'  */
#line 884 "./ircd_parser.y"
{
  FlagClr(&listen_flags, LISTEN_TLS);
}
#line 3328 "y.tab.c"
    break;

  case 216: /* $@5: %empty  */
#line 889 "./ircd_parser.y"
{
  maxlinks = 65535;
  port = 0;
}
#line 3337 "y.tab.c"
    break;

  case 217: /* clientblock: CLIENT $@5 '{' clientitems '}' ';'  */
#line 894 "./ircd_parser.y"
{
  struct ConfItem *aconf = 0;
  struct irc_in_addr addr;
  unsigned char addrbits = 0;

  if (!c_class)
    parse_error("Invalid or missing class in Client block");
  else if (pass && strlen(pass) > PASSWDLEN)
    parse_error("Password too long in connect block");
  else if (ip && !ipmask_parse(ip, &addr, &addrbits))
    parse_error("Invalid IP address %s in Client block", ip);
  else {
    aconf = make_conf(CONF_CLIENT);
    aconf->username = username;
    aconf->host = host;
    if (ip)
      memcpy(&aconf->address.addr, &addr, sizeof(aconf->address.addr));
    else
      memset(&aconf->address.addr, 0, sizeof(aconf->address.addr));
    aconf->address.port = port;
    aconf->addrbits = addrbits;
    aconf->name = ip;
    aconf->conn_class = c_class;
    aconf->maximum = maxlinks;
    aconf->passwd = pass;
  }
  if (!aconf) {
    MyFree(username);
    MyFree(host);
    MyFree(ip);
    MyFree(pass);
  }
  host = NULL;
  username = NULL;
  c_class = NULL;
  ip = NULL;
  pass = NULL;
  port = 0;
}
#line 3381 "y.tab.c"
    break;

  case 227: /* clienthost: HOST '=' QSTRING ';'  */
#line 936 "./ircd_parser.y"
{
  char *sep = strchr((yyvsp[-1].text), '@');
  MyFree(host);
  if (sep) {
    *sep++ = '\0';
    MyFree(username);
    DupString(host, sep);
    username = (yyvsp[-1].text);
  } else {
    host = (yyvsp[-1].text);
  }
}
#line 3398 "y.tab.c"
    break;

  case 228: /* clientip: IP '=' QSTRING ';'  */
#line 949 "./ircd_parser.y"
{
  char *sep;
  sep = strchr((yyvsp[-1].text), '@');
  MyFree(ip);
  if (sep) {
    *sep++ = '\0';
    MyFree(username);
    DupString(ip, sep);
    username = (yyvsp[-1].text);
  } else {
    ip = (yyvsp[-1].text);
  }
}
#line 3416 "y.tab.c"
    break;

  case 229: /* clientusername: USERNAME '=' QSTRING ';'  */
#line 963 "./ircd_parser.y"
{
  MyFree(username);
  username = (yyvsp[-1].text);
}
#line 3425 "y.tab.c"
    break;

  case 230: /* clientclass: CLASS '=' QSTRING ';'  */
#line 968 "./ircd_parser.y"
{
  c_class = find_class((yyvsp[-1].text));
  if (!c_class)
    parse_error("No such connection class '%s' for Client block", (yyvsp[-1].text));
  MyFree((yyvsp[-1].text));
}
#line 3436 "y.tab.c"
    break;

  case 231: /* clientpass: PASS '=' QSTRING ';'  */
#line 975 "./ircd_parser.y"
{
  MyFree(pass);
  pass = (yyvsp[-1].text);
}
#line 3445 "y.tab.c"
    break;

  case 232: /* clientmaxlinks: MAXLINKS '=' expr ';'  */
#line 980 "./ircd_parser.y"
{
  maxlinks = (yyvsp[-1].num);
}
#line 3453 "y.tab.c"
    break;

  case 233: /* clientport: PORT '=' expr ';'  */
#line 984 "./ircd_parser.y"
{
  port = (yyvsp[-1].num);
}
#line 3461 "y.tab.c"
    break;

  case 234: /* $@6: %empty  */
#line 989 "./ircd_parser.y"
{
  dconf = (struct DenyConf*) MyCalloc(1, sizeof(*dconf));
}
#line 3469 "y.tab.c"
    break;

  case 235: /* killblock: KILL $@6 '{' killitems '}' ';'  */
#line 992 "./ircd_parser.y"
{
  if (dconf->usermask || dconf->hostmask ||dconf->realmask) {
    dconf->next = denyConfList;
    denyConfList = dconf;
  }
  else
  {
    MyFree(dconf->usermask);
    MyFree(dconf->hostmask);
    MyFree(dconf->realmask);
    MyFree(dconf->message);
    MyFree(dconf);
    parse_error("Kill block must match on at least one of username, host or realname");
  }
  dconf = NULL;
}
#line 3490 "y.tab.c"
    break;

  case 243: /* killuhost: HOST '=' QSTRING ';'  */
#line 1011 "./ircd_parser.y"
{
  char *h;
  MyFree(dconf->hostmask);
  MyFree(dconf->usermask);
  if ((h = strchr((yyvsp[-1].text), '@')) == NULL)
  {
    DupString(dconf->usermask, "*");
    dconf->hostmask = (yyvsp[-1].text);
  }
  else
  {
    *h++ = '\0';
    DupString(dconf->hostmask, h);
    dconf->usermask = (yyvsp[-1].text);
  }
  ipmask_parse(dconf->hostmask, &dconf->address, &dconf->bits);
}
#line 3512 "y.tab.c"
    break;

  case 244: /* killusername: USERNAME '=' QSTRING ';'  */
#line 1030 "./ircd_parser.y"
{
  MyFree(dconf->usermask);
  dconf->usermask = (yyvsp[-1].text);
}
#line 3521 "y.tab.c"
    break;

  case 245: /* killreal: REAL '=' QSTRING ';'  */
#line 1036 "./ircd_parser.y"
{
 MyFree(dconf->realmask);
 dconf->realmask = (yyvsp[-1].text);
}
#line 3530 "y.tab.c"
    break;

  case 246: /* killreason: REASON '=' QSTRING ';'  */
#line 1042 "./ircd_parser.y"
{
 dconf->flags &= ~DENY_FLAGS_FILE;
 MyFree(dconf->message);
 dconf->message = (yyvsp[-1].text);
}
#line 3540 "y.tab.c"
    break;

  case 247: /* killreasonfile: TFILE '=' QSTRING ';'  */
#line 1049 "./ircd_parser.y"
{
 dconf->flags |= DENY_FLAGS_FILE;
 MyFree(dconf->message);
 dconf->message = (yyvsp[-1].text);
}
#line 3550 "y.tab.c"
    break;

  case 248: /* $@7: %empty  */
#line 1056 "./ircd_parser.y"
{
  tconn = CRULE_AUTO;
}
#line 3558 "y.tab.c"
    break;

  case 249: /* cruleblock: CRULE $@7 '{' cruleitems '}' ';'  */
#line 1059 "./ircd_parser.y"
{
  struct CRuleNode *node = NULL;
  struct SLink *link;

  if (hosts == NULL)
    parse_error("Missing server(s) in crule block");
  else if (pass == NULL)
    parse_error("Missing rule in crule block");
  else if ((node = crule_parse(pass)) == NULL)
    parse_error("Invalid rule '%s' in crule block", pass);
  else for (link = hosts; link != NULL; link = link->next)
  {
    struct CRuleConf *p = (struct CRuleConf*) MyMalloc(sizeof(*p));
    if (node == NULL)
      node = crule_parse(pass);
    DupString(p->hostmask, link->value.cp);
    DupString(p->rule, pass);
    p->type = tconn;
    p->node = node;
    node = NULL;
    p->next = cruleConfList;
    cruleConfList = p;
  }
  free_slist(&hosts);
  MyFree(pass);
  pass = NULL;
  tconn = 0;
}
#line 3591 "y.tab.c"
    break;

  case 255: /* cruleserver: SERVER '=' QSTRING ';'  */
#line 1092 "./ircd_parser.y"
{
  struct SLink *link;
  link = make_link();
  link->value.cp = (yyvsp[-1].text);
  link->next = hosts;
  hosts = link;
}
#line 3603 "y.tab.c"
    break;

  case 256: /* crulerule: RULE '=' QSTRING ';'  */
#line 1101 "./ircd_parser.y"
{
 MyFree(pass);
 pass = (yyvsp[-1].text);
}
#line 3612 "y.tab.c"
    break;

  case 257: /* cruleall: ALL '=' YES ';'  */
#line 1107 "./ircd_parser.y"
{
 tconn = CRULE_ALL;
}
#line 3620 "y.tab.c"
    break;

  case 258: /* cruleall: ALL '=' NO ';'  */
#line 1110 "./ircd_parser.y"
{
 tconn = CRULE_AUTO;
}
#line 3628 "y.tab.c"
    break;

  case 259: /* motdblock: MOTD '{' motditems '}' ';'  */
#line 1115 "./ircd_parser.y"
{
  struct SLink *link;
  if (pass != NULL)
    for (link = hosts; link != NULL; link = link->next)
      motd_add(link->value.cp, pass);
  free_slist(&hosts);
  MyFree(pass);
  pass = NULL;
}
#line 3642 "y.tab.c"
    break;

  case 264: /* motdhost: HOST '=' QSTRING ';'  */
#line 1128 "./ircd_parser.y"
{
  struct SLink *link;
  link = make_link();
  link->value.cp = (yyvsp[-1].text);
  link->next = hosts;
  hosts = link;
}
#line 3654 "y.tab.c"
    break;

  case 265: /* motdfile: TFILE '=' QSTRING ';'  */
#line 1137 "./ircd_parser.y"
{
  MyFree(pass);
  pass = (yyvsp[-1].text);
}
#line 3663 "y.tab.c"
    break;

  case 269: /* $@8: %empty  */
#line 1146 "./ircd_parser.y"
{
  stringlist[0] = (yyvsp[0].text);
  stringno = 1;
}
#line 3672 "y.tab.c"
    break;

  case 270: /* featureitem: QSTRING $@8 '=' stringlist ';'  */
#line 1149 "./ircd_parser.y"
                     {
  unsigned int ii;
  feature_set(NULL, (const char * const *)stringlist, stringno);
  for (ii = 0; ii < stringno; ++ii)
    MyFree(stringlist[ii]);
}
#line 3683 "y.tab.c"
    break;

  case 273: /* extrastring: QSTRING  */
#line 1158 "./ircd_parser.y"
{
  if (stringno < MAX_STRINGS)
    stringlist[stringno++] = (yyvsp[0].text);
  else
    MyFree((yyvsp[0].text));
}
#line 3694 "y.tab.c"
    break;

  case 277: /* quarantineitem: QSTRING '=' QSTRING ';'  */
#line 1168 "./ircd_parser.y"
{
  struct qline *qconf = MyCalloc(1, sizeof(*qconf));
  qconf->chname = (yyvsp[-3].text);
  qconf->reason = (yyvsp[-1].text);
  qconf->next = GlobalQuarantineList;
  GlobalQuarantineList = qconf;
}
#line 3706 "y.tab.c"
    break;

  case 278: /* $@9: %empty  */
#line 1177 "./ircd_parser.y"
{
  smap = MyCalloc(1, sizeof(struct s_map));
  smap->command = (yyvsp[-1].text);
}
#line 3715 "y.tab.c"
    break;

  case 279: /* pseudoblock: PSEUDO QSTRING '{' $@9 pseudoitems '}' ';'  */
#line 1182 "./ircd_parser.y"
{
  int valid = 0;

  if (!smap->name)
    parse_error("Missing name in pseudo %s block", smap->command);
  else if (!smap->services)
    parse_error("Missing nick in pseudo %s block", smap->command);
  else if (!strIsAlpha(smap->command))
    parse_error("Pseudo command %s invalid: must all be letters", smap->command);
  else
    valid = 1;
  if (valid && register_mapping(smap))
  {
    smap->next = GlobalServiceMapList;
    GlobalServiceMapList = smap;
  }
  else
  {
    free_mapping(smap);
  }
  smap = NULL;
}
#line 3742 "y.tab.c"
    break;

  case 286: /* pseudoname: NAME '=' QSTRING ';'  */
#line 1208 "./ircd_parser.y"
{
  MyFree(smap->name);
  smap->name = (yyvsp[-1].text);
}
#line 3751 "y.tab.c"
    break;

  case 287: /* pseudoprepend: PREPEND '=' QSTRING ';'  */
#line 1213 "./ircd_parser.y"
{
  MyFree(smap->prepend);
  smap->prepend = (yyvsp[-1].text);
}
#line 3760 "y.tab.c"
    break;

  case 288: /* pseudonick: NICK '=' QSTRING ';'  */
#line 1218 "./ircd_parser.y"
{
  char *sep = strchr((yyvsp[-1].text), '@');

  if (sep != NULL) {
    size_t slen = strlen((yyvsp[-1].text));
    struct nick_host *nh = MyMalloc(sizeof(*nh) + slen);
    memcpy(nh->nick, (yyvsp[-1].text), slen + 1);
    nh->nicklen = sep - (yyvsp[-1].text);
    nh->next = smap->services;
    smap->services = nh;
  }
  MyFree((yyvsp[-1].text));
}
#line 3778 "y.tab.c"
    break;

  case 289: /* pseudoflags: FAST ';'  */
#line 1232 "./ircd_parser.y"
{
  smap->flags |= SMAP_FAST;
}
#line 3786 "y.tab.c"
    break;

  case 290: /* iauthblock: IAUTH '{' iauthitems '}' ';'  */
#line 1237 "./ircd_parser.y"
{
  auth_spawn(stringno, stringlist);
  while (stringno > 0)
  {
    --stringno;
    MyFree(stringlist[stringno]);
  }
}
#line 3799 "y.tab.c"
    break;

  case 294: /* $@10: %empty  */
#line 1249 "./ircd_parser.y"
{
  while (stringno > 0)
  {
    --stringno;
    MyFree(stringlist[stringno]);
  }
}
#line 3811 "y.tab.c"
    break;

  case 296: /* $@11: %empty  */
#line 1258 "./ircd_parser.y"
{
  spoof = MyCalloc(1, sizeof(struct sline));
  spoof->spoofhost = (yyvsp[-1].text);
  spoof->passwd = NULL;
  spoof->realhost = NULL;
  spoof->username = NULL;
}
#line 3823 "y.tab.c"
    break;

  case 297: /* spoofblock: SPOOFHOST QSTRING '{' $@11 spoofitems '}' ';'  */
#line 1266 "./ircd_parser.y"
{
  struct irc_in_addr ip;
  char bits;
  int valid = 0;

  if (spoof->username == NULL && spoof->realhost) {
    parse_error("Username missing in spoofhost.");
  } else if (spoof->realhost == NULL && spoof->username) {
    parse_error("Realhost missing in spoofhost.");
  } else 
    valid = 1;

  if (valid) {
    if (spoof->realhost) {
      if (!string_has_wildcards(spoof->realhost)) {
        if (ipmask_parse(spoof->realhost, &ip, &bits) != 0) {
          spoof->address = ip;
          spoof->bits = bits;
          spoof->flags = SLINE_FLAGS_IP;
        } else {
          Debug((DEBUG_DEBUG, "S-Line: \"%s\" appears not to be a valid IP address, might be wildcarded.", spoof->realhost));
          spoof->flags = SLINE_FLAGS_HOSTNAME;
        }
      } else
        spoof->flags = SLINE_FLAGS_HOSTNAME;
    } else
      spoof->flags = 0;

    spoof->next = GlobalSList;
    GlobalSList = spoof;
  } else {
    MyFree(spoof->spoofhost);
    MyFree(spoof->passwd);
    MyFree(spoof->realhost);
    MyFree(spoof->username);
    MyFree(spoof);
  }
  spoof = NULL;
}
#line 3867 "y.tab.c"
    break;

  case 303: /* spoofpassword: PASS '=' QSTRING ';'  */
#line 1309 "./ircd_parser.y"
{
  MyFree(spoof->passwd);
  spoof->passwd = (yyvsp[-1].text);
}
#line 3876 "y.tab.c"
    break;

  case 304: /* spoofrealhost: HOST '=' QSTRING ';'  */
#line 1314 "./ircd_parser.y"
{
  MyFree(spoof->realhost);
  spoof->realhost = (yyvsp[-1].text);
}
#line 3885 "y.tab.c"
    break;

  case 305: /* spoofrealident: USERNAME '=' QSTRING ';'  */
#line 1319 "./ircd_parser.y"
{
  MyFree(spoof->username);
  spoof->username = (yyvsp[-1].text);
}
#line 3894 "y.tab.c"
    break;


#line 3898 "y.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

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
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
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
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

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
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

