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
#include "ircd_snprintf.h"
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
#include <time.h>

/** Searches for and handles a 0 in a join list.
 * @param[in] cptr Client that sent us the message.
 * @param[in] sptr Original source of message.
 * @param[in] chanlist List of channels to join.
 * @return First token in \a chanlist after the final 0 entry, which
 * may be its nul terminator (if the final entry is a 0 entry).
 */
static char*
last0(struct Client* cptr, struct Client* sptr, char* chanlist)
{
	char* p;
	int join0 = 0;

	for (p = chanlist; p[0]; p++) /* find last "JOIN 0" */
		if (p[0] == '0' && (p[1] == ',' || p[1] == '\0')) {
			if (p[1] == ',')
				p++;
			chanlist = p + 1;
			join0 = 1;
		}
		else {
			while (p[0] != ',' && p[0] != '\0') /* skip past channel name */
				p++;

			if (!p[0]) /* hit the end */
				break;
		}

	if (join0) {
		struct JoinBuf part;
		struct Membership* member;

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

/** Handle a JOIN message from a client connection.
 * See @ref m_functions for discussion of the arguments.
 * @param[in] cptr Client that sent us the message.
 * @param[in] sptr Original source of message.
 * @param[in] parc Number of arguments.
 * @param[in] parv Argument vector.
 */
int m_join(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
	struct Channel* chptr;
	struct JoinBuf join;
	struct JoinBuf create;
	struct Gline* gline;
	char* p = 0;
	char* chanlist;
	const char* name;
	char* keys;

	if (parc < 2 || *parv[1] == '\0')
		return need_more_params(sptr, "JOIN");

	joinbuf_init(&join, sptr, cptr, JOINBUF_TYPE_JOIN, 0, 0);
	joinbuf_init(&create, sptr, cptr, JOINBUF_TYPE_CREATE, 0, TStime());

	chanlist = last0(cptr, sptr, parv[1]); /* find last "JOIN 0" */

	keys = parv[2]; /* remember where keys are */

	for (name = ircd_strtok(&p, chanlist, ","); name; name = ircd_strtok(&p, 0, ",")) {
		char* key = 0;
		int submitted = 0;
		/* --- Network-secure channel handling --- */
		if (name[0] == '!') {
			if (!IsNetSecureChannelName(name) || strlen(name) < 7) {
				// No valid ID: check if channels with the same suffix exist
				struct Channel* ch;
				struct Channel* found_channels[10];
				int found_count = 0;
				for (ch = GlobalChannelList; ch; ch = ch->next) {
					if (IsNetSecureChannelName(ch->chname)) {
						if (strcmp(ch->chname + 6, name + 1) == 0) {
							if (found_count < 10)
								found_channels[found_count++] = ch;
						}
					}
				}
				if (found_count == 1) {
					// Only one match, join it
					if (HasCap(sptr, CAP_STANDARDREPLIES)) {
						send_note_reply(&me, sptr, "INFO", cli_name(sptr),
							"A network-secure channel with this name already exists: %s. Please join this channel.", found_channels[0]->chname);
					}
					else {
						sendrawto_one(sptr, ":%s NOTICE %s :A network-secure channel with this name already exists: %s. Please join this channel.",
							cli_name(&me), cli_name(sptr), found_channels[0]->chname);
					}
					name = found_channels[0]->chname;
				}
				else if (found_count > 1) {
					// Multiple matches, show all and pick the first
					char listbuf[512] = "";
					int i;
					for (i = 0; i < found_count; ++i) {
						if (i > 0) strncat(listbuf, ", ", sizeof(listbuf));
						size_t len = strlen(listbuf);
						size_t remain = sizeof(listbuf) - len - 1;
						if (remain > 0)
							strncat(listbuf, found_channels[i]->chname, remain);
					}
					if (HasCap(sptr, CAP_STANDARDREPLIES)) {
						send_note_reply(&me, sptr, "INFO", cli_name(sptr),
							"Multiple network-secure channels with this name exist: %s", listbuf);
						send_note_reply(&me, sptr, "INFO", cli_name(sptr),
							"Joining the first: %s", found_channels[0]->chname);
					}
					else {
						sendrawto_one(sptr, ":%s NOTICE %s :Multiple network-secure channels with this name exist: %s",
							cli_name(&me), cli_name(sptr), listbuf);
						sendrawto_one(sptr, ":%s NOTICE %s :Joining the first: %s",
							cli_name(&me), cli_name(sptr), found_channels[0]->chname);
					}
					name = found_channels[0]->chname;
				}
				else {
					// No matching channel, generate a new ID
					char idbuf[6];
					char newname[CHANNELLEN + 1];
					GenerateNetSecureChannelID(idbuf, sizeof(idbuf));
					ircd_snprintf(0, newname, sizeof(newname), "!%s%s", idbuf, name + 1);
					name = strdup(newname);
					if (HasCap(sptr, CAP_STANDARDREPLIES)) {
						send_note_reply(&me, sptr, "INFO", cli_name(sptr), "Generated network-secure channel name: %s", name);
					}
					else {
						sendrawto_one(sptr, ":%s NOTICE %s :Generated network-secure channel name: %s",
							cli_name(&me), cli_name(sptr), name);
					}
				}
			}
			else {
				// Has ID: check if channel with this ID exists
				const char* id = NetSecureChannelID(name);
				struct Channel* ch;
				int found = 0;
				for (ch = GlobalChannelList; ch; ch = ch->next) {
					if (IsNetSecureChannelName(ch->chname)) {
						const char* other_id = NetSecureChannelID(ch->chname);
						if (other_id && id && strcmp(id, other_id) == 0 && strcmp(name, ch->chname) != 0) {
							if (HasCap(sptr, CAP_STANDARDREPLIES)) {
								send_note_reply(&me, sptr, "INFO", cli_name(sptr),
									"Network-secure channel ID collision: %s already exists as %s. Please join that channel.", id, ch->chname);
							}
							else {
								sendrawto_one(sptr, ":%s NOTICE %s :Network-secure channel ID collision: %s already exists as %s. Please join that channel.",
									cli_name(&me), cli_name(sptr), id, ch->chname);
							}
							name = ch->chname;
							found = 1;
							break;
						}
					}
				}
				if (!found) {
					if (HasCap(sptr, CAP_STANDARDREPLIES)) {
						send_note_reply(&me, sptr, "INFO", cli_name(sptr), "Network-secure channel detected: %s", name);
					}
					else {
						sendrawto_one(sptr, ":%s NOTICE %s :Network-secure channel detected: %s",
							cli_name(&me), cli_name(sptr), name);
					}
				}
			}
		}

		// Save the original channel name for invite check
		const char* orig_name = name;
		struct Channel* orig_channel = FindChannel(orig_name);

		// If the user is invited to the original channel, allow join there and skip redirect/+L
		if (orig_channel && IsInvited(sptr, orig_channel)) {
			// Remove the invite so it can't be reused
			del_invite(sptr, orig_channel);

			// Usual join logic for invited users
			if (find_member_link(orig_channel, sptr)) {
				continue; // already on channel
			}
			if (check_target_limit(sptr, orig_channel, orig_channel->chname, 0)) {
				continue;
			}


			int flags = CHFL_DEOPPED;
			if (IsChannelService(sptr) || IsService(sptr)) {
				flags |= CHFL_CHANSERVICE;
			}

			joinbuf_join(&join, orig_channel, flags);

			// Send topic and names as usual
			if (orig_channel->topic[0]) {
				send_reply(sptr, RPL_TOPIC, orig_channel->chname, orig_channel->topic);
				send_reply(sptr, RPL_TOPICWHOTIME, orig_channel->chname, orig_channel->topic_nick, orig_channel->topic_time);
			}
			do_names(sptr, orig_channel, NAMES_ALL | NAMES_EON);

			continue; // skip redirect/+L logic for this channel
		}

		/* Channel redirect */
		const char* redirect = find_channel_redirect(name);

		if (redirect) {
			// Redirect vorhanden: auf neuen Namen umleiten
			name = redirect;
			if (IsLocalChannel(name)) {
				// Lokale Channels dürfen nicht weitergeleitet werden
				send_reply(sptr, ERR_BADNETWORKCHAN, name);
				continue;
			}
		}
		else {
			int redirect_count = 0;
			const int MAX_REDIRECTS = 5;
			struct Channel* ch = FindChannel(name);

			while (ch && redirect_count < MAX_REDIRECTS) {
				int err = 0;
				int flags = CHFL_DEOPPED;
				// Joinflood check also in redirect loop
				if ((ch->mode.mode & MODE_JOINFLOOD) &&
					ch->mode.jflood.max_joins > 0 && ch->mode.jflood.time_window > 0 &&
					!(ch->mode.mode & MODE_REGONLY) && !IsOper(sptr) && !IsService(sptr) && !IsChannelService(sptr)) {
					time_t now = TStime();
					struct JoinFloodState* jf = &ch->mode.jflood;

					if (jf->window_start == 0 || now - jf->window_start >= jf->time_window) {
						jf->window_start = now;
						jf->join_count = 1;
					}
					else {
						jf->join_count++;
					}

					if (jf->join_count > jf->max_joins) {
						// Instead of blocking, set REGONLY mode
						if (!(ch->mode.mode & MODE_REGONLY)) {
							ch->mode.mode |= MODE_REGONLY;
							struct ModeBuf mbuf;
							modebuf_init(&mbuf, &me, cptr, ch, MODEBUF_DEST_CHANNEL | MODEBUF_DEST_SERVER);
							modebuf_mode(&mbuf, MODE_ADD | MODE_REGONLY);
							modebuf_flush(&mbuf);
							jf->window_start = 0;
							jf->join_count = 0;
						}
						// Allow join to continue
					}
				}
				// Calculate flags as before (e.g. Services, Owner, etc.)

				// Check join conditions as before:
				if (find_member_link(ch, sptr)) {
					break; // already on channel
				}
				if (check_target_limit(sptr, ch, ch->chname, 0)) {
					break;
				}
				if (ch->mode.mode & MODE_INVITEONLY && !IsInvited(sptr, ch))
					err = ERR_INVITEONLYCHAN;
				else if (ch->mode.limit && (ch->users >= ch->mode.limit))
					err = ERR_CHANNELISFULL;
				else if ((ch->mode.mode & MODE_REGONLY) && !IsAccount(sptr))
					err = ERR_NEEDREGGEDNICK;
				else if (find_ban(sptr, ch->exceptlist) == NULL && find_ban(sptr, ch->banlist))
					err = ERR_BANNEDFROMCHAN;
				else if (*ch->mode.key && (!key || strcmp(key, ch->mode.key)))
					err = ERR_BADCHANNELKEY;
				else if ((ch->mode.mode & MODE_TLSONLY) && !IsTLS(sptr))
					err = ERR_TLSONLYCHAN;

				// XtraOp override as before
				if (IsXtraOp(sptr))
					err = 0;

				// If there is an error and +L is set, redirect
				if (err && (ch->mode.mode & MODE_LINK) && *ch->mode.linkchan) {
					// Loop protection
					struct Channel* target = FindChannel(ch->mode.linkchan);
					if (target && (target->mode.mode & MODE_LINK) && !strcmp(target->mode.linkchan, ch->chname)) {
						send_reply(sptr, ERR_LINKCHANNELLOOP, ch->chname, ch->mode.linkchan, ch->chname, ch->mode.linkchan);
						break;
					}
					if (IsLocalChannel(ch->mode.linkchan)) {
						send_reply(sptr, ERR_BADNETWORKCHAN, ch->mode.linkchan);
						break;
					}
					// Redirect
					name = ch->mode.linkchan;
					ch = FindChannel(name);
					redirect_count++;
					continue; // Repeat join check for new channel
				}

				// If no error, perform normal join
				if (!err) {
					joinbuf_join(&join, ch, flags);
					// Set modes as before
					del_invite(sptr, ch);
					if (ch->topic[0]) {
						send_reply(sptr, RPL_TOPIC, ch->chname, ch->topic);
						send_reply(sptr, RPL_TOPICWHOTIME, ch->chname, ch->topic_nick, ch->topic_time);
					}
					do_names(sptr, ch, NAMES_ALL | NAMES_EON);
				}
				else if (submitted == 0) {
					// Error without redirect: send error
					switch (err) {
					case ERR_NEEDREGGEDNICK:
						send_reply(sptr, ERR_NEEDREGGEDNICK, ch->chname, feature_str(FEAT_URLREG));
						break;
					default:
						send_reply(sptr, err, ch->chname);
						break;
					}
					submitted = 1; // Mark that we submitted an error
				}
				break; // Exit loop after join or error
			}
		}
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

		if (cli_user(sptr)->joined >= feature_int(FEAT_MAXCHANNELSPERUSER)
			&& !HasPriv(sptr, PRIV_CHAN_LIMIT)) {
			send_reply(sptr, ERR_TOOMANYCHANNELS, name);
			break; /* no point processing the other channels */
		}

		/* BADCHANed channel */
		if (!IsAnOper(sptr)
			&& (gline = gline_lookup_badchan((char*)name, GLINE_BADCHAN))) {
			send_reply(sptr, ERR_BADCHANNAME, (char*)name, gline->gl_reason);
			continue;
		}

		if (!(chptr = FindChannel(name))) {
			if (((name[0] == '&') && !feature_bool(FEAT_LOCAL_CHANNELS))
				|| strlen(name) >= IRCD_MIN(CHANNELLEN, feature_int(FEAT_CHANNELLEN))) {
				send_reply(sptr, ERR_NOSUCHCHANNEL, name);
				continue;
			}

			if (!(chptr = get_channel(sptr, (char*)name, CGT_CREATE)))
				continue;

			/* Joinflood check also on channel creation */
			if ((chptr->mode.mode & MODE_JOINFLOOD) &&
				chptr->mode.jflood.max_joins > 0 && chptr->mode.jflood.time_window > 0 &&
				!(chptr->mode.mode & MODE_REGONLY) && !IsOper(sptr) && !IsService(sptr) && !IsChannelService(sptr)) {
				time_t now = TStime();
				struct JoinFloodState* jf = &chptr->mode.jflood;

				if (jf->window_start == 0 || now - jf->window_start >= jf->time_window) {
					jf->window_start = now;
					jf->join_count = 1;
				}
				else {
					jf->join_count++;
				}

				if (jf->join_count > jf->max_joins) {
					/* Instead of blocking, set REGONLY mode */
					if (!(chptr->mode.mode & MODE_REGONLY)) {
						chptr->mode.mode |= MODE_REGONLY;
						struct ModeBuf mbuf;
						modebuf_init(&mbuf, &me, cptr, chptr, MODEBUF_DEST_CHANNEL | MODEBUF_DEST_SERVER);
						modebuf_mode(&mbuf, MODE_ADD | MODE_REGONLY);
						modebuf_flush(&mbuf);
						jf->window_start = 0;
						jf->join_count = 0;
					}
					// Allow join to continue
				}
			}

			/* Try to add the new channel as a recent target for the user. */
			if (check_target_limit(sptr, chptr, chptr->chname, 0)) {
				chptr->members = 0;
				destruct_channel(chptr);
				continue;
			}

			joinbuf_join(&create, chptr, CHFL_OWNER);
			if (feature_bool(FEAT_AUTOCHANMODES) && feature_str(FEAT_AUTOCHANMODES_LIST) && strlen(feature_str(FEAT_AUTOCHANMODES_LIST)) > 0)
				SetAutoChanModes(chptr);
		}
		else if (find_member_link(chptr, sptr)) {
			continue; /* already on channel */
		}
		else if (check_target_limit(sptr, chptr, chptr->chname, 0)) {
			continue;
		}
		else {
			/* Joinflood check on existing channel */
			if ((chptr->mode.mode & MODE_JOINFLOOD) &&
				chptr->mode.jflood.max_joins > 0 && chptr->mode.jflood.time_window > 0 &&
				!(chptr->mode.mode & MODE_REGONLY) && !IsOper(sptr) && !IsService(sptr) && !IsChannelService(sptr)) {
				time_t now = TStime();
				struct JoinFloodState* jf = &chptr->mode.jflood;

				if (jf->window_start == 0 || now - jf->window_start >= jf->time_window) {
					jf->window_start = now;
					jf->join_count = 1;
				}
				else {
					jf->join_count++;
				}

				if (jf->join_count > jf->max_joins) {
					if (!(chptr->mode.mode & MODE_REGONLY)) {
						/* Instead of blocking, set REGONLY mode */
						chptr->mode.mode |= MODE_REGONLY;
						struct ModeBuf mbuf;
						modebuf_init(&mbuf, &me, cptr, chptr, MODEBUF_DEST_CHANNEL | MODEBUF_DEST_SERVER);
						modebuf_mode(&mbuf, MODE_ADD | MODE_REGONLY);
						modebuf_flush(&mbuf);
						jf->window_start = 0;
						jf->join_count = 0;
					}
					// Allow join to continue
				}
			}
			int flags = CHFL_DEOPPED;
			int err = 0;
			if (IsChannelService(sptr) || IsService(sptr)) {
				flags |= CHFL_CHANSERVICE;
				// Services sollten keine weiteren User-Modi bekommen!
			}
			/* Check Apass/Upass -- since we only ever look at a single
			 * "key" per channel now, this hampers brute force attacks. */
			if (key && !strcmp(key, chptr->mode.apass))
				flags = CHFL_OWNER;
			else if (key && !strcmp(key, chptr->mode.upass))
				flags = CHFL_OWNER;
			else if (chptr->users == 0 && !chptr->mode.apass[0]) {
				/* Joining a zombie channel (zannel): give ops and increment TS. */
				flags = CHFL_OWNER;
				chptr->creationtime++;
			}
			else if (IsInvited(sptr, chptr)) {
				/* Invites bypass these other checks. */
			}
			else if (chptr->mode.mode & MODE_INVITEONLY)
				err = ERR_INVITEONLYCHAN;
			else if (chptr->mode.limit && (chptr->users >= chptr->mode.limit))
				err = ERR_CHANNELISFULL;
			else if ((chptr->mode.mode & MODE_REGONLY) && !IsAccount(sptr))
				err = ERR_NEEDREGGEDNICK;
			else if (find_ban(sptr, chptr->exceptlist) == NULL && find_ban(sptr, chptr->banlist))
				err = ERR_BANNEDFROMCHAN;
			else if (*chptr->mode.key && (!key || strcmp(key, chptr->mode.key)))
				err = ERR_BADCHANNELKEY;
			else if ((chptr->mode.mode & MODE_TLSONLY) && !IsTLS(sptr))
				err = ERR_TLSONLYCHAN;

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
			if (err && submitted == 0) {
				switch (err) {
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
				submitted = 1; // Mark that we submitted an error
				break;
			}

			if (submitted == 1) {
				// If we already submitted an error, skip further processing
				continue;
			}
			joinbuf_join(&join, chptr, flags);
			// Nur für Services: Mode setzen, aber keine doppelten Kommandos!
			if (IsChannelService(sptr) || IsService(sptr)) {
				struct ModeBuf mbuf;
				modebuf_init(&mbuf, &me, cptr, chptr, MODEBUF_DEST_SERVER);
				modebuf_mode_client(&mbuf, MODE_ADD | MODE_CHANSERVICE, sptr, 0);
				modebuf_flush(&mbuf);
			}
			// Nur für normale User: Chanop setzen
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

		do_names(sptr, chptr, NAMES_ALL | NAMES_EON); /* send /names list */
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
int ms_join(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
	struct Membership* member;
	struct Channel* chptr;
	struct JoinBuf join;
	unsigned int flags;
	time_t creation = 0;
	char* p = 0;
	char* chanlist;
	const char* name;

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

		flags = CHFL_DEOPPED;

		if (IsChannelService(sptr) || IsService(sptr))
			flags |= CHFL_CHANSERVICE;

		/* Network-secure channel: check if ID already exists and join that channel if so */
		if (IsNetSecureChannelName(name)) {
			const char* id = NetSecureChannelID(name);
			struct Channel* ch;
			int found = 0;
			for (ch = GlobalChannelList; ch; ch = ch->next) {
				if (IsNetSecureChannelName(ch->chname)) {
					const char* other_id = NetSecureChannelID(ch->chname);
					if (other_id && id && strcmp(id, other_id) == 0) {
						/* ID exists, join that channel instead */
						name = ch->chname;
						found = 1;
						break;
					}
				}
			}
		}

		// --- Channel redirect and +L forwarding for server joins ---
		const char* redirect = find_channel_redirect(name);
		if (redirect) {
			if (IsLocalChannel(redirect)) {
				send_reply(sptr, ERR_BADNETWORKCHAN, redirect);
				continue;
			}
			if (strcmp(redirect, name) == 0) {
				send_reply(sptr, ERR_LINKCHANNELLOOP, name, redirect, name, redirect);
				continue;
			}
			name = redirect;
		}
		else {
			struct Channel* ch = FindChannel(name);
			if (ch && (ch->mode.mode & MODE_LINK) && (ch->mode.mode & MODE_LIMIT) && *ch->mode.linkchan) {
				if (IsLocalChannel(ch->mode.linkchan)) {
					send_reply(sptr, ERR_BADNETWORKCHAN, ch->mode.linkchan);
					continue;
				}
				struct Channel* target = FindChannel(ch->mode.linkchan);
				if (target && (target->mode.mode & MODE_LINK) && !strcmp(target->mode.linkchan, name)) {
					send_reply(sptr, ERR_LINKCHANNELLOOP, name, ch->mode.linkchan, name, ch->mode.linkchan);
					continue;
				}
				name = ch->mode.linkchan;
			}
		}

		if (IsLocalChannel(name) || !IsChannelName(name))
		{
			protocol_violation(cptr, "%s tried to join %s", cli_name(sptr), name);
			continue;
		}

		if (!(chptr = FindChannel(name)))
		{
			/* No channel exists, so create one */
			if (!(chptr = get_channel(sptr, (char*)name, CGT_CREATE)))
			{
				protocol_violation(sptr, "couldn't get channel %s for %s",
					(char*)name, cli_name(sptr));
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
				struct Membership* member;
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
					if (IsChanOp(member)) {
						modebuf_mode_client(&mbuf, MODE_DEL | MODE_CHANOP, member->user, OpLevel(member));
						member->status &= ~CHFL_CHANOP;
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
