/*
 * IRC - Internet Relay Chat, ircd/m_wallchops.c
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
 * $Id: m_wallchops.c,v 1.9 2004/12/11 05:14:03 klmitch Exp $
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

#include "channel.h"
#include "client.h"
#include "hash.h"
#include "ircd.h"
#include "ircd_log.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "msg.h"
#include "numeric.h"
#include "numnicks.h"
#include "s_user.h"
#include "send.h"

/* #include <assert.h> -- Now using assert in ircd_log.h */

/*
 * m_wallchops - local generic message handler
 */
int m_wallchops(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  struct Channel *chptr;
  const char *ch;

  assert(0 != cptr);
  assert(cptr == sptr);

  ClrFlag(sptr, FLAG_TS8);

  if (parc < 2 || EmptyString(parv[1]))
    return send_reply(sptr, ERR_NORECIPIENT, "WALLCHOPS");

  if (parc < 3 || EmptyString(parv[parc - 1]))
    return send_reply(sptr, ERR_NOTEXTTOSEND);

  if (IsChannelName(parv[1]) && (chptr = FindChannel(parv[1]))) {
    if (client_can_send_to_channel(sptr, chptr, 0) && !(chptr->mode.mode & MODE_NONOTICE)) {
      if ((chptr->mode.mode & MODE_NOPRIVMSGS) &&
          check_target_limit(sptr, chptr, chptr->chname, 0))
        return 0;

      /* +cC checks */
      if (chptr->mode.mode & MODE_NOCOLOUR)
        for (ch=parv[parc - 1];*ch;ch++)
          if (*ch==2 || *ch==3 || *ch==22 || *ch==27 || *ch==31) {
            return 0;
          }

      if ((chptr->mode.mode & MODE_NOCTCP) && ircd_strncmp(parv[parc - 1],"\001ACTION ",8))
        for (ch=parv[parc - 1];*ch;)
          if (*ch++==1) {
            return 0;
          }

      sendcmdto_channel_butone(sptr, CMD_WALLCHOPS, chptr, cptr,
			       SKIP_DEAF | SKIP_BURST | SKIP_NONOPS,
			       "%H :@ %s", chptr, parv[parc - 1]);
      if (CapHas(cli_active(sptr), CAP_ECHOMESSAGE))
        sendcmdto_one(sptr, CMD_NOTICE, cli_from(sptr), // Sending CMD_NOTICE since CMD_WALLCHOPS is translated into CMD_NOTICE in sendcmdto_channel_butone()
                "@%H :@ %s", chptr, parv[parc - 1]);				   
    }
    else
      send_reply(sptr, ERR_CANNOTSENDTOCHAN, parv[1]);
  }
  else
    send_reply(sptr, ERR_NOSUCHCHANNEL, parv[1]);

  return 0;
}

/*
 * ms_wallchops - server message handler
 */
int ms_wallchops(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  struct Channel *chptr;
  assert(0 != cptr);
  assert(0 != sptr);

  if (parc < 3 || !IsUser(sptr))
    return 0;

  if (!IsLocalChannel(parv[1]) && (chptr = FindChannel(parv[1]))) {
    if (client_can_send_to_channel(sptr, chptr, 0) && !(chptr->mode.mode & MODE_NONOTICE)) {
      sendcmdto_channel_butone(sptr, CMD_WALLCHOPS, chptr, cptr,
			       SKIP_DEAF | SKIP_BURST | SKIP_NONOPS,
			       "%H :%s", chptr, parv[parc - 1]);
    } else
      send_reply(sptr, ERR_CANNOTSENDTOCHAN, parv[1]);
  }
  return 0;
}
