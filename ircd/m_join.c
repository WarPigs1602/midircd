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
 * $Id: m_join.c,v 1.34.2.10 2007/02/25 14:32:41 entrope Exp $
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

/** Searches for and handles a 0 in a join list.
 * @param[in] cptr Client that sent us the message.
 * @param[in] sptr Original source of message.
 * @param[in] chanlist List of channels to join.
 * @return First token in \a chanlist after the final 0 entry, which
 * may be its nul terminator (if the final entry is a 0 entry).
 */
static char *
last0(struct Client *cptr, struct Client *sptr, char *chanlist)
{
  char *p;
  int join0 = 0;

  for (p = chanlist; p[0]; p++) /* find last "JOIN 0" */
    if (p[0] == '0' && (p[1] == ',' || p[1] == '\0')) {
      if (p[1] == ',')
        p++;
      chanlist = p + 1;
      join0 = 1;
    } else {
      while (p[0] != ',' && p[0] != '\0') /* skip past channel name */
	p++;

      if (!p[0]) /* hit the end */
	break;
    }

  if (join0) {
    struct JoinBuf part;
    struct Membership *member;

    joinbuf_init(&part, sptr, cptr, JOINBUF_TYPE_PARTALL,
                 "Left all channels", 0);

    joinbuf_join(&part, 0, 0);

    while ((member = cli_user(sptr)->channel))
      joinbuf_join(&part, member->channel,
                   IsZombie(member) ? CHFL_ZOMBIE :
                   IsDelayedJoin(member) ? CHFL_DELAYED :
                   0);

    joinbuf_flush(&part);
  }

  return chanlist;
}

static int is_link_loop(const char* start, const char* target) {
    const int max_depth = 16; // Prevent excessive recursion
    int depth = 0;
    struct Channel* chptr = FindChannel(target);
    while (chptr && (chptr->mode.mode & MODE_LINK) && *chptr->mode.linktarget) {
        if (strcmp(chptr->chname, start) == 0)
            return 1; // Loop detected
        chptr = FindChannel(chptr->mode.linktarget);
        if (++depth > max_depth)
            return 1; // Too deep, treat as loop
    }
    return 0;
}

/** Handle a JOIN message from a client connection.
 * See @ref m_functions for discussion of the arguments.
 * @param[in] cptr Client that sent us the message.
 * @param[in] sptr Original source of message.
 * @param[in] parc Number of arguments.
 * @param[in] parv Argument vector.
 */
int m_join(struct Client *cptr, struct Client *sptr, int parc, char *parv[])
{
  struct Channel *chptr;
  struct JoinBuf join;
  struct JoinBuf create;
  struct Gline *gline;
  char *p = 0;
  char *chanlist;
  char *name;
  char *keys;

  if (parc < 2 || *parv[1] == '\0')
    return need_more_params(sptr, "JOIN");

  joinbuf_init(&join, sptr, cptr, JOINBUF_TYPE_JOIN, 0, 0);
  joinbuf_init(&create, sptr, cptr, JOINBUF_TYPE_CREATE, 0, TStime());

  chanlist = last0(cptr, sptr, parv[1]); /* find last "JOIN 0" */

  keys = parv[2]; /* remember where keys are */

  for (name = ircd_strtok(&p, chanlist, ","); name;
       name = ircd_strtok(&p, 0, ",")) {

      // --- LINK mode check and error ---
      chptr = FindChannel(name);
      if (chptr && !IsInvited(sptr, chptr) && IsGlobalChannel(name) && (chptr->mode.mode & MODE_LINK) && *chptr->mode.linktarget) {
          // Prevent infinite loop: check for cycles
          if (is_link_loop(chptr->chname, chptr->mode.linktarget)) {
              send_reply(sptr, RPL_INFO, "LINK mode would cause a loop, join denied");
              continue;
          }
          // Inform the user and do not join
          send_reply(sptr, ERR_LINKCHANNEL, name, "Channel is full +l", chptr->mode.linktarget);
		  name = chptr->mode.linktarget; // Redirect to the linked channel
      }

      if (IsNetworkChannel(name)) {
          char visible_name[CHANNELLEN + 1];
          char channel_list[1024];
          // Extract visible channel name (without ! and ID)
          strncpy(visible_name, name + 1 + NETWORK_ID_LEN, sizeof(visible_name) - 1);
          visible_name[sizeof(visible_name) - 1] = '\0';

          // Get all network channels with the same visible name
          get_network_channel_list(visible_name, channel_list, sizeof(channel_list));

          // Count how many channels exist
          int count = 0;
          char* ptr = channel_list;
          while (*ptr) {
              count++;
              ptr = strchr(ptr, ',');
              if (ptr) ptr++;
              else break;
          }

          if (count == 0) {
              // Create a new network channel name
              char newname[CHANNELLEN + 1];
              create_network_channel(name, newname, sizeof(newname));
              ircd_strncpy(name, newname, CHANNELLEN);
          } else
          if (count > 1) {
              // Inform the user about the available options
              send_reply(sptr, RPL_INFO, "Multiple network channels with name '%s' exist: %s", visible_name, channel_list);
              continue; // Skip joining until user selects a channel
          }
          if (count == 1) {
              // Use the first channel name from the list
              char* comma = strchr(channel_list, ',');
              if (comma) *comma = '\0';
              ircd_strncpy(name, channel_list, CHANNELLEN);
          }
      }
      else {
          const char* redirect = get_renamed_channel(name);
          if (redirect) {
              name = (char*)redirect;
          }
      }
    // If no channel exists, proceed as usual (creation logic)
    char *key = 0;

    /* If we have any more keys, take the first for this channel. */
    if (!BadPtr(keys)
        && (keys = strchr(key = keys, ',')))
      *keys++ = '\0';

    /* Empty keys are the same as no keys. */
    if (key && !key[0])
      key = 0;

    if (!IsChannelName(name) || !strIsIrcCh(name))
    {
      /* bad channel name */
      send_reply(sptr, ERR_NOSUCHCHANNEL, name);
      continue;
    }
    const char* renamed = get_renamed_channel(name);
    if (renamed) {
        send_reply(sptr, RPL_INFO, "Channel %s was renamed to %s, joining new channel.", name, renamed);
        name = (char*)renamed;
    }

    if (cli_user(sptr)->joined >= feature_int(FEAT_MAXCHANNELSPERUSER)
	&& !HasPriv(sptr, PRIV_CHAN_LIMIT)) {
      send_reply(sptr, ERR_TOOMANYCHANNELS, name);
      break; /* no point processing the other channels */
    }

    /* BADCHANed channel */
    if (!IsAnOper(sptr) 
        && (gline = gline_lookup_badchan(name, GLINE_BADCHAN))) {
      send_reply(sptr, ERR_BADCHANNAME, name, gline->gl_reason);
      continue;
    }

    if (!(chptr = FindChannel(name))) {
      if (((name[0] == '&') && !feature_bool(FEAT_LOCAL_CHANNELS))
          || strlen(name) >= IRCD_MIN(CHANNELLEN, feature_int(FEAT_CHANNELLEN))) {
        send_reply(sptr, ERR_NOSUCHCHANNEL, name);
        continue;
      }

      if (!(chptr = get_channel(sptr, name, CGT_CREATE)))
        continue;

      /* Try to add the new channel as a recent target for the user. */
      if (check_target_limit(sptr, chptr, chptr->chname, 0)) {
        chptr->members = 0;
        destruct_channel(chptr);
        continue;
      }

      joinbuf_join(&create, chptr, CHFL_OWNER | CHFL_ADMIN | CHFL_CHANOP | CHFL_CHANNEL_MANAGER);
      if (feature_bool(FEAT_AUTOCHANMODES) && feature_str(FEAT_AUTOCHANMODES_LIST) && strlen(feature_str(FEAT_AUTOCHANMODES_LIST)) > 0)
        SetAutoChanModes(chptr);
    } else if (find_member_link(chptr, sptr)) {
      continue; /* already on channel */
    } else if (check_target_limit(sptr, chptr, chptr->chname, 0)) {
      continue;
    } else {
        int flags = CHFL_DEOPPED;
        int err = 0;
        int ignore_restrictions = 0;
        if (chptr && (chptr->mode.mode & MODE_BURSTADDED))
            ignore_restrictions = 1;
        if (IsChannelService(sptr)) {
            flags |= CHFL_CHANSERVICE;
            // Services should not get further user modes!
        }
      /* Check Apass/Upass -- since we only ever look at a single
       * "key" per channel now, this hampers brute force attacks. */
      if (key && !strcmp(key, chptr->mode.apass))
        flags = CHFL_CHANOP | CHFL_CHANNEL_MANAGER;
      else if (key && !strcmp(key, chptr->mode.upass))
        flags = CHFL_CHANOP;
      else if (chptr->users == 0 && !chptr->mode.apass[0]) {
        /* Joining a zombie channel (zannel): give ops and increment TS. */
        flags = CHFL_CHANOP;
        chptr->creationtime++;
      } else if (IsInvited(sptr, chptr)) {
        /* Invites bypass these other checks. */
      }
      else if (!ignore_restrictions) {
          if (chptr->mode.mode & MODE_INVITEONLY)
              err = ERR_INVITEONLYCHAN;
          else if (chptr->mode.limit && (chptr->users >= chptr->mode.limit))
              err = ERR_CHANNELISFULL;
          else if ((chptr->mode.mode & MODE_REGONLY) && !IsAccount(sptr))
              err = ERR_NEEDREGGEDNICK;
          else if (find_ban(sptr, chptr->banlist, chptr->ban_exceptions))
              err = ERR_BANNEDFROMCHAN;
          else if (*chptr->mode.key && (!key || strcmp(key, chptr->mode.key)))
              err = ERR_BADCHANNELKEY;
          else if ((chptr->mode.mode & MODE_TLSONLY) && !IsTLS(sptr))
              err = ERR_TLSONLYCHAN;
      }

      /*
       * ASUKA_X:
       * Allow XtraOpers to join all channels.
       * --Bigfoot
       */
      if (IsXtraOp(sptr))
        err = 0;
      
      /* An oper with WALK_LCHAN privilege can join a local channel
       * he otherwise could not join by using "OVERRIDE" as the key.
       * This will generate a HACK(4) notice, but fails if the oper
       * could normally join the channel. */
      if (IsLocalChannel(chptr->chname)
          && HasPriv(sptr, PRIV_WALK_LCHAN)
          && !(flags & CHFL_CHANOP)
          && key && !strcmp(key, "OVERRIDE"))
      {
        switch (err) {
        case 0:
          if (strcmp(chptr->mode.key, "OVERRIDE")
              && strcmp(chptr->mode.apass, "OVERRIDE")
              && strcmp(chptr->mode.upass, "OVERRIDE")) {
            send_reply(sptr, ERR_DONTCHEAT, chptr->chname);
            continue;
          }
          break;
        case ERR_INVITEONLYCHAN: err = 'i'; break;
        case ERR_CHANNELISFULL:  err = 'l'; break;
        case ERR_BANNEDFROMCHAN: err = 'b'; break;
        case ERR_BADCHANNELKEY:  err = 'k'; break;
        case ERR_NEEDREGGEDNICK: err = 'r'; break;
        case ERR_TLSONLYCHAN:    err = 'Z'; break;
        default: err = '?'; break;
        }
        /* send accountability notice */
        if (err)
          sendto_opmask_butone(0, SNO_HACK4, "OPER JOIN: %C JOIN %H "
                               "(overriding +%c)", sptr, chptr, err);
        err = 0;
      }

      /* Is there some reason the user may not join? */
      if (err) {
        switch(err) {
          case ERR_NEEDREGGEDNICK:
            send_reply(sptr, 
                       ERR_NEEDREGGEDNICK, 
                       chptr->chname, 
                       feature_str(FEAT_URLREG));            
            break;
          default:
            send_reply(sptr, err, chptr->chname);
            break;
        }
        continue;
      }

      joinbuf_join(&join, chptr, flags);
      // For services: set mode, but avoid duplicate commands!
      if (IsChannelService(sptr) || IsService(sptr)) {
          struct ModeBuf mbuf;
          modebuf_init(&mbuf, &me, cptr, chptr, MODEBUF_DEST_SERVER);
          modebuf_mode_client(&mbuf, MODE_ADD | MODE_CHANSERVICE, sptr, 0);
          modebuf_flush(&mbuf);
      }
      // For normal users: set Chanop
      else if (flags & (CHFL_CHANSERVICE | CHFL_OWNER | CHFL_ADMIN | CHFL_CHANOP | CHFL_HALFOP | CHFL_VOICE)) {
          struct ModeBuf mbuf;
          modebuf_init(&mbuf, &me, cptr, chptr, MODEBUF_DEST_SERVER);
          if (flags & CHFL_CHANSERVICE)
              modebuf_mode_client(&mbuf, MODE_ADD | MODE_CHANSERVICE, sptr, 0);
          if (flags & CHFL_OWNER)
              modebuf_mode_client(&mbuf, MODE_ADD | MODE_OWNER, sptr, 0);
          if (flags & CHFL_ADMIN)
              modebuf_mode_client(&mbuf, MODE_ADD | MODE_ADMIN, sptr, 0);
          if (flags & CHFL_CHANOP)
              modebuf_mode_client(&mbuf, MODE_ADD | MODE_CHANOP, sptr,
                  chptr->mode.apass[0] ? ((flags & CHFL_CHANNEL_MANAGER) ? 0 : 1) : MAXOPLEVEL);
          if (flags & CHFL_HALFOP)
              modebuf_mode_client(&mbuf, MODE_ADD | MODE_HALFOP, sptr, 0);
          if (flags & CHFL_VOICE)
              modebuf_mode_client(&mbuf, MODE_ADD | MODE_VOICE, sptr, 0);
          modebuf_flush(&mbuf);
      }
    }

    del_invite(sptr, chptr);

    if (chptr->topic[0]) {
      send_reply(sptr, RPL_TOPIC, chptr->chname, chptr->topic);
      send_reply(sptr, RPL_TOPICWHOTIME, chptr->chname, chptr->topic_nick,
		 chptr->topic_time);
    }

    do_names(sptr, chptr, NAMES_ALL|NAMES_EON); /* send /names list */
  }

  joinbuf_flush(&join); /* must be first, if there's a JOIN 0 */
  joinbuf_flush(&create);

  return 0;
}

/** Handle a JOIN message from a server connection.
 * See @ref m_functions for discussion of the arguments.
 * @param[in] cptr Client that sent us the message.
 * @param[in] sptr Original source of message.
 * @param[in] parc Number of arguments.
 * @param[in] parv Argument vector.
 */
int ms_join(struct Client *cptr, struct Client *sptr, int parc, char *parv[])
{
  struct Membership *member;
  struct Channel *chptr;
  struct JoinBuf join;
  unsigned int flags;
  time_t creation = 0;
  char *p = 0;
  char *chanlist;
  char *name;

  if (IsServer(sptr))
  {
    return protocol_violation(cptr,
                              "%s tried to JOIN %s, duh!",
                              cli_name(sptr),
                              (parc < 2 || *parv[1] == '\0') ? "a channel" :
                                                               parv[1]
                              );
  }

  if (parc < 2 || *parv[1] == '\0')
    return need_more_params(sptr, "JOIN");

  if (parc > 2 && parv[2])
    creation = atoi(parv[2]);

  joinbuf_init(&join, sptr, cptr, JOINBUF_TYPE_JOIN, 0, 0);

  chanlist = last0(cptr, sptr, parv[1]); /* find last "JOIN 0" */

  for (name = ircd_strtok(&p, chanlist, ","); name;
       name = ircd_strtok(&p, 0, ",")) {
      const char* redirect = get_renamed_channel(name);
      if (redirect) {
          name = (char*)redirect;
      }
      chptr = FindChannel(name);
      // --- LINK mode check and error for server joins ---
      if (chptr && (chptr->mode.mode & MODE_LINK) && *chptr->mode.linktarget) {
          // Prevent infinite loop: check for cycles
          if (is_link_loop(chptr->chname, chptr->mode.linktarget)) {
              // Option: send error or ignore join
              continue;
          }
		  name = chptr->mode.linktarget; // Redirect to the linked channel
      }

      flags = CHFL_DEOPPED;

    if (IsLocalChannel(name) || !IsChannelName(name))
    {
      protocol_violation(cptr, "%s tried to join %s", cli_name(sptr), name);
      continue;
    }

    if (!(chptr = FindChannel(name)))
    {
      /* No channel exists, so create one */
      if (!(chptr = get_channel(sptr, name, CGT_CREATE)))
      {
        protocol_violation(sptr,"couldn't get channel %s for %s",
        		   name,cli_name(sptr));
      	continue;
      }
      flags |= HasFlag(sptr, FLAG_TS8) ? CHFL_SERVOPOK : 0;

      chptr->creationtime = creation;
    }
    else { /* We have a valid channel? */
      if ((member = find_member_link(chptr, sptr)))
      {
	/* It is impossible to get here --Run */
	if (!IsZombie(member)) /* already on channel */
	  continue;

	flags = member->status & (CHFL_DEOPPED | CHFL_SERVOPOK);
	remove_user_from_channel(sptr, chptr);
	chptr = FindChannel(name);
      }
      else
        flags |= HasFlag(sptr, FLAG_TS8) ? CHFL_SERVOPOK : 0;
      /* Always copy the timestamp when it is older, that is the only way to
         ensure network-wide synchronization of creation times.
         We now also copy a creation time that only 1 second younger...
         this is needed because the timestamp must be incremented
         by one when someone joins an existing, but empty, channel.
         However, this is only necessary when the channel is still
         empty (also here) and when this channel doesn't have +A set.

         To prevent this from allowing net-rides on the channel, we
         clear all modes from the channel.

         (Scenario for a net ride: c1 - s1 - s2 - c2, with c1 the only
         user in the channel; c1 parts and rejoins, gaining ops.
         Before s2 sees c1's part, c2 joins the channel and parts
         immediately.  s1 sees c1 part, c1 create, c2 join, c2 part;
         c2's join resets the timestamp.  s2 sees c2 join, c2 part, c1
         part, c1 create; but since s2 sees the channel as a zannel or
         non-existent, it does not bounce the create with the newer
         timestamp.)
      */
      if (creation && (creation < chptr->creationtime ||
		       (!chptr->mode.apass[0] && chptr->users == 0))) {
        struct Membership *member;
        struct ModeBuf mbuf;

	chptr->creationtime = creation;
        /* Wipe out the current modes on the channel. */
        modebuf_init(&mbuf, sptr, cptr, chptr, MODEBUF_DEST_CHANNEL | MODEBUF_DEST_HACK3);

        modebuf_mode(&mbuf, MODE_DEL | chptr->mode.mode);
        chptr->mode.mode &= MODE_BURSTADDED | MODE_WASDELJOINS;

        if (chptr->mode.limit) {
          modebuf_mode_uint(&mbuf, MODE_DEL | MODE_LIMIT, chptr->mode.limit);
          chptr->mode.limit = 0;
        }

        if (chptr->mode.key[0]) {
          modebuf_mode_string(&mbuf, MODE_DEL | MODE_KEY, chptr->mode.key, 0);
          chptr->mode.key[0] = '\0';
        }

        if (chptr->mode.upass[0]) {
          modebuf_mode_string(&mbuf, MODE_DEL | MODE_UPASS, chptr->mode.upass, 0);
          chptr->mode.upass[0] = '\0';
        }

        if (chptr->mode.apass[0]) {
          modebuf_mode_string(&mbuf, MODE_DEL | MODE_APASS, chptr->mode.apass, 0);
          chptr->mode.apass[0] = '\0';
        }

        for (member = chptr->members; member; member = member->next_member)
        {
            if (IsChanService(member)) {
                modebuf_mode_client(&mbuf, MODE_DEL | MODE_CHANSERVICE, member->user, OpLevel(member));
                member->status &= ~CHFL_CHANSERVICE;
            }
            if (IsOwner(member)) {
                modebuf_mode_client(&mbuf, MODE_DEL | MODE_OWNER, member->user, OpLevel(member));
                member->status &= ~CHFL_OWNER;
            }
            if (IsAdmin(member)) {
                modebuf_mode_client(&mbuf, MODE_DEL | MODE_ADMIN, member->user, OpLevel(member));
                member->status &= ~CHFL_ADMIN;
            }
            if (IsChanOp(member)) {
                modebuf_mode_client(&mbuf, MODE_DEL | MODE_CHANOP, member->user, OpLevel(member));
                member->status &= ~CHFL_CHANOP;
            }
            if (IsHalfOp(member)) {
                modebuf_mode_client(&mbuf, MODE_DEL | MODE_HALFOP, member->user, OpLevel(member));
                member->status &= ~CHFL_HALFOP;
            }
            if (HasVoice(member)) {
                modebuf_mode_client(&mbuf, MODE_DEL | MODE_VOICE, member->user, OpLevel(member));
                member->status &= ~CHFL_VOICE;
            }
        }
        modebuf_flush(&mbuf);
      }
    }

    joinbuf_join(&join, chptr, flags);
  }

  joinbuf_flush(&join); /* flush joins... */

  return 0;
}
