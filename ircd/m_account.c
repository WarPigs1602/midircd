/*
 * IRC - Internet Relay Chat, ircd/m_account.c
 * Copyright (C) 2002 Kevin L. Mitchell <klmitch@mit.edu>
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
 * $Id: m_account.c,v 1.6 2004/12/11 05:13:46 klmitch Exp $
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
#include "gline.h"
#include "ircd.h"
#include "ircd_features.h"
#include "ircd_log.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "msg.h"
#include "numeric.h"
#include "numnicks.h"
#include "s_conf.h"
#include "s_debug.h"
#include "s_misc.h"
#include "s_user.h"
#include "send.h"

/* #include <assert.h> -- Now using assert in ircd_log.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ms_account - server message handler
 *
 * parv[0] = sender prefix
 * parv[1] = numeric of client to act on
 * parv[2] = account name (12 characters or less)
 * parv[3] = account timestamp (optional)
 * parv[4] = account id (optional, requires timestamp to be set to use)
 */
int ms_account(struct Client* cptr, struct Client* sptr, int parc,
	       char* parv[])
{
  struct Client *acptr;
  struct Gline *gline;
  int killreason;

  if (parc < 3)
    return need_more_params(sptr, "ACCOUNT");

  if (!IsServer(sptr))
    return protocol_violation(cptr, "ACCOUNT from non-server %s",
			      cli_name(sptr));

  if (!(acptr = findNUser(parv[1])))
    return 0; /* Ignore ACCOUNT for a user that QUIT; probably crossed */

  if (IsAccount(acptr) && ((parc < 5) || (parc >= 5 && cli_user(acptr)->acc_id)))
    return protocol_violation(cptr, "ACCOUNT for already registered user %s "
			      "(%s -> %s)", cli_name(acptr),
			      cli_user(acptr)->account, parv[2]);

  /* special case for current snircd release only */
  if (parc >= 5 && cli_user(acptr)->account[0]) {
    if (strcmp(cli_user(acptr)->account, parv[2])) {
      return protocol_violation(cptr, "ACCOUNT change for already registered user %s "
                                "(%s -> %s)", cli_name(acptr),
                                cli_user(acptr)->account, parv[2]);
    }
    cli_user(acptr)->acc_create = atoi(parv[3]);
    cli_user(acptr)->acc_id = strtoul(parv[4], NULL, 10);      
    sendcmdto_serv_butone(sptr, CMD_ACCOUNT, cptr, "%C %s %Tu %lu",
                              acptr, cli_user(acptr)->account,
                              cli_user(acptr)->acc_create,
                              cli_user(acptr)->acc_id);
    return 0;
  }

  assert(0 == cli_user(acptr)->account[0]);

  if (strlen(parv[2]) > ACCOUNTLEN)
    return protocol_violation(cptr,
                              "Received account (%s) longer than %d for %s; "
                              "ignoring.",
                              parv[2], ACCOUNTLEN, cli_name(acptr));

  if (parc > 3) {
    cli_user(acptr)->acc_create = atoi(parv[3]);
    Debug((DEBUG_DEBUG, "Received timestamped account: account \"%s\", "
           "timestamp %Tu", parv[2], cli_user(acptr)->acc_create));
    if (parc > 4) {
      cli_user(acptr)->acc_id = strtoul(parv[4], NULL, 10); 
      Debug((DEBUG_DEBUG, "Received account id for account \"%s\": id %lu", parv[2], cli_user(acptr)->acc_id));
    }
  }

  ircd_strncpy(cli_user(acptr)->account, parv[2], ACCOUNTLEN);
    hide_hostmask(acptr, FLAG_ACCOUNT);
   if (cli_user(acptr)->acc_id) {
     sendcmdto_serv_butone(sptr, CMD_ACCOUNT, cptr, "%C %s %Tu %lu",
                           acptr, cli_user(acptr)->account,
                           cli_user(acptr)->acc_create,
                           cli_user(acptr)->acc_id);
   } else if (cli_user(acptr)->acc_create) {
     sendcmdto_serv_butone(sptr, CMD_ACCOUNT, cptr, "%C %s %Tu",
                           acptr, cli_user(acptr)->account,
                           cli_user(acptr)->acc_create);
   } else {
     sendcmdto_serv_butone(sptr, CMD_ACCOUNT, cptr, "%C %s",
                           acptr, cli_user(acptr)->account);
   }
  /* G-Line fix for accounts */
   sprintf(cli_user(acptr)->authhost, "%s.%s", parv[2], feature_str(FEAT_HIDDEN_HOST));	
   killreason = find_kill(acptr);
   if (killreason)
   {
     ++ServerStats->is_ref;
     return exit_client(acptr, acptr, cptr,
                        (killreason == -1 ? "K-lined" : "G-lined"));
   }
  return 0;
}
