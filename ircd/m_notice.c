/*
 * IRC - Internet Relay Chat, ircd/m_notice.c
 * Copyright (C) 1990 Jarkko Oikarinen and
 *                    University of Oulu, Computing Center
 *
 * See file AUTHORS in IRC package for additional names of
 * the programmers.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: m_notice.c,v 1.10 2004/12/11 05:13:48 klmitch Exp $
 */

/*
 * m_functions execute protocol messages on this server:
 *
 *    cptr    is always NON-NULL, pointing to a *LOCAL* client
 *            structure (with an open socket connected!). This
 *            identifies the physical socket where the message
 *            originated (or which caused the m_function to be
 *            executed--some m_functions may call others...).
 *
 *    sptr    is the source of the message, defined by the
 *            prefix part of the message if present. If not
 *            or prefix not found, then sptr==cptr.
 *
 *            (!IsServer(cptr)) => (cptr == sptr), because
 *            prefixes are taken *only* from servers...
 *
 *            (IsServer(cptr))
 *                    (sptr == cptr) => the message didn't
 *                    have the prefix.
 *
 *                    (sptr != cptr && IsServer(sptr) means
 *                    the prefix specified servername. (?)
 *
 *                    (sptr != cptr && !IsServer(sptr) means
 *                    that message originated from a remote
 *                    user (not local).
 *
 *            combining
 *
 *            (!IsServer(sptr)) means that, sptr can safely
 *            taken as defining the target structure of the
 *            message in this server.
 *
 *    *Always* true (if 'parse' and others are working correct):
 *
 *    1)      sptr->from == cptr  (note: cptr->from == cptr)
 *
 *    2)      MyConnect(sptr) <=> sptr == cptr (e.g. sptr
 *            *cannot* be a local connection, unless it's
 *            actually cptr!). [MyConnect(x) should probably
 *            be defined as (x == x->from) --msa ]
 *
 *    parc    number of variable parameter strings (if zero,
 *            parv is allowed to be NULL)
 *
 *    parv    a NULL terminated list of parameter pointers,
 *
 *                    parv[0], sender (prefix string), if not present
 *                            this points to an empty string.
 *                    parv[1]...parv[parc-1]
 *                            pointers to additional parameters
 *                    parv[parc] == NULL, *always*
 *
 *            note:   it is guaranteed that parv[0]..parv[parc-1] are all
 *                    non-NULL pointers.
 */
#include "config.h"

#include "client.h"
#include "hash.h"
#include "struct.h"
#include "ircd.h"
#include "ircd_chattr.h"
#include "ircd_log.h"
#include "ircd_relay.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "ircd_snprintf.h"
#include "match.h"
#include "msg.h"
#include "numeric.h"
#include "numnicks.h"
#include "s_conf.h"
#include "s_debug.h"
#include "s_misc.h"
#include "s_user.h"
#include "send.h"

/* #include <assert.h> -- Now using assert in ircd_log.h */
#include <string.h>
#include <stdio.h>

#if !defined(XXX_BOGUS_TEMP_HACK)
#include "handlers.h"
#endif

/*
 * m_notice - generic message handler
 */
int m_notice(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  char*           name;
  char*           server;
  int             i;
  int             count;
  int             chancount;
  char*           vector[MAXTARGETS];

  assert(0 != cptr);
  assert(cptr == sptr);

  ClrFlag(sptr, FLAG_TS8);

  if (parc < 2 || EmptyString(parv[1]))
    return send_reply(sptr, ERR_NORECIPIENT, MSG_NOTICE);

  if (parc < 3 || EmptyString(parv[parc - 1]))
    return send_reply(sptr, ERR_NOTEXTTOSEND);

  if (parv[1][0] == '@' && IsChannelPrefix(parv[1][1])) {
    parv[1]++;                        /* Get rid of '@' */
    return m_wallchops(cptr, sptr, parc, parv);
  }

  count = unique_name_vector(parv[1], ',', vector, MAXTARGETS);

  chancount = 0;
  for (i = 0; i < count; ++i) {
    name = vector[i];

    if (IsChannelPrefix(*name))
      chancount++;
  }

  for (i = 0; i < count; ++i) {
    name = vector[i];
    /*
     * channel msg?
     */
    if (IsChannelPrefix(*name)) {
      relay_channel_notice(sptr, name, parv[parc - 1], chancount);
    }
    /*
     * we have to check for the '@' at least once no matter what we do
     * handle it first so we don't have to do it twice
     */
    else if ((server = strchr(name, '@')))
      relay_directed_notice(sptr, name, server, parv[parc - 1]);
    else 
      relay_private_notice(sptr, name, parv[parc - 1]);
  }
  return 0;
}

/*
 * ms_notice - server message handler
 */
int ms_notice(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
    // SASL message handling
    if (parc >= 3 && parv[2] && !strncmp(parv[2], "SASL", 4)) {
        const char* sasl_service_nick = feature_str(FEAT_SASL_SERVICE_NAMES);
        if (sasl_service_nick && !ircd_strcmp(cli_name(sptr), sasl_service_nick)) {
            char sasl_nick[64] = { 0 }, sasl_cmd[8] = { 0 }, sasl_account[128] = { 0 }, sasl_ts[32] = { 0 }, sasl_id[32] = { 0 };
            int args = sscanf(parv[2], "SASL %63s %7s %127s %31s %31s", sasl_nick, sasl_cmd, sasl_account, sasl_ts, sasl_id);

            struct Client* target = FindClient(sasl_nick);

            if (!target) {
                send_reply(sptr, ERR_NOSUCHNICK, sasl_nick);
                return 0;
            }

            if (EmptyString(sasl_cmd)) {
                send_reply(target, ERR_SASLFAIL);
                return 0;
            }

            if (!ircd_strcmp(sasl_cmd, "Q")) {
                sendcmdto_one(&me, CMD_AUTHENTICATE, target, "+");
            }
            else if (!ircd_strcmp(sasl_cmd, "O")) {
                send_reply(target, ERR_SASLABORTED);
            }
            else if (!ircd_strcmp(sasl_cmd, "M")) {
                send_reply(target, RPL_SASLMECHS, sasl_account);
                send_reply(target, ERR_SASLFAIL);
            }
            else if (!ircd_strcmp(sasl_cmd, "S")) {
                if (args < 5 || EmptyString(sasl_ts) || EmptyString(sasl_id)) {
                    send_reply(target, ERR_SASLFAIL);
                    return 0;
                }
                ircd_strncpy(cli_user(target)->account, sasl_account, ACCOUNTLEN);
                cli_user(target)->acc_create = atoi(sasl_ts);
                cli_user(target)->acc_id = strtoul(sasl_id, NULL, 10);
                SetAccount(target);
                ircd_snprintf(0, cli_user(target)->authhost, HOSTLEN, "%s.%s", sasl_account, feature_str(FEAT_HIDDEN_HOST));

                int killreason = find_kill(target, 1);
                if (killreason) {
                    ++ServerStats->is_ref;
                    return exit_client(target, target, sptr, (killreason == -1 ? "K-lined" : "G-lined"));
                }
                send_reply(target, RPL_LOGGEDIN, sptr, cli_name(target), sasl_account);
                send_reply(target, RPL_SASLSUCCESS);
            }
            else if (!ircd_strcmp(sasl_cmd, "N")) {
                send_reply(target, ERR_NICKLOCKED);
                send_reply(target, ERR_SASLFAIL);
            }
            else if (!ircd_strcmp(sasl_cmd, "L")) {
                send_reply(target, ERR_SASLTOOLONG);
                send_reply(target, ERR_SASLFAIL);
            }
            else if (!ircd_strcmp(sasl_cmd, "A")) {
                send_reply(target, ERR_SASLALREADY);
                send_reply(target, ERR_SASLFAIL);
            }
            else if (!ircd_strcmp(sasl_cmd, "F")) {
                send_reply(target, ERR_SASLFAIL);
            }
            else {
                send_reply(target, ERR_SASLFAIL);
            }
            return 0;
        }
    }
  char* name;
  char* server;

  ClrFlag(sptr, FLAG_TS8);

  if (parc < 3) {
    /*
     * we can't deliver it, sending an error back is pointless
     */
    return protocol_violation(sptr,"Not enough params for NOTICE");
  }

  name = parv[1];
  /*
   * channel msg?
   */
  if (IsChannelPrefix(*name)) {
    server_relay_channel_notice(sptr, name, parv[parc - 1]);
  }
  /*
   * coming from another server, we have to check this here
   */
  else if ('$' == *name && IsOper(sptr)) {
    server_relay_masked_notice(sptr, name, parv[parc - 1]);
  }
  else if ((server = strchr(name, '@'))) {
    /*
     * XXX - can't get away with not doing everything
     * relay_directed_notice has to do
     */
    relay_directed_notice(sptr, name, server, parv[parc - 1]);
  }
  else {
    server_relay_private_notice(sptr, name, parv[parc - 1]);
  }
  return 0;
}

/*
 * mo_notice - oper message handler
 */
int mo_notice(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  char*           name;
  char*           server;
  int             i;
  int             count;
  int             chancount;
  char*           vector[MAXTARGETS];
  assert(0 != cptr);
  assert(cptr == sptr);

  ClrFlag(sptr, FLAG_TS8);

  if (parc < 2 || EmptyString(parv[1]))
    return send_reply(sptr, ERR_NORECIPIENT, MSG_NOTICE);

  if (parc < 3 || EmptyString(parv[parc - 1]))
    return send_reply(sptr, ERR_NOTEXTTOSEND);

  if (parv[1][0] == '@' && IsChannelPrefix(parv[1][1])) {
    parv[1]++;                        /* Get rid of '@' */
    return m_wallchops(cptr, sptr, parc, parv);
  }

  count = unique_name_vector(parv[1], ',', vector, MAXTARGETS);

  chancount = 0;
  for (i = 0; i < count; ++i) {
    name = vector[i];

    if (IsChannelPrefix(*name))
      chancount++;
  }

  for (i = 0; i < count; ++i) {
    name = vector[i];
    /*
     * channel msg?
     */
    if (IsChannelPrefix(*name))
      relay_channel_notice(sptr, name, parv[parc - 1], chancount);

    else if (*name == '$')
      relay_masked_notice(sptr, name, parv[parc - 1]);

    else if ((server = strchr(name, '@')))
      relay_directed_notice(sptr, name, server, parv[parc - 1]);

    else 
      relay_private_notice(sptr, name, parv[parc - 1]);
  }
  return 0;
}
