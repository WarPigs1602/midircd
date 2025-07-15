/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Define if you have (reliable) BSD signals. */
/* #undef BSD_RELIABLE_SIGNALS */

/* Configuration file name */
#define CPATH "ircd.conf"

/* Enable debugging code */
/* #undef DEBUGMODE */

/* Domain name to be used for some statistics gathering */
#define DOMAINNAME "*icicddbjmfvephrpzukohovmhb.bx.internal.cloudapp.net"

/* Path to data directory */
#define DPATH "/home/runner/lib"

/* Define to implement epoll system calls */
/* #undef EPOLL_NEED_BODY */

/* Force inlining for a few critical functions */
#define FORCEINLINE 1

/* Define to 1 if you have the <crypt.h> header file. */
#define HAVE_CRYPT_H 1

/* Define to 1 if you have the `getrusage' function. */
#define HAVE_GETRUSAGE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `kqueue' function. */
/* #undef HAVE_KQUEUE */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `resolv' library (-lresolv). */
/* #undef HAVE_LIBRESOLV */

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the <memory.h> header file. */
/* #undef HAVE_MEMORY_H */

/* Define to 1 if you have the <poll.h> header file. */
#define HAVE_POLL_H 1

/* Define to 1 if system calls automatically restart after interruption by a
   signal. */
/* #undef HAVE_RESTARTABLE_SYSCALLS */

/* Define to 1 if you have the `setrlimit' function. */
#define HAVE_SETRLIMIT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/devpoll.h> header file. */
/* #undef HAVE_SYS_DEVPOLL_H */

/* Define to 1 if you have the <sys/epoll.h> header file. */
#define HAVE_SYS_EPOLL_H 1

/* Define to 1 if you have the <sys/event.h> header file. */
/* #undef HAVE_SYS_EVENT_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
/* #undef HAVE_SYS_WAIT_H */

/* Define to 1 if you have the `times' function. */
#define HAVE_TIMES 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if we have va_copy */
#define HAVE_VA_COPY 1

/* Define if we have __va_copy */
#define HAVE___VA_COPY 1

/* Enable IPv6 support */
#define IPV6 1

/* Define if building on Solaris */
/* #undef IRCU_SOLARIS */

/* Path to debugging log file */
#define LPATH "ircd.log"

/* Maximum number of network connections */
#define MAXCONNECTIONS 65532

/* Define if you have BSD non-blocking sockets. */
/* #undef NBLOCK_BSD */

/* Define if you have POSIX non-blocking sockets. */
#define NBLOCK_POSIX /**/

/* Define if you have SysV non-blocking sockets. */
/* #undef NBLOCK_SYSV */

/* Disable assertions */
/* #undef NDEBUG */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define if you have POSIX signals. */
#define POSIX_SIGNALS /**/

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `int64_t', as computed by sizeof. */
#define SIZEOF_INT64_T 8

/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of a `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of a `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of a `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* Path to executable for restarts */
#define SPATH "/home/runner/bin/ircd"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if you have (unreliable) SysV signals. */
/* #undef SYSV_UNRELIABLE_SIGNALS */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Define to enable the /dev/poll engine */
/* #undef USE_DEVPOLL */

/* Define to enable the epoll engine */
#define USE_EPOLL 1

/* Define to enable the kqueue engine */
/* #undef USE_KQUEUE */

/* Specify whether or not to use poll() */
#define USE_POLL 1

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to `short' if <sys/types.h> does not define. */
/* #undef int16_t */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef int32_t */

/* Define to `long long' if <sys/types.h> does not define. */
/* #undef int64_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* type to use in place of socklen_t if not defined */
/* #undef socklen_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define to `unsigned short' if <sys/types.h> does not define. */
/* #undef uint16_t */

/* Define to `unsigned long' if <sys/types.h> does not define. */
/* #undef uint32_t */

/* Define to `unsigned long long' if <sys/types.h> does not define. */
/* #undef uint64_t */
