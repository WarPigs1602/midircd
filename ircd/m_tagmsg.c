/*
 * IRC - Internet Relay Chat, ircd/m_tagmsg.c
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
 * $Id$
 */

/*
 * m_functions execute protocol messages on this server:
 *
 *    cptr    is always NON-sptr, pointing to a *LOCAL* client
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
 *                    parv[parc] == sptr, *always*
 *
 *            note:   it is guaranteed that parv[0]..parv[parc-1] are all
 *                    non-NULL pointers.
 */
#include "config.h"

#include "capab.h"
#include "channel.h"
#include "client.h"
#include "gline.h"
#include "hash.h"
#include "ircd.h"
#include "ircd_chattr.h"
#include "ircd_features.h"
#include "ircd_log.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "msg.h"
#include "numeric.h"
#include "numnicks.h"
#include "s_debug.h"
#include "s_user.h"
#include "send.h"
#include "sys.h"

/* #include <assert.h> -- Now using assert in ircd_log.h */
#include <stdio.h>
#include <string.h>

/*
 * m_tagmsg - typing message handler
 */
int m_tagmsg(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  return 0;
}

/*
 * m_tagmsg - typing message handler
 */
int m_tagmsg_typing(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  if (parc < 3)
    return need_more_params(sptr, "TAGMSG");
  if(feature_bool(FEAT_CAP_MESSAGETAGS)) {
	sendcmdto_capflag_tagmsg_butone(sptr, FindChannel(parv[2]),
          sptr, "@+draft/typing=active");
	sendcmdto_capflag_tagmsg_butone(sptr, FindChannel(parv[2]),
          sptr, "@+typing=active");		  
  }
  return 0;
}

/*
 * m_tagmsg - typing message handler
 */
int m_tagmsg_paused(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  if (parc < 3)
    return need_more_params(sptr, "TAGMSG");
  if(feature_bool(FEAT_CAP_MESSAGETAGS)) {
	sendcmdto_capflag_tagmsg_butone(sptr, FindChannel(parv[2]),
          sptr, "@+draft/typing=paused");
	sendcmdto_capflag_tagmsg_butone(sptr, FindChannel(parv[2]),
          sptr, "@+typing=paused");		  
  }
  return 0;
}

/*
 * m_tagmsg - typing message handler
 */
int m_tagmsg_done(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  if (parc < 3)
    return need_more_params(sptr, "TAGMSG");
  if(feature_bool(FEAT_CAP_MESSAGETAGS)) {
	sendcmdto_capflag_tagmsg_butone(sptr, FindChannel(parv[2]),
          sptr, "@+draft/typing=done");
	sendcmdto_capflag_tagmsg_butone(sptr, FindChannel(parv[2]),
          sptr, "@+typing=done");
  }
  return 0;
}

/*
 * m_tagmsg - typing message handler
 */
int ms_tagmsg(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  if (parc < 3)
    return need_more_params(sptr, "TAGMSG");
  if(feature_bool(FEAT_CAP_MESSAGETAGS)) {
	if (FindChannel(parv[2]))
		sendcmdto_capflag_tagmsg_butone(FindUser(parv[0]), FindChannel(parv[2]), FindUser(parv[0]), parv[1]);
  }
  return 0;
}
