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

#line 298 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
