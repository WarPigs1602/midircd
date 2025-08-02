/*
 * IRC - Internet Relay Chat, ircd/m_create.c
 * Copyright (C) 1990 Jarkko Oikarinen and
 *                    University of Oulu, Computing Center
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
 * $Id: m_create.c,v 1.18.2.4 2007/01/13 18:45:08 entrope Exp $
 */

#include "config.h"

#include "channel.h"
#include "client.h"
#include "hash.h"
#include "ircd.h"
#include "ircd_log.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "ircd_snprintf.h"
#include "msg.h"
#include "numeric.h"
#include "numnicks.h"
#include "s_debug.h"
#include "s_misc.h"
#include "s_user.h"
#include "send.h"

#include <stdlib.h>
#include <string.h>

/*
 * ms_create - server message handler
 */
int ms_create(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  time_t chanTS; /* channel creation time */
  char *p; /* strtok state */
  char *name; /* channel name */
  struct Channel *chptr; /* channel */
  struct JoinBuf join; /* join and create buffers */
  struct JoinBuf create;
  struct ModeBuf mbuf; /* a mode buffer */
  int badop; /* a flag */

  if (IsServer(sptr))
    return protocol_violation(sptr,"%s tried to CREATE a channel", cli_name(sptr));

  /* sanity checks: Only accept CREATE messages from servers */
  if (parc < 3 || *parv[2] == '\0')
    return need_more_params(sptr,"CREATE");

  chanTS = atoi(parv[2]);

  if (!IsBurstOrBurstAck(sptr) && 0 != chanTS)
    cli_serv(cli_user(sptr)->server)->lag = TStime() - chanTS;

  if (TStime() - chanTS<-60)
  {
    static time_t rate;
    sendto_opmask_butone_ratelimited(0, SNO_NETWORK, &rate,
                                     "Timestamp drift from %C (%is); issuing "
                                     "SETTIME to correct this",
				     cli_user(sptr)->server,
				     chanTS - TStime());
    sendcmdto_prio_one(&me, CMD_SETTIME, cli_user(sptr)->server,
                       "%Tu %C", TStime(), cli_user(sptr)->server);
  }

  joinbuf_init(&join, sptr, cptr, JOINBUF_TYPE_JOIN, 0, 0);
  joinbuf_init(&create, sptr, cptr, JOINBUF_TYPE_CREATE, 0, chanTS);

  /* For each channel in the comma separated list: */
  for (name = ircd_strtok(&p, parv[1], ","); name;
       name = ircd_strtok(&p, 0, ",")) {
    badop = 0;

    if (IsLocalChannel(name))
      continue;

    /* If this is a network-secure channel without a valid ID, generate one */
    /* Network-secure channel: check if ID already exists */
    if (name[0] == '!' && (!IsNetSecureChannelName(name) || strlen(name) < 7)) {
        char idbuf[6];
        char newname[CHANNELLEN + 1];
        GenerateNetSecureChannelID(idbuf, sizeof(idbuf));
        ircd_snprintf(0, newname, sizeof(newname), "!%s%s", idbuf, name + 1);
        name = strdup(newname);
    }
    else if (IsNetSecureChannelName(name)) {
        const char* id = NetSecureChannelID(name);
        struct Channel* ch;
        int found = 0;
        for (ch = GlobalChannelList; ch; ch = ch->next) {
            if (IsNetSecureChannelName(ch->chname)) {
                const char* other_id = NetSecureChannelID(ch->chname);
                if (other_id && id && strcmp(id, other_id) == 0) {
                    /* ID exists, join that channel instead */
                    joinbuf_join(&join, ch, CHFL_OWNER);
                    found = 1;
                    break;
                }
            }
        }
        if (found)
            continue;
    }

    if ((chptr = FindChannel(name)))
    {
      if (find_member_link(chptr, sptr)) {
        protocol_violation(sptr, "%s tried to CREATE a channel already joined (%s)", cli_name(sptr), chptr->chname);
        continue;
      }

      if (TStime() - chanTS > TS_LAG_TIME ||
	  (chptr->creationtime && chanTS > chptr->creationtime &&
	   !(chptr->users == 0 && !chptr->mode.apass[0]))) {
        if (!IsBurstOrBurstAck(cli_user(sptr)->server)) {
          modebuf_init(&mbuf, sptr, cptr, chptr,
                       (MODEBUF_DEST_SERVER |
                        MODEBUF_DEST_HACK2  |
                        MODEBUF_DEST_BOUNCE));

          modebuf_mode_client(&mbuf, MODE_ADD | MODE_OWNER, sptr, MAXOPLEVEL + 1);

          modebuf_flush(&mbuf);

          badop = 1;
        } else if (chanTS > chptr->creationtime + 4) {
          badop = 1;
        }

        if (badop)
          joinbuf_join(&join, chptr, 0);
      }
    }
    else /* Channel doesn't exist: create it */
      chptr = get_channel(sptr, name, CGT_CREATE);

    if (!badop) {
      chptr->creationtime = chanTS;
      joinbuf_join(&create, chptr, CHFL_OWNER);
    }
  }

  joinbuf_flush(&join);
  joinbuf_flush(&create);

  return 0;
}
