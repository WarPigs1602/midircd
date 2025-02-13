/*
 * IRC - Internet Relay Chat, ircd/m_join.c
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
 * $Id: m_rename.c,v 1.0.0.0 2025/02/02 18:41:00 entrope Exp $
 */

#include "config.h"

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
#include <stdlib.h>
#include <string.h>

/** Handle a RENAME message from a client connection.
 * See @ref m_functions for discussion of the arguments.
 * @param[in] cptr Client that sent us the message.
 * @param[in] sptr Original source of message.
 * @param[in] parc Number of arguments.
 * @param[in] parv Argument vector.
 */
int m_rename(struct Client *cptr, struct Client *sptr, int parc, char *parv[])
{
  struct Membership *member, *nmember;  
  struct Channel *chptr, *chptr2, *nchptr;
  
  struct RenamedChan *ren;
  struct Client *c2ptr;
  char *name;
  char *target;
  char *reason; 
  if (parc < 3 || *parv[1] == '\0')
    return need_more_params(sptr, "RENAME");
  name = parv[1];
  target = parv[2];
  reason = EmptyString(parv[parc - 1]) ? parv[0] : parv[parc - 1];
  if (!IsChannelName(target) || !strIsIrcCh(target))
  {
      /* bad channel name */
      send_reply(sptr, ERR_NOSUCHCHANNEL, target);
      return 0;
  }
  int flags = CHFL_DEOPPED;
  chptr = FindChannel(target);
  chptr2 = FindChannel(name);
  if (!IsChannelName(name) || !chptr2) {
	 if(CapHas(cli_active(sptr), CAP_STANDARDREPLYS))
		sendfailto_one(sptr, &me, "RENAME", "CANNOT_RENAME", ":The requesting channel %s doesn't exists", name); 
	 else
		send_reply(sptr, ERR_NOSUCHCHANNEL, name);
  } else if (FindRenamed(name)) {
	if(CapHas(cli_active(sptr), CAP_STANDARDREPLYS))
		sendfailto_one(sptr, &me, "RENAME", "CANNOT_RENAME", ":%s channel has been renamed recently", name);  
	 else
		send_reply(sptr, ERR_LINKSET, name, target, "Channel has been renamed recently", name);
  } else if (IsChannelName(target) && chptr) {
    if(CapHas(cli_active(sptr), CAP_STANDARDREPLYS))
		sendfailto_one(sptr, &me, "RENAME", "CHANNEL_NAME_IN_USE", ":Channel %s already exists", target);  
	 else
		send_reply(sptr, ERR_LINKSET, name, target, "Channel already exists");
 } else if (name[0] != target[0]) {
	 if(CapHas(cli_active(sptr), CAP_STANDARDREPLYS))
		sendfailto_one(sptr, &me, "RENAME", "CANNOT_RENAME", ":You cannot change a channel prefix type"); 
	 else
		send_reply(sptr, ERR_LINKSET, name, target, "You cannot change a channel prefix type");
  } else {
	if (!(member = find_member_link(chptr2, sptr)) || IsZombie(member)
          || !IsChanOp(member))
      return send_reply(sptr, ERR_CHANOPRIVSNEEDED, name);
	if(!(ren = get_renamed(sptr, name, CGT_CREATE))) {
      return 0;		
	}
	ircd_strncpy(ren->newname, target, CHANNELLEN + 1);
	ircd_strncpy(ren->reason, reason, BUFSIZE + 1);
	if(!chptr2) {
		chptr2 = get_channel(sptr, name, CGT_CREATE);
	}
	if(!chptr) {
		chptr = get_channel(sptr, target, CGT_CREATE);
	}
    for (member=chptr2->members;member;member=nmember) {
        nmember=member->next_member;
		c2ptr = member->user;
        if (!MyUser(c2ptr) || IsZombie(member) || IsAnOper(c2ptr))
          continue;		
		if (IsChanOp(member))
			flags = CHFL_CHANOP;
		else if (HasVoice(member))
			flags = CHFL_VOICE;
		struct JoinBuf join;
		struct JoinBuf parts;
		joinbuf_init(&parts, c2ptr, cptr, JOINBUF_TYPE_PART, reason,
	       0);		
		joinbuf_init(&join, c2ptr, cptr, JOINBUF_TYPE_JOIN, 0, 0); 
		chptr->topic_time = chptr2->topic_time;		
		ircd_strncpy(chptr->topic, chptr2->topic, TOPICLEN);
		ircd_strncpy(chptr->topic_nick, chptr2->topic_nick, TOPICLEN); 
		chptr->mode = chptr2->mode;	    
		joinbuf_join(&parts, chptr2, flags); /* part client from channel */	
		joinbuf_join(&join, chptr, flags);
		if (IsChanOp(member) && HasVoice(member)) {
			sendcmdto_channel_butserv_butone(&his, CMD_MODE, chptr, cptr, 0,
				"%H +ov %C %C", chptr, c2ptr, c2ptr);
		} else if (IsChanOp(member) || HasVoice(member)) {
			sendcmdto_channel_butserv_butone(&his, CMD_MODE, chptr, cptr, 0,
				"%H +%c %C", chptr, IsChanOp(member) ? 'o' : 'v', c2ptr);
		}		
		if (chptr->topic[0]) {
			send_reply(c2ptr, RPL_TOPIC, chptr->chname, chptr->topic);
			send_reply(c2ptr, RPL_TOPICWHOTIME, chptr->chname, chptr->topic_nick,
				chptr->topic_time);
		}
		do_names(c2ptr, chptr, NAMES_ALL|NAMES_EON);
		joinbuf_flush(&parts);
		joinbuf_flush(&join);	
	}	
  } 
  return 0;
}

/** Handle a RENAME message from a server connection.
 * See @ref m_functions for discussion of the arguments.
 * @param[in] cptr Client that sent us the message.
 * @param[in] sptr Original source of message.
 * @param[in] parc Number of arguments.
 * @param[in] parv Argument vector.
 */
int ms_rename(struct Client *cptr, struct Client *sptr, int parc, char *parv[])
{
  struct Membership *member;
  struct Channel *chptr;
  char *p = 0;
  char *chanlist;
  char *name;

  if (IsServer(sptr))
  {
    return protocol_violation(cptr,
                              "%s tried to RENAME %s, duh!",
                              cli_name(sptr),
                              (parc < 3 || *parv[1] == '\0') ? "a channel" :
                                                               parv[1]
                              );
  }

  if (parc < 3 || *parv[1] == '\0')
    return need_more_params(sptr, "RE");
  return 0;
}
