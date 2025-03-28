/*
 * IRC - Internet Relay Chat, ircd/m_names.c
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
 * $Id: m_names.c,v 1.22.2.3 2007/08/14 03:02:24 entrope Exp $
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
#include <string.h>

/*
 *  Sends a suitably formatted 'names' reply to 'sptr' consisting of nicks within
 *  'chptr', depending on 'filter'.
 *
 *  NAMES_ALL - Lists all users on channel.
 *  NAMES_VIS - Only list visible (-i) users. --Gte (04/06/2000).
 *  NAMES_DEL - Show join-delayed names list.
 *  NAMES_EON - When OR'd with the other two, adds an 'End of Names' numeric
 *              used by m_join
 *
 */

void do_names(struct Client* sptr, struct Channel* chptr, int filter)
{ 
  int mlen;
  int idx;
  int flag;
  int needs_space; 
  int len; 
  char buf[BUFSIZE];
  char anon[BUFSIZE];
  struct Client *c2ptr;
  struct Membership* member;
  
  assert(chptr);
  assert(sptr);
  assert((filter&NAMES_ALL) != (filter&NAMES_VIS));

  /* Tag Pub/Secret channels accordingly. */

  strcpy(buf, "* ");  
  strcpy(anon, "* ");
  if (PubChannel(chptr)) {
    *buf = '=';
	*anon = '=';
  } else if (SecretChannel(chptr)) {
    *buf = '@';
	*anon = '@';
  }
 
  len = strlen(chptr->chname);
  strcpy(anon + 2, chptr->chname);
  strcpy(anon + 2 + len, " :");
  strcpy(buf + 2, chptr->chname);
  strcpy(buf + 2 + len, " :");

  idx = len + 4;
  flag = 1;
  needs_space = 0;

  if (!ShowChannel(sptr, chptr)) /* Don't list private channels unless we are on them. */
    return;

  /* Iterate over all channel members, and build up the list. */

  mlen = strlen(cli_name(&me)) + 10 + strlen(cli_name(sptr));
 
  for (member = chptr->members; member; member = member->next_member)
  {
    c2ptr = member->user;
    if ((chptr->mode.mode & MODE_ANONYMOUS)) {
	  strcpy(anon + idx, "anonymous");
      idx = len + 4;
      flag = 0;
      needs_space=0;	  
      send_reply(c2ptr, RPL_NAMREPLY, anon); 
	  continue;
    }
    if (((filter&NAMES_VIS)!=0) && IsInvisible(c2ptr))
      continue;

    if (IsZombie(member) && member->user != sptr)
      continue;

    if (IsDelayedJoin(member) && (member->user != sptr) && !(filter & NAMES_DEL))
        continue;

    if ((!IsDelayedJoin(member) || (member->user == sptr)) && (filter & NAMES_DEL))
        continue;
    if (needs_space)
      buf[idx++] = ' ';
    needs_space=1;
    if (IsChanService(member))
      buf[idx++] = '!';
    else if (IsChannelManager(member))
      buf[idx++] = '~';
    else if (IsAdmin(member))
      buf[idx++] = '&';
    else if (IsZombie(member))
	  buf[idx++] = '$';
    else if (IsChanOp(member))
      buf[idx++] = '@';
    else if (IsHalfOp(member))
      buf[idx++] = '%';
    else if (HasVoice(member))
      buf[idx++] = '+';
    strcpy(buf + idx, cli_name(c2ptr));
    idx += strlen(cli_name(c2ptr));
    flag = 1;
    if (mlen + idx + NICKLEN + 5 > BUFSIZE)
      /* space, modifier, nick, \r \n \0 */
    {
      send_reply(sptr, (filter & NAMES_DEL) ? RPL_DELNAMREPLY : RPL_NAMREPLY, buf);
      idx = len + 4;
      flag = 0;
      needs_space=0;
    }
  }
  if (flag)
    send_reply(sptr, (filter & NAMES_DEL) ? RPL_DELNAMREPLY : RPL_NAMREPLY, buf); 
  if (filter&NAMES_EON)
    send_reply(sptr, RPL_ENDOFNAMES, chptr->chname);
}

/*
 * m_names - generic message handler
 *
 * parv[0] = sender prefix
 * parv[1] = channel
 */

int m_names(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  struct Channel *chptr; 
  struct Channel *ch2ptr; 
  struct Client *c2ptr;
  struct Membership* member; 
  char* s;
  char* para = parc > 1 ? parv[1] : 0; 
  int showingdelayed = 0;

  if (parc > 1 && !ircd_strcmp(parv[1], "-D")) {
    para = (parc > 2) ? parv[2] : 0;
    showingdelayed = NAMES_DEL;
    if (parc > 3 && hunt_server_cmd(sptr, CMD_NAMES, cptr, 1, "%s %s %C", 3, parc, parv))
      return 0;
  } else if (parc > 2 && hunt_server_cmd(sptr, CMD_NAMES, cptr, 1, "%s %C", 2, parc, parv))
    return 0;

  if (EmptyString(para)) {
    send_reply(sptr, RPL_ENDOFNAMES, "*");
    return 0;
  }

  do {
    s = strchr(para, ',');
    if (s)
      *s++ = '\0';
    /*
     * Special Case 1: "/names 0". 
     * Full list as per RFC. 
     */
    if ((*para == '0') || (*para == '\0'))
    {
      int idx; 
      int mlen;
      int flag;
      struct Channel *ch3ptr;
      char buf[BUFSIZE]; 

      mlen = strlen(cli_name(&me)) + 10 + strlen(cli_name(sptr));

      /* List all visible channels/visible members */ 

      for (ch2ptr = GlobalChannelList; ch2ptr; ch2ptr = ch2ptr->next)
      { 
        if (!ShowChannel(sptr, ch2ptr))
          continue;                 /* Don't show secret chans. */ 
        else if (find_channel_member(sptr, ch2ptr))
          do_names(sptr, ch2ptr, showingdelayed|NAMES_ALL); /* Full list if we're in this chan. */
        else
          do_names(sptr, ch2ptr, showingdelayed|NAMES_VIS);
      } 

      /* List all remaining users on channel '*' */

      strcpy(buf, "* * :");
      idx = 5;
      flag = 0;

      for (c2ptr = GlobalClientList; c2ptr; c2ptr = cli_next(c2ptr))
      {
        int showflag = 0;

        if (!IsUser(c2ptr) || (sptr != c2ptr && IsInvisible(c2ptr)))
          continue;

        member = cli_user(c2ptr)->channel;

        while (member)
        {
          ch3ptr = member->channel;
  
          if (PubChannel(ch3ptr) || find_channel_member(sptr, ch3ptr))
            showflag = 1;
 
          member = member->next_channel;
        }

        if (showflag)               /* Have we already shown them? */
          continue;
 
        strcpy(buf + idx, cli_name(c2ptr));
        idx += strlen(cli_name(c2ptr));
        buf[idx++] = ' ';
        flag = 1;

        if (mlen + idx + NICKLEN + 3 > BUFSIZE)     /* space, \r\n\0 */
        {
          buf[idx-1] = '\0';
          send_reply(sptr, RPL_NAMREPLY, buf);
          strcpy(buf, "* * :");
          idx = 5;
          flag = 0;
        }
      }
      if (flag)
      {
        buf[idx-1] = '\0';
        send_reply(sptr, RPL_NAMREPLY, buf);
      }
      send_reply(sptr, RPL_ENDOFNAMES, "*");
    }
    else if ((chptr = FindChannel(para)) != NULL)
    {
      member = find_member_link(chptr, sptr);
      if (member)
      {
        /*
         *  Special Case 2: User is on this channel, requesting full names list.
         *  (As performed with each /join) - ** High frequency usage **
         */
        do_names(sptr, chptr, showingdelayed|NAMES_ALL|NAMES_EON);
      }
      else
      {
        /*
         *  Special Case 3: User isn't on this channel, show all visible users, in 
         *  non secret channels.
         */ 
        do_names(sptr, chptr, showingdelayed|NAMES_VIS|NAMES_EON);
      } 
    }
    else
        send_reply(sptr, RPL_ENDOFNAMES, para);
  } while ((para = s) != NULL);

  return 1;
}
