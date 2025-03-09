/*
 * IRC - Internet Relay Chat, ircd/m_asll.c
 * Copyright (C) 2002 Alex Badea <vampire@p16.pub.ro>
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
 * $Id: m_asll.c,v 1.3.2.1 2007/03/28 04:04:32 entrope Exp $
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
#include "ircd.h"
#include "ircd_crypt_b64.h"
#include "ircd_features.h"
#include "ircd_log.h"
#include "ircd_relay.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "ircd_snprintf.h"
#include "msg.h"
#include "numeric.h"
#include "numnicks.h"
#include "send.h"
#include "s_auth.h"
#include "s_conf.h"
#include "s_user.h"

/* #include <assert.h> -- Now using assert in ircd_log.h */
#include <stdlib.h>
#include <stdio.h>

/*
 * m_sasl - client message handler
 */
int m_sasl(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{		
  char *buf, *buf1, *arr[3], text[256], *user, nick[NICKLEN], auth[NICKLEN], pass[BUFSIZE];
  struct Client *target; 
  if (parc < 2 || *parv[1] == '\0')
    return need_more_params(sptr, "AUTHENTICATE");	
  if(!CapHas(cli_active(sptr), CAP_SASL)) {
	 send_reply(sptr, ERR_SASLFAIL); 
  } else {
	 sptr->cli_sasla = 0;
	 if(!ircd_strcmp(parv[1], "PLAIN")) {
		if(sptr->cli_sasl != 1) {
			sendrawto_one(sptr, "AUTHENTICATE %s", "+");
			sptr->cli_sasl = 1;
		} else {
			send_reply(sptr, ERR_SASLALREADY);
		}
	 } else if(*parv[1] == '*') {
		send_reply(sptr, ERR_SASLABORTED);
	 } else if(sptr->cli_sasl == 1){
		buf = base64_decode(parv[1]);
		int cnt = 0;
		for (user = buf; *user; user += strlen(user) + 1) {
			arr[cnt] = user;
			cnt++;
		}
		if(!arr) {
			send_reply(sptr, ERR_SASLFAIL); 
			return 0;
		}
		ircd_strncpy(nick, arr[0], NICKLEN);
        ircd_strncpy(auth, arr[1], NICKLEN);
        ircd_strncpy(pass, arr[2], BUFSIZE);
		ircd_snprintf(0, text, 255, "SASL %s %s %s", nick, auth, pass);
		if ((target = FindServer(feature_str(FEAT_SASL_SERVER)))) {
			sendrawto_one(target, "%s %s %s", cli_yxx(&me), text, cli_yxx(&me));
            ircd_strncpy(sptr->cli_saslnick, nick, NICKLEN);
            ircd_strncpy(sptr->cli_saslacc, auth, NICKLEN);
		} else
			send_reply(sptr, ERR_SASLFAIL);

	 } else {
	    send_reply(sptr, RPL_SASLMECHS, "PLAIN");	 
	 }
  }
  return 0;
}

/*
 * ms_sasl - server message handler
 */
int ms_sasl(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  if (parc < 5)
    return need_more_params(sptr, "AUTHENTICATE");
  char *cmd = parv[2];
  char *nick = parv[3];
  char *account = parv[4];
  struct Client *usr = FindSasl(nick);
  if(usr) {
	if(!ircd_strcmp(cmd, "SUCCESS")) {
		ircd_strncpy(cli_user(usr)->account, account, ACCOUNTLEN);
		SetAccount(usr);
		send_reply(usr, RPL_LOGGEDIN, usr, nick, account);
		send_reply(usr, RPL_SASLSUCCESS);
	} else if(usr && !ircd_strcmp(cmd, "NOTYOU")) {
		send_reply(usr, ERR_NICKLOCKED); 
	} else if(usr && !ircd_strcmp(cmd, "ALREADY")) {
		send_reply(usr, ERR_SASLALREADY); 
	} else {
		send_reply(usr, ERR_SASLFAIL); 
	}
  } else {
	  send_reply(sptr, ERR_SASLFAIL); 
  }
  return 0;
}