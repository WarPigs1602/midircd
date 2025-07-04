/************************************************************************
 *   IRC - Internet Relay Chat, src/s_auth.c
 *   Copyright (C) 1992 Darren Reed
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Changes:
 *   July 6, 1999 - Rewrote most of the code here. When a client connects
 *     to the server and passes initial socket validation checks, it
 *     is owned by this module (auth) which returns it to the rest of the
 *     server when dns and auth queries are finished. Until the client is
 *     released, the server does not know it exists and does not process
 *     any messages from it.
 *     --Bleep  Thomas Helvey <tomh@inxpress.net>
 *
 *  December 26, 2005 - Rewrite the flag handling and integrate that with
 *     an IRCnet-style IAuth protocol.
 *     -- Michael Poole
 */
/** @file
 * @brief Implementation of DNS and ident lookups.
 * @version $Id: s_auth.c,v 1.37.2.25 2007/08/09 03:46:20 entrope Exp $
 */
#include "config.h"

#include "s_auth.h"
#include "class.h"
#include "client.h"
#include "hash.h"
#include "IPcheck.h"
#include "ircd.h"
#include "ircd_alloc.h"
#include "ircd_chattr.h"
#include "ircd_events.h"
#include "ircd_features.h"
#include "ircd_log.h"
#include "ircd_osdep.h"
#include "ircd_reply.h"
#include "ircd_snprintf.h"
#include "ircd_string.h"
#include "ircd_tls.h"
#include "list.h"
#include "msg.h"	/* for MAXPARA */
#include "numeric.h"
#include "querycmds.h"
#include "random.h"
#include "res.h"
#include "s_bsd.h"
#include "s_conf.h"
#include "s_debug.h"
#include "s_misc.h"
#include "s_user.h"
#include "send.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

/** Pending operations during registration. */
enum AuthRequestFlag {
    AR_AUTH_PENDING,    /**< ident connecting or waiting for response */
    AR_DNS_PENDING,     /**< dns request sent, waiting for response */
    AR_CAP_PENDING,     /**< in middle of CAP negotiations */
    AR_NEEDS_PONG,      /**< user has not PONGed */
    AR_NEEDS_USER,      /**< user must send USER command */
    AR_NEEDS_NICK,      /**< user must send NICK command */
    AR_LAST_SCAN = AR_NEEDS_NICK, /**< maximum flag to scan through */
    AR_IAUTH_PENDING,   /**< iauth request sent, waiting for response */
    AR_IAUTH_HURRY,     /**< we told iauth to hurry up */
    AR_IAUTH_USERNAME,  /**< iauth sent a username (preferred or forced) */
    AR_IAUTH_FUSERNAME, /**< iauth sent a forced username */
    AR_PASSWORD_CHECKED, /**< client password already checked */
    AR_NUM_FLAGS
};

DECLARE_FLAGSET(AuthRequestFlags, AR_NUM_FLAGS);

/** Stores registration state of a client. */
struct AuthRequest {
  struct AuthRequest* next;       /**< linked list node ptr */
  struct AuthRequest* prev;       /**< linked list node ptr */
  struct Client*      client;     /**< pointer to client struct for request */
  struct irc_sockaddr local;      /**< local endpoint address */
  struct irc_in_addr  original;   /**< original client IP address */
  struct Socket       socket;     /**< socket descriptor for auth queries */
  struct Timer        timeout;    /**< timeout timer for ident and dns queries */
  struct AuthRequestFlags flags;  /**< current state of request */
  unsigned int        cookie;     /**< cookie the user must PONG */
  unsigned short      port;       /**< client's remote port number */
};

/** Array of message text (with length) pairs for AUTH status
 * messages.  Indexed using #ReportType.
 */
static struct {
  const char*  message;
  unsigned int length;
} HeaderMessages [] = {
#define MSG(STR) { STR, sizeof(STR) - 1 }
  MSG("NOTICE AUTH :*** Looking up your hostname\r\n"),
  MSG("NOTICE AUTH :*** Found your hostname\r\n"),
  MSG("NOTICE AUTH :*** Couldn't look up your hostname\r\n"),
  MSG("NOTICE AUTH :*** Checking Ident\r\n"),
  MSG("NOTICE AUTH :*** Got ident response\r\n"),
  MSG("NOTICE AUTH :*** No ident response\r\n"),
  MSG("NOTICE AUTH :*** \r\n"),
  MSG("NOTICE AUTH :*** Your forward and reverse DNS do not match, "
    "ignoring hostname.\r\n"),
  MSG("NOTICE AUTH :*** Invalid hostname\r\n")
#undef MSG
};

/** Enum used to index messages in the HeaderMessages[] array. */
typedef enum {
  REPORT_DO_DNS,
  REPORT_FIN_DNS,
  REPORT_FAIL_DNS,
  REPORT_DO_ID,
  REPORT_FIN_ID,
  REPORT_FAIL_ID,
  REPORT_FAIL_IAUTH,
  REPORT_IP_MISMATCH,
  REPORT_INVAL_DNS
} ReportType;

/** Enumeration of IAuth connection flags. */
enum IAuthFlag
{
  IAUTH_BLOCKED,                        /**< socket buffer full */
  IAUTH_CLOSING,                        /**< candidate to be disposed */
  /* The following flags are controlled by iauth's "O" options command. */
  IAUTH_ADDLINFO,                       /**< Send additional info
                                         * (password and username). */
  IAUTH_FIRST_OPTION = IAUTH_ADDLINFO,  /**< First flag that is a policy option. */
  IAUTH_REQUIRED,                       /**< IAuth completion required for registration. */
  IAUTH_TIMEOUT,                        /**< Refuse new connections if IAuth is behind. */
  IAUTH_EXTRAWAIT,                      /**< Give IAuth extra time to answer. */
  IAUTH_UNDERNET,                       /**< Enable Undernet extensions. */
  IAUTH_LAST_FLAG                       /**< total number of flags */
};
/** Declare a bitset structure indexed by IAuthFlag. */
DECLARE_FLAGSET(IAuthFlags, IAUTH_LAST_FLAG);

/** Describes state of an IAuth connection. */
struct IAuth {
  struct MsgQ i_sendQ;                  /**< messages queued to send */
  struct Socket i_socket;               /**< main socket to iauth */
  struct Socket i_stderr;               /**< error socket for iauth */
  struct IAuthFlags i_flags;            /**< connection state/status/flags */
  uint64_t i_recvB;                     /**< bytes received */
  uint64_t i_sendB;                     /**< bytes sent */
  time_t started;                       /**< time that this instance was started */
  unsigned int i_recvM;                 /**< messages received */
  unsigned int i_sendM;                 /**< messages sent */
  unsigned int i_count;                 /**< characters used in i_buffer */
  unsigned int i_errcount;              /**< characters used in i_errbuf */
  int i_debug;                          /**< debug level */
  char i_buffer[BUFSIZE+1];             /**< partial unprocessed line from server */
  char i_errbuf[BUFSIZE+1];             /**< partial unprocessed error line */
  char *i_version;                      /**< iauth version string */
  struct SLink *i_config;               /**< configuration string list */
  struct SLink *i_stats;                /**< statistics string list */
  char **i_argv;                        /**< argument list */
};

/** Return whether flag \a flag is set on \a iauth. */
#define IAuthHas(iauth, flag) ((iauth) && FlagHas(&(iauth)->i_flags, flag))
/** Set flag \a flag on \a iauth. */
#define IAuthSet(iauth, flag) FlagSet(&(iauth)->i_flags, flag)
/** Clear flag \a flag from \a iauth. */
#define IAuthClr(iauth, flag) FlagClr(&(iauth)->i_flags, flag)
/** Get connected flag for \a iauth. */
#define i_GetConnected(iauth) ((iauth) && s_fd(i_socket(iauth)) > -1)

/** Return socket event generator for \a iauth. */
#define i_socket(iauth) (&(iauth)->i_socket)
/** Return stderr socket for \a iauth. */
#define i_stderr(iauth) (&(iauth)->i_stderr)
/** Return outbound message queue for \a iauth. */
#define i_sendQ(iauth) (&(iauth)->i_sendQ)
/** Return debug level for \a iauth. */
#define i_debug(iauth) ((iauth)->i_debug)

/** Active instance of IAuth. */
static struct IAuth *iauth;
/** Freelist of AuthRequest structures. */
static struct AuthRequest *auth_freelist;

static void iauth_sock_callback(struct Event *ev);
static void iauth_stderr_callback(struct Event *ev);
static int sendto_iauth(struct Client *cptr, const char *format, ...);
static int preregister_user(struct Client *cptr);
typedef int (*iauth_cmd_handler)(struct IAuth *iauth, struct Client *cli,
				 int parc, char **params);

/** Sends response \a r (from #ReportType) to client \a cptr. */
static void sendheader(struct Client *cptr, ReportType r)
{
  if (IsTLS(cptr))
  {
    sendrawto_one(cptr, "%.*s", HeaderMessages[r].length - 2,
      HeaderMessages[r].message);
    send_queued(cptr);
  }
  else
  {
   send(cli_fd(cptr), HeaderMessages[r].message, HeaderMessages[r].length, 0);
  }
}

int auth_set_webirc(struct AuthRequest *auth, const char *password, const char *username, const char *hostname, struct irc_in_addr *ip)
{
  struct Client *cptr;

  assert(auth != NULL);

  cptr = auth->client;

  if (!FlagHas(&auth->flags, AR_NEEDS_NICK) || !FlagHas(&auth->flags, AR_NEEDS_USER))
    return exit_client(cptr, cptr, &me, "WEBIRC must not be used after USER or NICK");

  if (IAuthHas(iauth, IAUTH_UNDERNET)) {
    sendto_iauth(cptr, "W %s %s %s %s", password, username, hostname, ircd_ntoa(ip));
	SetWebirc(cptr);
  }
  return 0;
}


/** Set username for user associated with \a auth.
 * @param[in] auth Client authorization request to work on.
 * @return Zero if client is kept, CPTR_KILLED if client rejected.
 */
static int auth_set_username(struct AuthRequest *auth)
{
  struct Client *sptr = auth->client;
  struct User   *user = cli_user(sptr);
  char *d;
  char *s;
  int   rlen = USERLEN;
  int   killreason;
  short character = 0;
  short pos = 0;
  short other = 0;
  short digits = 0;
  char  ch;
  char  last;

  if (FlagHas(&auth->flags, AR_IAUTH_USERNAME))
  {
      ircd_strncpy(cli_user(sptr)->username, cli_username(sptr), USERLEN);
  }
  else
  {
    /* Copy username from source to destination.  Since they may be the
     * same, and we may prefix with a '~', use a buffer character (ch)
     * to hold the next character to copy.
     */
    s = IsIdented(sptr) ? cli_username(sptr) : user->username;
    last = *s++;
    d = user->username;
    if (HasFlag(sptr, FLAG_DOID) && !IsIdented(sptr))
    {
      *d++ = '~';
      --rlen;
    }
    while (last && !IsCntrl(last) && rlen--)
    {
      ch = *s++;
      *d++ = IsUserChar(last) ? last : '_';
      last = (ch != '~') ? ch : '_';
    }
    *d = 0;
  }

  /* If username is empty or just ~, reject. */
  if ((user->username[0] == '\0')
      || ((user->username[0] == '~') && (user->username[1] == '\0')))
    return exit_client(sptr, sptr, &me, "USER: Bogus userid.");

  /* Have to set up "realusername" before doing the gline check below */
  ircd_strncpy(user->realusername, user->username, USERLEN);

  /* Check for K- or G-line. */
  killreason = find_kill(sptr, 1);
  if (killreason) {
    ServerStats->is_ref++;
    return exit_client(sptr, sptr, &me,
                       (killreason == -1 ? "K-lined" : "G-lined"));
  }

  if (!FlagHas(&auth->flags, AR_IAUTH_FUSERNAME))
  {
    /* Check for mixed case usernames, meaning probably hacked.  Jon2 3-94
     * Explanations of rules moved to where it is checked     Entrope 2-06
     */
    s = d = user->username + (user->username[0] == '~');
    for (last = '\0';
         (ch = *d++) != '\0';
         pos++, last = ch)
    {
      if (IsLower(ch) || IsUpper(ch))
      {
        character++;
      }
      else if (IsDigit(ch))
      {
        digits++;
      }
      else if (ch == '-' || ch == '_' || ch == '.')
      {
        other++;
        /* If -_. exist at start, consecutively, or more than twice, reject. */
        if (pos == 0 || last == '-' || last == '_' || last == '.' || other > 2)
          goto badid;
      }
      else /* All other punctuation is rejected. */
        goto badid;
    }

    /* Must have at least one letter. */
    if (!character)
      goto badid;
  }

  return 0;

badid:
  /* If we confirmed their username, and it is what they claimed,
   * accept it. */
  if (IsIdented(sptr) && !strcmp(cli_username(sptr), user->username))
    return 0;

  ServerStats->is_ref++;
  send_reply(sptr, SND_EXPLICIT | ERR_INVALIDUSERNAME,
             ":Your username is invalid.");
  send_reply(sptr, SND_EXPLICIT | ERR_INVALIDUSERNAME,
             ":Connect with your real username, in lowercase.");
  send_reply(sptr, SND_EXPLICIT | ERR_INVALIDUSERNAME,
             ":If your mail address were foo@bar.com, your username "
             "would be foo.");
  return exit_client(sptr, sptr, &me, "USER: Bad username");
}

/** Check whether an authorization request is complete.
 * This means that no flags from 0 to #AR_LAST_SCAN are set on \a auth.
 * If #AR_IAUTH_PENDING is set, optionally go into "hurry" state.  If
 * 0 through #AR_LAST_SCAN and #AR_IAUTH_PENDING are all clear,
 * destroy \a auth, clear the password, set the username, and register
 * the client.
 * @param[in] auth Authorization request to check.
 * @return Zero if client is kept, CPTR_KILLED if client rejected.
 */
static int check_auth_finished(struct AuthRequest *auth)
{
  enum AuthRequestFlag flag;
  int res;

  /* Check non-iauth registration blocking flags. */
  for (flag = 0; flag <= AR_LAST_SCAN; ++flag)
    if (FlagHas(&auth->flags, flag))
    {
      Debug((DEBUG_INFO, "Auth %p [%d] still has flag %d", auth,
             cli_fd(auth->client), flag));
      return 0;
    }

  /* If appropriate, do preliminary assignment to connection class. */
  if (IsUserPort(auth->client)
      && !FlagHas(&auth->flags, AR_IAUTH_HURRY)
      && preregister_user(auth->client))
    return CPTR_KILLED;

  /* If we have not done so, check client password.  Do this as soon
   * as possible so that iauth's challenge/response (which uses PASS
   * for responses) is not confused with the client's password.
   */
  if (IsUserPort(auth->client)
      && !FlagHas(&auth->flags, AR_PASSWORD_CHECKED))
  {
    struct ConfItem *aconf;

    aconf = cli_confs(auth->client)->value.aconf;
    if (aconf
        && !EmptyString(aconf->passwd)
        && strcmp(cli_passwd(auth->client), aconf->passwd))
    {
      ServerStats->is_ref++;
      send_reply(auth->client, ERR_PASSWDMISMATCH);
      return exit_client(auth->client, auth->client, &me, "Bad Password");
    }
 
      /* Check TLS fingerprint. */
      if ((res == 0) && aconf && !EmptyString(aconf->tls_fingerprint)
        && !ircd_tls_fingerprint_matches(auth->client, aconf->tls_fingerprint))
      {
        ++ServerStats->is_ref;
        send_reply(auth->client, ERR_TLSCLIFINGERPRINT);
        return exit_client(auth->client, auth->client, &me, "Bad TLS fingerprint");
      }  
	  FlagSet(&auth->flags, AR_PASSWORD_CHECKED);
  }


	  
  /* Check if iauth is done. */
  if (FlagHas(&auth->flags, AR_IAUTH_PENDING))
  {
    /* Switch auth request to hurry-up state. */
    if (!FlagHas(&auth->flags, AR_IAUTH_HURRY))
    {
      /* Set "hurry" flag in auth request. */
      FlagSet(&auth->flags, AR_IAUTH_HURRY);

      /* If iauth wants it, send notification. */
      if (IAuthHas(iauth, IAUTH_UNDERNET))
        sendto_iauth(auth->client, "H %s", get_client_class(auth->client));

      /* If iauth wants it, give client more time. */
      if (IAuthHas(iauth, IAUTH_EXTRAWAIT))
        cli_firsttime(auth->client) = CurrentTime;
    }

    Debug((DEBUG_INFO, "Auth %p [%d] still has flag %d", auth,
           cli_fd(auth->client), AR_IAUTH_PENDING));
    return 0;
  }
  else
    FlagSet(&auth->flags, AR_IAUTH_HURRY);

  if (IsUserPort(auth->client))
  {
    memset(cli_passwd(auth->client), 0, sizeof(cli_passwd(auth->client)));
    res = auth_set_username(auth);
    if (res == 0)
      res = register_user(auth->client, auth->client);
  }
  else
    res = 0;
  if (res == 0)
    destroy_auth_request(auth);
  return res;
}

/** Verify that a hostname is valid, i.e., only contains characters
 * valid for a hostname and that a hostname is not too long.
 * @param host Hostname to check.
 * @param maxlen Maximum length of hostname, not including NUL terminator.
 * @return Non-zero if the hostname is valid.
 */
static int
auth_verify_hostname(const char *host, int maxlen)
{
  int i;

  /* Walk through the host name */
  for (i = 0; host[i]; i++)
    /* If it's not a hostname character or if it's too long, return false */
    if (!IsHostChar(host[i]) || i >= maxlen)
      return 0;

  return 1; /* it's a valid hostname */
}

/** Assign a client to a connection class.
 * @param[in] cptr Client to assign to a class.
 * @return Zero if client is kept, CPTR_KILLED if rejected.
 */
static int preregister_user(struct Client *cptr)
{
  static time_t last_too_many1;
  static time_t last_too_many2;

  ircd_strncpy(cli_user(cptr)->host, cli_sockhost(cptr), HOSTLEN);
  ircd_strncpy(cli_user(cptr)->realhost, cli_sockhost(cptr), HOSTLEN);

  switch (conf_check_client(cptr))
  {
  case ACR_OK:
    break;
  case ACR_NO_AUTHORIZATION:
    sendto_opmask_butone(0, SNO_UNAUTH, "Unauthorized connection from %s.",
                         get_client_name(cptr, HIDE_IP));
    ++ServerStats->is_ref;
    return exit_client(cptr, cptr, &me,
                       "No Authorization - use another server");
  case ACR_TOO_MANY_IN_CLASS:
    sendto_opmask_butone_ratelimited(0, SNO_TOOMANY, &last_too_many1,
                                     "Too many connections in class %s for %s.",
                                     get_client_class(cptr),
                                     get_client_name(cptr, SHOW_IP));
    ++ServerStats->is_ref;
    return exit_client(cptr, cptr, &me,
                       "Sorry, your connection class is full - try "
                       "again later or try another server");
  case ACR_TOO_MANY_FROM_IP:
    sendto_opmask_butone_ratelimited(0, SNO_TOOMANY, &last_too_many2,
                                     "Too many connections from same IP for %s.",
                                     get_client_name(cptr, SHOW_IP));
    ++ServerStats->is_ref;
    return exit_client(cptr, cptr, &me,
                       "Too many connections from your host");
  case ACR_ALREADY_AUTHORIZED:
    /* Can this ever happen? */
  case ACR_BAD_SOCKET:
    ++ServerStats->is_ref;
    IPcheck_connect_fail(cptr);
    return exit_client(cptr, cptr, &me, "Unknown error -- Try again");
  }
  return 0;
}

/** Send the ident server a query giving "theirport , ourport". The
 * write is only attempted *once* so it is deemed to be a fail if the
 * entire write doesn't write all the data given.  This shouldn't be a
 * problem since the socket should have a write buffer far greater
 * than this message to store it in should problems arise. -avalon
 * @param[in] auth The request to send.
 */
static void send_auth_query(struct AuthRequest* auth)
{
  char               authbuf[32];
  unsigned int       count;

  assert(0 != auth);

  ircd_snprintf(0, authbuf, sizeof(authbuf), "%hu , %hu\r\n",
                auth->port, auth->local.port);

  if (IO_SUCCESS != os_send_nonb(s_fd(&auth->socket), authbuf, strlen(authbuf), &count)) {
    close(s_fd(&auth->socket));
    socket_del(&auth->socket);
    s_fd(&auth->socket) = -1;
    ++ServerStats->is_abad;
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_FAIL_ID);
    FlagClr(&auth->flags, AR_AUTH_PENDING);
    check_auth_finished(auth);
  }
}

/** Enum used to index ident reply fields in a human-readable way. */
enum IdentReplyFields {
  IDENT_PORT_NUMBERS,
  IDENT_REPLY_TYPE,
  IDENT_OS_TYPE,
  IDENT_INFO,
  USERID_TOKEN_COUNT
};

/** Parse an ident reply line and extract the userid from it.
 * @param[in] reply The ident reply line.
 * @return The userid, or NULL on parse failure.
 */
static char* check_ident_reply(char* reply)
{
  char* token;
  char* end;
  char* vector[USERID_TOKEN_COUNT];
  int count = token_vector(reply, ':', vector, USERID_TOKEN_COUNT);

  if (USERID_TOKEN_COUNT != count)
    return 0;
  /*
   * second token is the reply type
   */
  token = vector[IDENT_REPLY_TYPE];
  if (EmptyString(token))
    return 0;

  while (IsSpace(*token))
    ++token;

  if (0 != strncmp(token, "USERID", 6))
    return 0;

  /*
   * third token is the os type
   */
  token = vector[IDENT_OS_TYPE];
  if (EmptyString(token))
    return 0;
  while (IsSpace(*token))
   ++token;

  /*
   * Unless "OTHER" is specified as the operating system
   * type, the server is expected to return the "normal"
   * user identification of the owner of this connection.
   * "Normal" in this context may be taken to mean a string
   * of characters which uniquely identifies the connection
   * owner such as a user identifier assigned by the system
   * administrator and used by such user as a mail
   * identifier, or as the "user" part of a user/password
   * pair used to gain access to system resources.  When an
   * operating system is specified (e.g., anything but
   * "OTHER"), the user identifier is expected to be in a
   * more or less immediately useful form - e.g., something
   * that could be used as an argument to "finger" or as a
   * mail address.
   */
  if (0 == strncmp(token, "OTHER", 5))
    return 0;
  /*
   * fourth token is the username
   */
  token = vector[IDENT_INFO];
  if (EmptyString(token))
    return 0;
  while (IsSpace(*token))
    ++token;
  /*
   * look for the end of the username, terminators are '\0, @, <SPACE>, :'
   */
  for (end = token; *end; ++end) {
    if (IsSpace(*end) || '@' == *end || ':' == *end)
      break;
  }
  *end = '\0';
  return token;
}

/** Read the reply (if any) from the ident server we connected to.  We
 * only give it one shot, if the reply isn't good the first time fail
 * the authentication entirely. --Bleep
 * @param[in] auth The request to read.
 */
static void read_auth_reply(struct AuthRequest* auth)
{
  char*        username = 0;
  unsigned int len;
  /*
   * rfc1453 sez we MUST accept 512 bytes
   */
  char   buf[BUFSIZE + 1];

  assert(0 != auth);
  assert(0 != auth->client);
  assert(auth == cli_auth(auth->client));

  if (IO_SUCCESS == os_recv_nonb(s_fd(&auth->socket), buf, BUFSIZE, &len)) {
    buf[len] = '\0';
    Debug((DEBUG_INFO, "Auth %p [%d] reply: %s", auth, cli_fd(auth->client), buf));
    username = check_ident_reply(buf);
    Debug((DEBUG_INFO, "Username: %s", username));
  }

  Debug((DEBUG_INFO, "Deleting auth [%d] socket %p", auth, cli_fd(auth->client)));
  close(s_fd(&auth->socket));
  socket_del(&auth->socket);
  s_fd(&auth->socket) = -1;

  if (EmptyString(username)) {
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_FAIL_ID);
    ++ServerStats->is_abad;
  } else {
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_FIN_ID);
    ++ServerStats->is_asuc;
    if (!FlagHas(&auth->flags, AR_IAUTH_USERNAME)) {
      ircd_strncpy(cli_username(auth->client), username, USERLEN);
      SetGotId(auth->client);
    }
    if (IAuthHas(iauth, IAUTH_UNDERNET))
      sendto_iauth(auth->client, "u %s", username);
  }

  FlagClr(&auth->flags, AR_AUTH_PENDING);
  check_auth_finished(auth);
}

/** Handle socket I/O activity.
 * @param[in] ev A socket event whos associated data is the active
 *   struct AuthRequest.
 */
static void auth_sock_callback(struct Event* ev)
{
  struct AuthRequest* auth;

  assert(0 != ev_socket(ev));
  assert(0 != s_data(ev_socket(ev)));

  auth = (struct AuthRequest*) s_data(ev_socket(ev));

  switch (ev_type(ev)) {
  case ET_DESTROY: /* being destroyed */
    break;

  case ET_CONNECT: /* socket connection completed */
    Debug((DEBUG_INFO, "Connection completed for auth %p [%d]; sending query",
           auth, cli_fd(auth->client)));
    socket_state(&auth->socket, SS_CONNECTED);
    send_auth_query(auth);
    break;

  case ET_READ: /* socket is readable */
  case ET_EOF: /* end of file on socket */
  case ET_ERROR: /* error on socket */
    Debug((DEBUG_INFO, "Auth socket %p [%p] readable", auth, ev_socket(ev)));
    read_auth_reply(auth);
    break;

  default:
    assert(0 && "Unrecognized event in auth_socket_callback().");
    break;
  }
}

/** Stop an auth request completely.
 * @param[in] auth The struct AuthRequest to cancel.
 */
void destroy_auth_request(struct AuthRequest* auth)
{
  Debug((DEBUG_INFO, "Deleting auth request for %p", auth->client));

  if (FlagHas(&auth->flags, AR_DNS_PENDING)) {
    delete_resolver_queries(auth);
  }

  if (-1 < s_fd(&auth->socket)) {
    close(s_fd(&auth->socket));
    socket_del(&auth->socket);
    s_fd(&auth->socket) = -1;
  }

  if (t_active(&auth->timeout))
    timer_del(&auth->timeout);

  cli_auth(auth->client) = NULL;
  auth->next = auth_freelist;
  auth_freelist = auth;
}

/** Handle SASL
*/
int auth_set_sasl(struct AuthRequest *auth, const char *crypt)
{
  assert(auth != NULL);
  if(CapHas(cli_active(auth->client), CAP_SASL)) {
	 sendto_iauth(auth->client, "Y %s", crypt);
  }
  return 0;
}

/** Handle a 'ping' (authorization) timeout for a client.
 * @param[in] cptr The client whose session authorization has timed out.
 * @return Zero if client is kept, CPTR_KILLED if client rejected.
 */
int auth_ping_timeout(struct Client *cptr)
{
  struct AuthRequest *auth;
  enum AuthRequestFlag flag;

  auth = cli_auth(cptr);

  /* Check whether the auth request is gone (more likely, it never
   * existed, as in an outbound server connection). */
  if (!auth)
      return exit_client_msg(cptr, cptr, &me, "Registration Timeout");

  /* Check for a user-controlled timeout. */
  for (flag = 0; flag <= AR_LAST_SCAN; ++flag) {
    if (FlagHas(&auth->flags, flag)) {
      /* Display message if they have sent a NICK and a USER but no
       * nospoof PONG.
       */
      if (*(cli_name(cptr)) && cli_user(cptr) && *(cli_user(cptr))->username) {
        send_reply(cptr, SND_EXPLICIT | ERR_BADPING,
                   ":Your client may not be compatible with this server.");
        send_reply(cptr, SND_EXPLICIT | ERR_BADPING,
                   ":Compatible clients are available at %s",
                   feature_str(FEAT_URL_CLIENTS));
      }
      return exit_client_msg(cptr, cptr, &me, "Registration Timeout");
    }
  }

  /* Check for iauth timeout. */
  if (FlagHas(&auth->flags, AR_IAUTH_PENDING)) {
    sendto_iauth(cptr, "T");
    if (IAuthHas(iauth, IAUTH_REQUIRED)) {
      sendheader(cptr, REPORT_FAIL_IAUTH);
      return exit_client_msg(cptr, cptr, &me, "Authorization Timeout");
    }
    FlagClr(&auth->flags, AR_IAUTH_PENDING);
    return check_auth_finished(auth);
  }

  assert(0 && "Unexpectedly reached end of auth_ping_timeout()");
  return 0;
}

/** Timeout a given auth request.
 * @param[in] ev A timer event whose associated data is the expired
 *   struct AuthRequest.
 */
static void auth_timeout_callback(struct Event* ev)
{
  struct AuthRequest* auth;

  assert(0 != ev_timer(ev));
  assert(0 != t_data(ev_timer(ev)));

  auth = (struct AuthRequest*) t_data(ev_timer(ev));

  if (ev_type(ev) == ET_EXPIRE) {
    /* Report the timeout in the log. */
    log_write(LS_RESOLVER, L_INFO, 0, "Registration timeout %s",
              get_client_name(auth->client, HIDE_IP));

    /* Notify client if ident lookup failed. */
    if (FlagHas(&auth->flags, AR_AUTH_PENDING)) {
      FlagClr(&auth->flags, AR_AUTH_PENDING);
      if (IsUserPort(auth->client))
        sendheader(auth->client, REPORT_FAIL_ID);
    }

    /* Likewise if dns lookup failed. */
    if (FlagHas(&auth->flags, AR_DNS_PENDING)) {
      FlagClr(&auth->flags, AR_DNS_PENDING);
      delete_resolver_queries(auth);
      if (IsUserPort(auth->client))
        sendheader(auth->client, REPORT_FAIL_DNS);
    }

    /* Try to register the client. */
    check_auth_finished(auth);
  }
}

/** Handle a complete DNS lookup.  Send the client on it's way to a
 * connection completion, regardless of success or failure -- unless
 * there was a mismatch and KILL_IPMISMATCH is set.
 * @param[in] vptr The pending struct AuthRequest.
 * @param[in] addr IP address being resolved.
 * @param[in] h_name Resolved name, or NULL if lookup failed.
 */
static void auth_dns_callback(void* vptr, const struct irc_in_addr *addr, const char *h_name)
{
  struct AuthRequest* auth = (struct AuthRequest*) vptr;
  assert(0 != auth);

  FlagClr(&auth->flags, AR_DNS_PENDING);
  if (!addr) {
    /* DNS entry was missing for the IP. */
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_FAIL_DNS);
    sendto_iauth(auth->client, "d");
  } else if (!irc_in_addr_valid(addr)
             || (irc_in_addr_cmp(&cli_ip(auth->client), addr)
                 && irc_in_addr_cmp(&auth->original, addr))) {
    /* IP for hostname did not match client's IP. */
    sendto_opmask_butone(0, SNO_IPMISMATCH, "IP# Mismatch: %s != %s[%s]",
                         cli_sock_ip(auth->client), h_name,
                         ircd_ntoa(addr));
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_IP_MISMATCH);
    if (feature_bool(FEAT_KILL_IPMISMATCH)) {
      exit_client(auth->client, auth->client, &me, "IP mismatch");
      return;
    }
  } else if (!auth_verify_hostname(h_name, HOSTLEN)) {
    /* Hostname did not look valid. */
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_INVAL_DNS);
    sendto_iauth(auth->client, "d");
  } else {
    /* Hostname and mappings checked out. */
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_FIN_DNS);
    ircd_strncpy(cli_sockhost(auth->client), h_name, HOSTLEN);
    sendto_iauth(auth->client, "N %s", h_name);
  }
  check_auth_finished(auth);
}

/** Flag the client to show an attempt to contact the ident server on
 * the client's host.  Should the connect or any later phase of the
 * identifying process fail, it is aborted and the user is given a
 * username of "unknown".
 * @param[in] auth The request for which to start the ident lookup.
 */
static void start_auth_query(struct AuthRequest* auth)
{
  struct irc_sockaddr remote_addr;
  struct irc_sockaddr local_addr;
  int                 fd;
  IOResult            result;

  assert(0 != auth);
  assert(0 != auth->client);

  /*
   * get the local address of the client and bind to that to
   * make the auth request.  This used to be done only for
   * ifdef VIRTUAL_HOST, but needs to be done for all clients
   * since the ident request must originate from that same address--
   * and machines with multiple IP addresses are common now
   */
  memcpy(&local_addr, &auth->local, sizeof(local_addr));
  local_addr.port = 0;
  memcpy(&remote_addr.addr, &cli_ip(auth->client), sizeof(remote_addr.addr));
  remote_addr.port = 113;
  fd = os_socket(&local_addr, SOCK_STREAM, "auth query", 0);
  if (fd < 0) {
    ++ServerStats->is_abad;
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_FAIL_ID);
    return;
  }
  if (IsUserPort(auth->client))
    sendheader(auth->client, REPORT_DO_ID);

  if ((result = os_connect_nonb(fd, &remote_addr)) == IO_FAILURE ||
      !socket_add(&auth->socket, auth_sock_callback, (void*) auth,
                  result == IO_SUCCESS ? SS_CONNECTED : SS_CONNECTING,
                  SOCK_EVENT_READABLE, fd)) {
    ++ServerStats->is_abad;
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_FAIL_ID);
    close(fd);
    return;
  }

  FlagSet(&auth->flags, AR_AUTH_PENDING);
  if (result == IO_SUCCESS)
    send_auth_query(auth);
}

/** Initiate DNS lookup for a client.
 * @param[in] auth The auth request for which to start the DNS lookup.
 */
static void start_dns_query(struct AuthRequest *auth)
{
  if (feature_bool(FEAT_NODNS)) {
    sendto_iauth(auth->client, "d");
    return;
  }

  if (irc_in_addr_is_loopback(&cli_ip(auth->client))) {
    strcpy(cli_sockhost(auth->client), cli_name(&me));
    sendto_iauth(auth->client, "N %s", cli_sockhost(auth->client));
    return;
  }

  if (IsUserPort(auth->client))
    sendheader(auth->client, REPORT_DO_DNS);

  FlagSet(&auth->flags, AR_DNS_PENDING);
  gethost_byaddr(&cli_ip(auth->client), auth_dns_callback, auth);
}

/** Initiate IAuth check for a client.
 * @param[in] auth The auth request for which to star the IAuth check.
 */
static void start_iauth_query(struct AuthRequest *auth)
{
  FlagSet(&auth->flags, AR_IAUTH_PENDING);
  if (!sendto_iauth(auth->client, "C %s %hu %s %hu",
                    cli_sock_ip(auth->client), auth->port,
                    ircd_ntoa(&auth->local.addr), auth->local.port))
    FlagClr(&auth->flags, AR_IAUTH_PENDING);
}

/** Starts auth (identd) and dns queries for a client.
 * @param[in] client The client for which to start queries.
 */
void start_auth(struct Client* client)
{
  struct irc_sockaddr remote;
  struct AuthRequest* auth;

  assert(0 != client);
  Debug((DEBUG_INFO, "Beginning auth request on client %p", client));

  /* Register with event handlers. */
  cli_lasttime(client) = CurrentTime;
  cli_since(client) = CurrentTime;
  if (cli_fd(client) > HighestFd)
    HighestFd = cli_fd(client);
  LocalClientArray[cli_fd(client)] = client;
  socket_events(&(cli_socket(client)), SOCK_ACTION_SET | SOCK_EVENT_READABLE);

  /* Allocate the AuthRequest. */
  auth = auth_freelist;
  if (auth)
      auth_freelist = auth->next;
  else
      auth = MyMalloc(sizeof(*auth));
  assert(0 != auth);
  memset(auth, 0, sizeof(*auth));
  auth->client = client;
  cli_auth(client) = auth;
  s_fd(&auth->socket) = -1;
  timer_add(timer_init(&auth->timeout), auth_timeout_callback, (void*) auth,
            TT_RELATIVE, feature_int(FEAT_AUTH_TIMEOUT));

  /* Try to get socket endpoint addresses. */
  if (!os_get_sockname(cli_fd(client), &auth->local)
      || !os_get_peername(cli_fd(client), &remote)) {
    ++ServerStats->is_abad;
    if (IsUserPort(auth->client))
      sendheader(auth->client, REPORT_FAIL_ID);
    exit_client(auth->client, auth->client, &me, "Socket local/peer lookup failed");
    return;
  }
  auth->port = remote.port;

  /* Try to start DNS lookup. */
  start_dns_query(auth);

  /* Try to start ident lookup. */
  start_auth_query(auth);

  /* Set required client inputs for users. */
  if (IsUserPort(client)) {
    cli_user(client) = make_user(client);
    cli_user(client)->server = &me;
    FlagSet(&auth->flags, AR_NEEDS_USER);
    FlagSet(&auth->flags, AR_NEEDS_NICK);

    /* Try to start iauth lookup. */
    start_iauth_query(auth);
  }

  /* Add client to GlobalClientList. */
  add_client_to_list(client);

  /* Check which auth events remain pending. */
  check_auth_finished(auth);
}

/** Mark that a user has PONGed while unregistered.
 * @param[in] auth Authorization request for client.
 * @param[in] cookie PONG cookie value sent by client.
 * @return Zero if client should be kept, CPTR_KILLED if rejected.
 */
int auth_set_pong(struct AuthRequest *auth, unsigned int cookie)
{
  assert(auth != NULL);
  if (!FlagHas(&auth->flags, AR_NEEDS_PONG))
    return 0;
  if (cookie != auth->cookie)
  {
    send_reply(auth->client, SND_EXPLICIT | ERR_BADPING,
               ":To connect, type /QUOTE PONG %u", auth->cookie);
    return 0;
  }
  cli_lasttime(auth->client) = CurrentTime;
  FlagClr(&auth->flags, AR_NEEDS_PONG);
  return check_auth_finished(auth);
}

/** Record a user's claimed username and userinfo.
 * @param[in] auth Authorization request for client.
 * @param[in] username Client's asserted username.
 * @param[in] hostname Third argument of USER command (client's
 *   hostname, per RFC 1459).
 * @param[in] servername Fourth argument of USER command (server's
 *   name, per RFC 1459).
 * @param[in] userinfo Client's asserted self-description.
 * @return Zero if client should be kept, CPTR_KILLED if rejected.
 */
int auth_set_user(struct AuthRequest *auth, const char *username, const char *hostname, const char *servername, const char *userinfo)
{
  struct Client *cptr;

  assert(auth != NULL);
  if (FlagHas(&auth->flags, AR_IAUTH_HURRY))
    return 0;
  FlagClr(&auth->flags, AR_NEEDS_USER);
  cptr = auth->client;
  ircd_strncpy(cli_info(cptr), userinfo, REALLEN);
  ircd_strncpy(cli_user(cptr)->username, username, USERLEN);
  ircd_strncpy(cli_user(cptr)->host, cli_sockhost(cptr), HOSTLEN);
  if (IAuthHas(iauth, IAUTH_UNDERNET))
    sendto_iauth(cptr, "U %s %s %s :%s", username, hostname, servername, userinfo);
  else if (IAuthHas(iauth, IAUTH_ADDLINFO))
    sendto_iauth(cptr, "U %s", username);
  return check_auth_finished(auth);
}

/** Handle authorization-related aspects of initial nickname selection.
 * This is called after verifying that the nickname is available.
 * @param[in] auth Authorization request for client.
 * @param[in] nickname Client's requested nickname.
 * @return Zero if client should be kept, CPTR_KILLED if rejected.
 */
int auth_set_nick(struct AuthRequest *auth, const char *nickname)
{
  assert(auth != NULL);
  FlagClr(&auth->flags, AR_NEEDS_NICK);
  /*
   * If the client hasn't gotten a cookie-ping yet,
   * choose a cookie and send it. -record!jegelhof@cloud9.net
   */
  if (!auth->cookie) {
    do {
      auth->cookie = ircrandom();
    } while (!auth->cookie);
    sendrawto_one(auth->client, "PING :%u", auth->cookie);
    FlagSet(&auth->flags, AR_NEEDS_PONG);
  }
  if (IAuthHas(iauth, IAUTH_UNDERNET))
    sendto_iauth(auth->client, "n %s", nickname);
  return check_auth_finished(auth);
}

/** Record a user's password.
 * @param[in] auth Authorization request for client.
 * @param[in] password Client's password.
 * @return Zero if client should be kept, CPTR_KILLED if rejected.
 */
int auth_set_password(struct AuthRequest *auth, const char *password)
{
  assert(auth != NULL);
  if (IAuthHas(iauth, IAUTH_ADDLINFO))
    sendto_iauth(auth->client, "P :%s", password);
  return 0;
}

/** Send exit notification for \a cptr to iauth.
 * @param[in] cptr Client who is exiting.
 */
void auth_send_exit(struct Client *cptr)
{
  sendto_iauth(cptr, "D");
}

/** Mark that a user has started capabilities negotiation.
 * This blocks authorization until auth_cap_done() is called.
 * @param[in] auth Authorization request for client.
 * @return Zero if client should be kept, CPTR_KILLED if rejected.
 */
int auth_cap_start(struct AuthRequest *auth)
{
  assert(auth != NULL);
  FlagSet(&auth->flags, AR_CAP_PENDING);
  return 0;
}

/** Mark that a user has completed capabilities negotiation.
 * This unblocks authorization if auth_cap_start() was called.
 * @param[in] auth Authorization request for client.
 * @return Zero if client should be kept, CPTR_KILLED if rejected.
 */
int auth_cap_done(struct AuthRequest *auth)
{
  assert(auth != NULL);
  FlagClr(&auth->flags, AR_CAP_PENDING);
  return check_auth_finished(auth);
}

/** Attempt to spawn the process for an IAuth instance.
 * @param[in] iauth IAuth descriptor.
 * @param[in] automatic If non-zero, apply sanity checks against
 *   excessive automatic restarts.
 * @return 0 on success, non-zero on failure.
 */
int iauth_do_spawn(struct IAuth *iauth, int automatic)
{
  pid_t cpid;
  int s_io[2];
  int s_err[2];
  int res;

  if (automatic && CurrentTime - iauth->started < 5)
  {
    sendto_opmask_butone(NULL, SNO_AUTH, "IAuth crashed fast, leaving it dead.");
    return -1;
  }

  /* Record time we tried to spawn the iauth process. */
  iauth->started = CurrentTime;

  /* Attempt to allocate a pair of sockets. */
  res = os_socketpair(s_io);
  if (res)
    return errno;

  /* Mark the parent's side of the pair (element 0) as non-blocking. */
  res = os_set_nonblocking(s_io[0]);
  if (!res) {
    res = errno;
    close(s_io[1]);
    close(s_io[0]);
    return res;
  }

  /* Initialize the socket structure to talk to the child. */
  res = socket_add(i_socket(iauth), iauth_sock_callback, iauth,
                   SS_CONNECTED, SOCK_EVENT_READABLE, s_io[0]);
  if (!res) {
    res = errno;
    close(s_io[1]);
    close(s_io[0]);
    return res;
  }

  /* Allocate another pair for stderr. */
  res = os_socketpair(s_err);
  if (res) {
    res = errno;
    socket_del(i_socket(iauth));
    close(s_io[1]);
    close(s_io[0]);
    return res;
  }

  /* Mark parent side of this pair non-blocking, too. */
  res = os_set_nonblocking(s_err[0]);
  if (!res) {
    res = errno;
    close(s_err[1]);
    close(s_err[0]);
    socket_del(i_socket(iauth));
    close(s_io[1]);
    close(s_io[0]);
    return res;
  }

  /* And set up i_stderr(iauth). */
  res = socket_add(i_stderr(iauth), iauth_stderr_callback, iauth,
                   SS_CONNECTED, SOCK_EVENT_READABLE, s_err[0]);
  if (!res) {
    res = errno;
    close(s_err[1]);
    close(s_err[0]);
    socket_del(i_socket(iauth));
    close(s_io[1]);
    close(s_io[0]);
    return res;
  }

  /* Attempt to fork a child process. */
  cpid = fork();
  if (cpid < 0) {
    /* Error forking the child, still in parent. */
    res = errno;
    socket_del(i_stderr(iauth));
    close(s_err[1]);
    close(s_err[0]);
    socket_del(i_socket(iauth));
    close(s_io[1]);
    close(s_io[0]);
    return res;
  }

  if (cpid > 0) {
    /* We are the parent process.  Close the child's sockets. */
    close(s_io[1]);
    close(s_err[1]);
    /* Send our server name (supposedly for proxy checking purposes)
     * and maximum number of connections (for allocation hints).
     * Need to use conf_get_local() since &me may not be fully
     * initialized the first time we run.
     */
    sendto_iauth(NULL, "M %s %d", conf_get_local()->name, MAXCONNECTIONS);
    /* Indicate success (until the child dies). */
    return 0;
  }

  /* We are the child process.
   * Duplicate our end of the socket to stdin, stdout and stderr.
   * Then close all the higher-numbered FDs and exec the process.
   */
  if (dup2(s_io[1], 0) == 0
      && dup2(s_io[1], 1) == 1
      && dup2(s_err[1], 2) == 2) {
    close_connections(0);
    execvp(iauth->i_argv[0], iauth->i_argv);
  }

  /* If we got here, something was seriously wrong. */
  exit(EXIT_FAILURE);
}

/** See if an %IAuth program must be spawned.
 * If a process is already running with the specified options, keep it.
 * Otherwise spawn a new child process to perform the %IAuth function.
 * @param[in] argc Number of parameters to use when starting process.
 * @param[in] argv Array of parameters to start process.
 * @return 0 on failure, 1 on new process, 2 on reuse of existing process.
 */
int auth_spawn(int argc, char *argv[])
{
  int ii;

  if (iauth) {
    int same = 1;

    /* Check that incoming arguments all match pre-existing arguments. */
    for (ii = 0; same && (ii < argc); ++ii) {
      if (NULL == iauth->i_argv[ii]
          || 0 != strcmp(iauth->i_argv[ii], argv[ii]))
        same = 0;
    }
    /* Check that we have no more pre-existing arguments. */
    if (iauth->i_argv[ii])
      same = 0;
    /* If they are the same and still connected, clear the "closing" flag and exit.*/
    if (same && i_GetConnected(iauth)) {
      IAuthClr(iauth, IAUTH_CLOSING);
      return 2;
    }
    /* Deallocate old argv elements. */
    for (ii = 0; iauth->i_argv[ii]; ++ii)
      MyFree(iauth->i_argv[ii]);
    MyFree(iauth->i_argv);
  }

  /* Need to initialize a new connection. */
  iauth = MyCalloc(1, sizeof(*iauth));
  msgq_init(i_sendQ(iauth));
  /* Populate iauth's argv array. */
  iauth->i_argv = MyCalloc(argc + 1, sizeof(iauth->i_argv[0]));
  for (ii = 0; ii < argc; ++ii)
    DupString(iauth->i_argv[ii], argv[ii]);
  iauth->i_argv[ii] = NULL;
  /* Try to spawn it, and handle the results. */
  if (iauth_do_spawn(iauth, 0))
    return 0;
  IAuthClr(iauth, IAUTH_CLOSING);
  return 1;
}

/** Mark all %IAuth connections as closing. */
void auth_mark_closing(void)
{
  if (iauth)
    IAuthSet(iauth, IAUTH_CLOSING);
}

/** Complete disconnection of an %IAuth connection.
 * @param[in] iauth %Connection to fully close.
 */
static void iauth_disconnect(struct IAuth *iauth)
{
  if (!i_GetConnected(iauth))
    return;

  /* Close main socket. */
  close(s_fd(i_socket(iauth)));
  socket_del(i_socket(iauth));
  s_fd(i_socket(iauth)) = -1;

  /* Close error socket. */
  close(s_fd(i_stderr(iauth)));
  socket_del(i_stderr(iauth));
  s_fd(i_stderr(iauth)) = -1;
}

/** Close all %IAuth connections marked as closing. */
void auth_close_unused(void)
{
  if (IAuthHas(iauth, IAUTH_CLOSING)) {
    int ii;
    iauth_disconnect(iauth);
    if (iauth->i_argv) {
      for (ii = 0; iauth->i_argv[ii]; ++ii)
        MyFree(iauth->i_argv[ii]);
      MyFree(iauth->i_argv);
    }
    MyFree(iauth);
  }
}

/** Send queued output to \a iauth.
 * @param[in] iauth Writable connection with queued data.
 */
static void iauth_write(struct IAuth *iauth)
{
  unsigned int bytes_tried, bytes_sent;
  IOResult iores;

  if (IAuthHas(iauth, IAUTH_BLOCKED))
    return;
  while (MsgQLength(i_sendQ(iauth)) > 0) {
    iores = os_sendv_nonb(s_fd(i_socket(iauth)), i_sendQ(iauth), &bytes_tried, &bytes_sent);
    switch (iores) {
    case IO_SUCCESS:
      msgq_delete(i_sendQ(iauth), bytes_sent);
      iauth->i_sendB += bytes_sent;
      if (bytes_tried == bytes_sent)
        break;
      /* If bytes_sent < bytes_tried, fall through to IO_BLOCKED. */
    case IO_BLOCKED:
      IAuthSet(iauth, IAUTH_BLOCKED);
      socket_events(i_socket(iauth), SOCK_ACTION_ADD | SOCK_EVENT_WRITABLE);
      return;
    case IO_FAILURE:
      iauth_disconnect(iauth);
      return;
    }
  }
  /* We were able to flush all events, so remove notification. */
  socket_events(i_socket(iauth), SOCK_ACTION_DEL | SOCK_EVENT_WRITABLE);
}

/** Send a message to iauth.
 * @param[in] cptr Optional client context for message.
 * @param[in] format Format string for message.
 * @return Non-zero on successful send or buffering, zero on failure.
 */
static int sendto_iauth(struct Client *cptr, const char *format, ...)
{
  struct VarData vd;
  struct MsgBuf *mb;

  /* Do not send requests when we have no iauth. */
  if (!i_GetConnected(iauth))
    return 0;
  /* Do not send for clients in the NORMAL state. */
  if (cptr
      && (format[0] != 'D')
      && (!cli_auth(cptr) || !FlagHas(&cli_auth(cptr)->flags, AR_IAUTH_PENDING)))
    return 0;

  /* Build the message buffer. */
  vd.vd_format = format;
  va_start(vd.vd_args, format);
  mb = msgq_make(NULL, "%d %v", cptr ? cli_fd(cptr) : -1, &vd);
  va_end(vd.vd_args);

  /* Tack it onto the iauth sendq and try to write it. */
  ++iauth->i_sendM;
  msgq_add(i_sendQ(iauth), mb, 0);
  msgq_clean(mb);
  iauth_write(iauth);
  return 1;
}

/** Send text to interested operators (SNO_AUTH server notice).
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params Text to send.
 * @return Zero.
 */
static int iauth_cmd_snotice(struct IAuth *iauth, struct Client *cli,
			     int parc, char **params)
{
  sendto_opmask_butone(NULL, SNO_AUTH, "%s", params[0]);
  return 0;
}

/** Set the debug level for the session.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params String starting with an integer.
 * @return Zero.
 */
static int iauth_cmd_debuglevel(struct IAuth *iauth, struct Client *cli,
				int parc, char **params)
{
  int new_level;

  new_level = parc > 0 ? atoi(params[0]) : 0;
  if (i_debug(iauth) > 0 || new_level > 0) {
    /* The "ia_dbg" name is borrowed from (IRCnet) ircd. */
    sendto_opmask_butone(NULL, SNO_AUTH, "ia_dbg = %d", new_level);
  }
  i_debug(iauth) = new_level;
  return 0;
}

/** Set policy options for the session.
 * Old policy is forgotten, and any of the following characters in \a
 * params enable the corresponding policy:
 * \li A IAUTH_ADDLINFO
 * \li R IAUTH_REQUIRED
 * \li T IAUTH_TIMEOUT
 * \li W IAUTH_EXTRAWAIT
 * \li U IAUTH_UNDERNET
 *
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params Zero or more policy options.
 * @return Zero.
 */
static int iauth_cmd_policy(struct IAuth *iauth, struct Client *cli,
			    int parc, char **params)
{
  enum IAuthFlag flag;
  char *p;

  /* Erase old policy first. */
  for (flag = IAUTH_FIRST_OPTION; flag < IAUTH_LAST_FLAG; ++flag)
    IAuthClr(iauth, flag);

  if (parc > 0) /* only try to parse if we were given a policy string */
    /* Parse new policy set. */
    for (p = params[0]; *p; p++) switch (*p) {
    case 'A': IAuthSet(iauth, IAUTH_ADDLINFO); break;
    case 'R': IAuthSet(iauth, IAUTH_REQUIRED); break;
    case 'T': IAuthSet(iauth, IAUTH_TIMEOUT); break;
    case 'W': IAuthSet(iauth, IAUTH_EXTRAWAIT); break;
    case 'U': IAuthSet(iauth, IAUTH_UNDERNET); break;
    }

  /* Optionally notify operators. */
  if (i_debug(iauth) > 0)
    sendto_opmask_butone(NULL, SNO_AUTH, "iauth options: %s", params[0]);
  return 0;
}

/** Set the iauth program version number.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params Version number or name.
 * @return Zero.
 */
static int iauth_cmd_version(struct IAuth *iauth, struct Client *cli,
			     int parc, char **params)
{
  MyFree(iauth->i_version);
  DupString(iauth->i_version, parc > 0 ? params[0] : "<NONE>");
  sendto_opmask_butone(NULL, SNO_AUTH, "iauth version %s running.",
		       iauth->i_version);
  return 0;
}

/** Paste a parameter list together into a single string.
 * @param[in] parc Number of parameters.
 * @param[in] params Parameter list to paste together.
 * @return Pasted parameter list.
 */
static char *paste_params(int parc, char **params)
{
  char *str, *tmp;
  int len = 0, lengths[MAXPARA], i;

  /* Compute the length... */
  for (i = 0; i < parc; i++)
    len += lengths[i] = strlen(params[i]);

  /* Allocate memory, accounting for string lengths, spaces (parc - 1), a
   * sentinel, and the trailing \0
   */
  str = MyMalloc(len + parc + 1);

  /* Build the pasted string */
  for (tmp = str, i = 0; i < parc; i++) {
    if (i) /* add space separator... */
      *(tmp++) = ' ';
    if (i == parc - 1) /* add colon sentinel */
      *(tmp++) = ':';

    /* Copy string component... */
    memcpy(tmp, params[i], lengths[i]);
    tmp += lengths[i]; /* move to end of string */
  }

  /* terminate the string... */
  *tmp = '\0';

  return str; /* return the pasted string */
}

/** Clear cached iauth configuration information.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (0).
 * @param[in] params Parameter list (ignored).
 * @return Zero.
 */
static int iauth_cmd_newconfig(struct IAuth *iauth, struct Client *cli,
			       int parc, char **params)
{
  struct SLink *head;
  struct SLink *next;

  head = iauth->i_config;
  iauth->i_config = NULL;
  for (; head; head = next) {
    next = head->next;
    MyFree(head->value.cp);
    free_link(head);
  }
  sendto_opmask_butone(NULL, SNO_AUTH, "New iauth configuration.");
  return 0;
}

/** Append iauth configuration information.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters.
 * @param[in] params Description of configuration element.
 * @return Zero.
 */
static int iauth_cmd_config(struct IAuth *iauth, struct Client *cli,
			    int parc, char **params)
{
  struct SLink *node;

  if (iauth->i_config) {
    for (node = iauth->i_config; node->next; node = node->next) ;
    node = node->next = make_link();
  } else {
    node = iauth->i_config = make_link();
  }
  node->value.cp = paste_params(parc, params);
  node->next = 0; /* must be explicitly cleared */
  return 0;
}

/** Clear cached iauth configuration information.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (0).
 * @param[in] params Parameter list (ignored).
 * @return Zero.
 */
static int iauth_cmd_newstats(struct IAuth *iauth, struct Client *cli,
			      int parc, char **params)
{
  struct SLink *head;
  struct SLink *next;

  head = iauth->i_stats;
  iauth->i_stats = NULL;
  for (; head; head = next) {
    next = head->next;
    MyFree(head->value.cp);
    free_link(head);
  }
  sendto_opmask_butone(NULL, SNO_AUTH, "New iauth statistics.");
  return 0;
}

/** Append iauth statistics information.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters.
 * @param[in] params Statistics element.
 * @return Zero.
 */
static int iauth_cmd_stats(struct IAuth *iauth, struct Client *cli,
			   int parc, char **params)
{
  struct SLink *node;
  if (iauth->i_stats) {
    for (node = iauth->i_stats; node->next; node = node->next) ;
    node = node->next = make_link();
  } else {
    node = iauth->i_stats = make_link();
  }
  node->value.cp = paste_params(parc, params);
  node->next = 0; /* must be explicitly cleared */
  return 0;
}

/** Set client's username to a trusted string even if it breaks the rules.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params Forced username.
 * @return One.
 */
static int iauth_cmd_username_forced(struct IAuth *iauth, struct Client *cli,
				     int parc, char **params)
{
  assert(cli_auth(cli) != NULL);
  FlagClr(&cli_auth(cli)->flags, AR_AUTH_PENDING);
  if (!EmptyString(params[0])) {
    ircd_strncpy(cli_username(cli), params[0], USERLEN);
    SetGotId(cli);
    FlagSet(&cli_auth(cli)->flags, AR_IAUTH_USERNAME);
    FlagSet(&cli_auth(cli)->flags, AR_IAUTH_FUSERNAME);
  }
  return 1;
}

/** Set client's username to a trusted string.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params Trusted username.
 * @return One.
 */
static int iauth_cmd_username_good(struct IAuth *iauth, struct Client *cli,
				   int parc, char **params)
{
  assert(cli_auth(cli) != NULL);
  FlagClr(&cli_auth(cli)->flags, AR_AUTH_PENDING);
  if (!EmptyString(params[0])) {
    ircd_strncpy(cli_username(cli), params[0], USERLEN);
    SetGotId(cli);
    FlagSet(&cli_auth(cli)->flags, AR_IAUTH_USERNAME);
  }
  return 1;
}

/** Set client's username to an untrusted string.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params Untrusted username.
 * @return One.
 */
static int iauth_cmd_username_bad(struct IAuth *iauth, struct Client *cli,
				  int parc, char **params)
{
  assert(cli_auth(cli) != NULL);
  FlagClr(&cli_auth(cli)->flags, AR_AUTH_PENDING);
  if (!EmptyString(params[0]))
    ircd_strncpy(cli_user(cli)->username, params[0], USERLEN);
  return 1;
}

/** Set client's hostname.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params New hostname for client.
 * @return Non-zero if \a cli authorization should be checked for completion.
 */
static int iauth_cmd_hostname(struct IAuth *iauth, struct Client *cli,
			      int parc, char **params)
{
  struct AuthRequest *auth;

  if (EmptyString(params[0])) {
    sendto_iauth(cli, "E Missing :Missing hostname parameter");
    return 0;
  }

  auth = cli_auth(cli);
  assert(auth != NULL);

  /* If a DNS request is pending, abort it. */
  if (FlagHas(&auth->flags, AR_DNS_PENDING)) {
    FlagClr(&auth->flags, AR_DNS_PENDING);
    delete_resolver_queries(auth);
    if (IsUserPort(cli))
      sendheader(cli, REPORT_FIN_DNS);
  }
  /* Set hostname from params. */
  ircd_strncpy(cli_sockhost(cli), params[0], HOSTLEN);
  return 1;
}

/** Set client's IP address.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params New IP address for client in dotted quad or
 *   standard IPv6 format.
 * @return Zero.
 */
static int iauth_cmd_ip_address(struct IAuth *iauth, struct Client *cli,
				int parc, char **params)
{
  struct irc_in_addr addr;
  struct AuthRequest *auth;

  if (EmptyString(params[0])) {
    sendto_iauth(cli, "E Missing :Missing IP address parameter");
    return 0;
  }

  /* Get AuthRequest for client. */
  auth = cli_auth(cli);
  assert(auth != NULL);

  /* Parse the client's new IP address. */
  if (!ircd_aton(&addr, params[0])) {
    sendto_iauth(cli, "E Invalid :Unable to parse IP address [%s]", params[0]);
    return 0;
  }

  /* If this is the first IP override, save the client's original
   * address in case we get a DNS response later.
   */
  if (!irc_in_addr_valid(&auth->original))
    memcpy(&auth->original, &cli_ip(cli), sizeof(auth->original));

  /* Undo original IP connection in IPcheck. */
  IPcheck_connect_fail(cli);
  ClearIPChecked(cli);

  /* Update the IP and charge them as a remote connect. */
  memcpy(&cli_ip(cli), &addr, sizeof(cli_ip(cli)));
  IPcheck_remote_connect(cli, 0);

  return 0;
}

/** Find a ConfItem structure for a named connection class.
 * @param[in] class_name Name of configuration class to find.
 * @return A ConfItem of type CONF_CLIENT for the class, or NULL on failure.
 */
static struct ConfItem *auth_find_class_conf(const char *class_name)
{
  static struct ConfItem *aconf_list;
  struct ConnectionClass *class;
  struct ConfItem *aconf;

  /* Make sure the configuration class is valid. */
  class = find_class(class_name);
  if (!class)
    return NULL;

  /* Look for an existing ConfItem for the class. */
  for (aconf = aconf_list; aconf; aconf = aconf->next)
    if (aconf->conn_class == class)
      break;

  /* If no ConfItem, create one. */
  if (!aconf) {
    aconf = make_conf(CONF_CLIENT);
    if (!aconf) {
      sendto_opmask_butone(NULL, SNO_AUTH,
                           "Unable to allocate ConfItem for class %s!",
                           ConClass(class));
      return NULL;
    }
    aconf->conn_class = class;
    aconf->next = aconf_list;
    aconf_list = aconf;
  }

  return aconf;
}

/** Accept a client in IAuth.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters.
 * @param[in] params Optional class name for client.
 * @return One.
 */
static int iauth_cmd_done_client(struct IAuth *iauth, struct Client *cli,
				 int parc, char **params)
{
  static time_t warn_time;

  /* Clear iauth pending flag. */
  assert(cli_auth(cli) != NULL);
  FlagClr(&cli_auth(cli)->flags, AR_IAUTH_PENDING);

  /* If a connection class was specified (and usable), assign the client to it. */
  if (!EmptyString(params[0])) {
    struct ConfItem *aconf;

    aconf = auth_find_class_conf(params[0]);
    if (aconf)
      attach_conf(cli, aconf);
    else
      sendto_opmask_butone_ratelimited(NULL, SNO_AUTH, &warn_time,
                                       "iauth tried to use undefined class [%s]",
                                       params[0]);
  }

  return 1;
}

/** Accept a client in IAuth and assign them to an account.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters.
 * @param[in] params Account name and optional class name for client.
 * @return Non-zero if \a cli authorization should be checked for completion.
 */
static int iauth_cmd_done_account(struct IAuth *iauth, struct Client *cli,
				  int parc, char **params)
{
  size_t len;

  /* Sanity check. */
  if (EmptyString(params[0])) {
    sendto_iauth(cli, "E Missing :Missing account parameter");
    return 0;
  }
  /* Check length of account name. */
  len = strcspn(params[0], ": ");
  if (len > ACCOUNTLEN) {
    sendto_iauth(cli, "E Invalid :Account parameter too long");
    return 0;
  }
  /* If account has a creation timestamp, use it. */
  assert(cli_user(cli) != NULL);
  if (params[0][len] == ':') {
    char *end;
    cli_user(cli)->acc_create = strtoul(params[0] + len + 1, &end, 10);
    if (*end == ':')
      cli_user(cli)->acc_id = strtoul(end + 1, NULL, 10);
  }

  /* Copy account name to User structure. */
  ircd_strncpy(cli_user(cli)->account, params[0], ACCOUNTLEN);
  SetAccount(cli);

  /* Fall through to the normal "done" handler. */
  return iauth_cmd_done_client(iauth, cli, parc - 1, params + 1);
}

/** Reject a client's connection.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params Optional kill message.
 * @return Zero.
 */
static int iauth_cmd_kill(struct IAuth *iauth, struct Client *cli,
			  int parc, char **params)
{
  if (cli_auth(cli))
    FlagClr(&cli_auth(cli)->flags, AR_IAUTH_PENDING);
  if (EmptyString(params[0]))
    params[0] = "Access denied";
  exit_client(cli, cli, &me, params[0]);
  return 0;
}

/** Accept a client in IAuth and assign them to an account.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters.
 * @param[in] params Account name and optional class name for client.
 * @return Non-zero if \a cli authorization should be checked for completion.
 */
static int iauth_cmd_sasl(struct IAuth *iauth, struct Client *cli,
				  int parc, char **params)
{
  assert(cli_auth(cli) != NULL);
  /* Sanity check. */
  if (EmptyString(params[0])) {
    return 0;
  }
  char *cmd = params[0];
	if(!ircd_strcmp(cmd, "Q")) {
		sendcmdto_one(&me, CMD_AUTHENTICATE, cli, params[1]); 
	} else if(!ircd_strcmp(cmd, "O")) {
		send_reply(cli, ERR_SASLABORTED);
	} else if(!ircd_strcmp(cmd, "S")) {
		if (EmptyString(params[3]) || EmptyString(params[4])) {
			return 0;
		}		
		char *nick = params[1];
		char *account = params[2]; 
		char *timestamp = params[3];
		char *id = params[4];     
		ircd_strncpy(cli_user(cli)->account, account, ACCOUNTLEN);
        cli_user(cli)->acc_create = atoi(timestamp);
	    cli_user(cli)->acc_id = strtoul(id, NULL, 10);
		SetAccount(cli);
		ircd_snprintf(0, cli_user(cli)->authhost, HOSTLEN, "%s.%s", account, feature_str(FEAT_HIDDEN_HOST));
		int killreason = find_kill(cli,1);	
	    if (killreason)
	    {
		  ++ServerStats->is_ref;
		  return exit_client(cli, cli, &me,
							(killreason == -1 ? "K-lined" : "G-lined"));
	    }		
		send_reply(cli, RPL_LOGGEDIN, cli, cli_name(cli), account);
		send_reply(cli, RPL_SASLSUCCESS);
	} else if(!ircd_strcmp(cmd, "N")) {
		send_reply(cli, ERR_NICKLOCKED);
		send_reply(cli, ERR_SASLFAIL);	
	} else if(!ircd_strcmp(cmd, "L")) {
		send_reply(cli, ERR_SASLTOOLONG);
		send_reply(cli, ERR_SASLFAIL);
	} else if(!ircd_strcmp(cmd, "A")) {
		send_reply(cli, ERR_SASLALREADY);
		send_reply(cli, ERR_SASLFAIL);
	} else if(!ircd_strcmp(cmd, "F")) {
		send_reply(cli, ERR_SASLFAIL);
	}
  return 0;
}

/** Change a client's usermode.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (at least one).
 * @param[in] params Usermode arguments for client (with the first
 *   starting with '+').
 * @return Zero.
 */
static int iauth_cmd_usermode(struct IAuth *iauth, struct Client *cli,
                              int parc, char **params)
{
  if (params[0][0] == '+')
  {
    set_user_mode(cli, cli, parc + 2, params - 2, ALLOWMODES_ANY);
  }
  return 0;
}


/** Send a challenge string to the client.
 * @param[in] iauth Active IAuth session.
 * @param[in] cli Client referenced by command.
 * @param[in] parc Number of parameters (1).
 * @param[in] params Challenge message for client.
 * @return Zero.
 */
static int iauth_cmd_challenge(struct IAuth *iauth, struct Client *cli,
			       int parc, char **params)
{
  if (!EmptyString(params[0]))
    sendrawto_one(cli, "NOTICE AUTH :*** %s", params[0]);
  return 0;
}

/** Parse a \a message from \a iauth.
 * @param[in] iauth Active IAuth session.
 * @param[in] message Message to be parsed.
 */
static void iauth_parse(struct IAuth *iauth, char *message)
{
  char *params[MAXPARA + 1]; /* leave space for NULL */
  int parc = 0;
  iauth_cmd_handler handler;
  struct AuthRequest *auth;
  struct Client *cli;
  int has_cli;
  int id;

  /* Find command handler... */
  switch (*(message++)) {
  case '>': handler = iauth_cmd_snotice; has_cli = 0; break;
  case 'G': handler = iauth_cmd_debuglevel; has_cli = 0; break;
  case 'O': handler = iauth_cmd_policy; has_cli = 0; break;
  case 'V': handler = iauth_cmd_version; has_cli = 0; break;
  case 'a': handler = iauth_cmd_newconfig; has_cli = 0; break;
  case 'A': handler = iauth_cmd_config; has_cli = 0; break;
  case 's': handler = iauth_cmd_newstats; has_cli = 0; break;
  case 'S': handler = iauth_cmd_stats; has_cli = 0; break;
  case 'Y': handler = iauth_cmd_sasl; has_cli = 1; break;
  case 'o': handler = iauth_cmd_username_forced; has_cli = 1; break;
  case 'U': handler = iauth_cmd_username_good; has_cli = 1; break;
  case 'u': handler = iauth_cmd_username_bad; has_cli = 1; break;
  case 'N': handler = iauth_cmd_hostname; has_cli = 1; break;
  case 'I': handler = iauth_cmd_ip_address; has_cli = 1; break;
  case 'M': handler = iauth_cmd_usermode; has_cli = 1; break;
  case 'C': handler = iauth_cmd_challenge; has_cli = 1; break;
  case 'D': handler = iauth_cmd_done_client; has_cli = 1; break;
  case 'R': handler = iauth_cmd_done_account; has_cli = 1; break;
  case 'k': /* The 'k' command indicates the user should be booted
	     * off without telling opers.  There is no way to
	     * signal that to exit_client(), so we fall through to
	     * the case that we do implement.
	     */
  case 'K': handler = iauth_cmd_kill; has_cli = 2; break;
  case 'r': /* we handle termination directly */ return;
  default:  sendto_iauth(NULL, "E Garbage :[%s]", message); return;
  }

  while (parc < MAXPARA) {
    while (IsSpace(*message)) /* skip leading whitespace */
      message++;

    if (!*message) /* hit the end of the string, break out */
      break;

    if (*message == ':') { /* found sentinel... */
      params[parc++] = message + 1;
      break; /* it's the last parameter anyway */
    }

    params[parc++] = message; /* save the parameter */
    while (*message && !IsSpace(*message))
      message++; /* find the end of the parameter */

    if (*message) /* terminate the parameter */
      *(message++) = '\0';
  }

  params[parc] = NULL; /* terminate the parameter list */

  /* Check to see if the command specifies a client... */
  if (!has_cli) {
    /* Handler does not need a client. */
    handler(iauth, NULL, parc, params);
  } else {
    /* Try to find the client associated with the request. */
    id = strtol(params[0], NULL, 10);
    if (parc < 3)
      sendto_iauth(NULL, "E Missing :Need <id> <ip> <port>");
    else if (id < 0 || id > HighestFd || !(cli = LocalClientArray[id]))
      /* Client no longer exists (or never existed). */
      sendto_iauth(NULL, "E Gone :[%s %s %s]", params[0], params[1],
		   params[2]);
    else if ((!(auth = cli_auth(cli)) ||
	      !FlagHas(&auth->flags, AR_IAUTH_PENDING)) &&
	     has_cli == 1)
      /* Client is done with IAuth checks. */
      sendto_iauth(cli, "E Done :[%s %s %s]", params[0], params[1], params[2]);
    else {
      struct irc_sockaddr addr;
      int res;

      /* Parse IP address and port number from parameters */
      res = ipmask_parse(params[1], &addr.addr, NULL);
      addr.port = strtol(params[2], NULL, 10);

      /* Check IP address and port number against expected. */
      if (0 == res ||
	  irc_in_addr_cmp(&addr.addr, &cli_ip(cli)) ||
	  (auth && addr.port != auth->port))
	/* Report mismatch to iauth. */
	sendto_iauth(cli, "E Mismatch :[%s] != [%s]", params[1],
		     ircd_ntoa(&cli_ip(cli)));
      else if (handler(iauth, cli, parc - 3, params + 3))
	/* Handler indicated a possible state change. */
	check_auth_finished(auth);
    }
  }
}

/** Read input from \a iauth.
 * Reads up to SERVER_TCP_WINDOW bytes per pass.
 * @param[in] iauth Readable connection.
 */
static void iauth_read(struct IAuth *iauth)
{
  static char readbuf[SERVER_TCP_WINDOW];
  unsigned int length, count;
  char *sol;
  char *eol;

  /* Copy partial data to readbuf, append new data. */
  length = iauth->i_count;
  memcpy(readbuf, iauth->i_buffer, length);
  if (IO_SUCCESS != os_recv_nonb(s_fd(i_socket(iauth)),
				 readbuf + length,
				 sizeof(readbuf) - length - 1,
				 &count))
    return;
  readbuf[length += count] = '\0';

  /* Parse each complete line. */
  for (sol = readbuf; (eol = strchr(sol, '\n')) != NULL; sol = eol + 1) {
    *eol = '\0';
    if (*(eol - 1) == '\r') /* take out carriage returns, too... */
      *(eol - 1) = '\0';

    /* If spammy debug, send the message to opers. */
    if (i_debug(iauth) > 1)
      sendto_opmask_butone(NULL, SNO_AUTH, "Parsing: \"%s\"", sol);

    /* Parse the line... */
    iauth_parse(iauth, sol);
  }

  /* Put unused data back into connection's buffer. */
  iauth->i_count = strlen(sol);
  if (iauth->i_count > BUFSIZE)
    iauth->i_count = BUFSIZE;
  memcpy(iauth->i_buffer, sol, iauth->i_count);
}

/** Handle socket activity for an %IAuth connection.
 * @param[in] ev &Socket event; the IAuth connection is the user data
 *   pointer for the socket.
 */
static void iauth_sock_callback(struct Event *ev)
{
  struct IAuth *iauth;

  assert(0 != ev_socket(ev));
  iauth = (struct IAuth*) s_data(ev_socket(ev));
  assert(0 != iauth);

  switch (ev_type(ev)) {
  case ET_DESTROY:
    /* Hm, what happened here? */
    if (!IAuthHas(iauth, IAUTH_CLOSING))
      iauth_do_spawn(iauth, 1);
    break;
  case ET_READ:
    iauth_read(iauth);
    break;
  case ET_WRITE:
    IAuthClr(iauth, IAUTH_BLOCKED);
    iauth_write(iauth);
    break;
  case ET_ERROR:
    log_write(LS_IAUTH, L_ERROR, 0, "IAuth socket error: %s", strerror(ev_data(ev)));
    /* and fall through to the ET_EOF case */
  case ET_EOF:
    iauth_disconnect(iauth);
    break;
  default:
    assert(0 && "Unrecognized event type");
    break;
  }
}

/** Read error input from \a iauth.
 * @param[in] iauth Readable connection.
 */
static void iauth_read_stderr(struct IAuth *iauth)
{
  static char readbuf[SERVER_TCP_WINDOW];
  unsigned int length, count;
  char *sol;
  char *eol;

  /* Copy partial data to readbuf, append new data. */
  length = iauth->i_errcount;
  memcpy(readbuf, iauth->i_errbuf, length);
  if (IO_SUCCESS != os_recv_nonb(s_fd(i_stderr(iauth)),
                                 readbuf + length,
                                 sizeof(readbuf) - length - 1,
                                 &count))
    return;
  readbuf[length += count] = '\0';

  /* Send each complete line to SNO_AUTH. */
  for (sol = readbuf; (eol = strchr(sol, '\n')) != NULL; sol = eol + 1) {
    *eol = '\0';
    if (*(eol - 1) == '\r') /* take out carriage returns, too... */
      *(eol - 1) = '\0';
    Debug((DEBUG_ERROR, "IAuth error: %s", sol));
    log_write(LS_IAUTH, L_ERROR, 0, "IAuth error: %s", sol);
    sendto_opmask_butone(NULL, SNO_AUTH, "%s", sol);
  }

  /* Put unused data back into connection's buffer. */
  iauth->i_errcount = strlen(sol);
  if (iauth->i_errcount > BUFSIZE)
    iauth->i_errcount = BUFSIZE;
  memcpy(iauth->i_errbuf, sol, iauth->i_errcount);
}

/** Handle error socket activity for an %IAuth connection.
 * @param[in] ev &Socket event; the IAuth connection is the user data
 *   pointer for the socket.
 */
static void iauth_stderr_callback(struct Event *ev)
{
  struct IAuth *iauth;

  assert(0 != ev_socket(ev));
  iauth = (struct IAuth*) s_data(ev_socket(ev));
  assert(0 != iauth);

  switch (ev_type(ev)) {
  case ET_READ:
    iauth_read_stderr(iauth);
    break;
  case ET_ERROR:
    log_write(LS_IAUTH, L_ERROR, 0, "IAuth stderr error: %s", strerror(ev_data(ev)));
    /* and fall through to the ET_EOF/ET_DESTROY case */
  case ET_DESTROY:
  case ET_EOF:
    break;
  default:
    assert(0 && "Unrecognized event type");
    break;
  }
}

/** Report active iauth's configuration to \a cptr.
 * @param[in] cptr Client requesting statistics.
 * @param[in] sd Stats descriptor for request.
 * @param[in] param Extra parameter from user (may be NULL).
 */
void report_iauth_conf(struct Client *cptr, const struct StatDesc *sd, char *param)
{
    struct SLink *link;

    if (iauth) for (link = iauth->i_config; link; link = link->next)
    {
        send_reply(cptr, SND_EXPLICIT | RPL_STATSDEBUG, ":%s",
                   link->value.cp);
    }
}

/** Report active iauth's statistics to \a cptr.
 * @param[in] cptr Client requesting statistics.
 * @param[in] sd Stats descriptor for request.
 * @param[in] param Extra parameter from user (may be NULL).
 */
 void report_iauth_stats(struct Client *cptr, const struct StatDesc *sd, char *param)
{
    struct SLink *link;

    if (iauth) for (link = iauth->i_stats; link; link = link->next)
    {
        send_reply(cptr, SND_EXPLICIT | RPL_STATSDEBUG, ":%s",
                   link->value.cp);
    }
}
