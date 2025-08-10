/*
 * IRC - Internet Relay Chat, ircd/m_rename.c
 * Implements IRCv3 draft/channel-rename capability with standard replies and fallback.
 * Copyright (C) 2025 GitHub Copilot
 */

#include "config.h"
#include "client.h"
#include "channel.h"
#include "hash.h"
#include "ircd.h"
#include "ircd_log.h"
#include "ircd_reply.h"
#include "ircd_features.h"
#include "send.h"
#include "msg.h"
#include "numeric.h"
#include "s_conf.h"
#include "s_user.h"
#include "capab.h"
#include <string.h>

static int has_rename_caps(struct Client* c)
{
    capset_t caps = cli_active(c);
    return CapHas(caps, CAP_CHANNELRENAME) && CapHas(caps, CAP_STANDARDREPLIES);
}

/*
 * m_rename - handle channel renaming
 * parv[0] = sender prefix
 * parv[1] = old channel name
 * parv[2] = new channel name
 */
int m_rename(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
    struct Channel* chptr;
    struct Channel* newchptr;
    char* oldname;
    char* newname;

    if (parc < 3) {
        if (has_rename_caps(sptr)) {
            send_standard_reply(&me, sptr, "FAIL", "RENAME", cli_name(sptr), "Not enough parameters");
        } else {
            send_reply(sptr, ERR_NEEDMOREPARAMS, "RENAME");
        }
        return 0;
    }

    oldname = parv[1];
    newname = parv[2];

    chptr = FindChannel(oldname);
    // Prevent rename if a service is present in the channel
    for (struct Membership* member = chptr->members; member; member = member->next_member) {
        if (IsService(member->user) || IsChannelService(member->user)) {
            if (has_rename_caps(sptr)) {
                send_standard_reply(&me, sptr, "FAIL", "RENAME", cli_name(sptr), "Channel contains a service, rename not allowed");
            }
            else {
                send_reply(sptr, ERR_RENAME_SERVICEPRESENT, oldname);
            }
            return 0;
        }
    }
    if (!chptr) {
        if (has_rename_caps(sptr)) {
            send_standard_reply(&me, sptr, "FAIL", "RENAME", cli_name(sptr), "No such channel: %s", oldname);
        } else {
            send_reply(sptr, ERR_NOSUCHCHANNEL, oldname);
        }
        return 0;
    }

    newchptr = FindChannel(newname);
    if (newchptr) {
        if (has_rename_caps(sptr)) {
            send_standard_reply(&me, sptr, "FAIL", "RENAME", cli_name(sptr), "Channel name is already in use: %s", newname);
        } else {
            send_reply(sptr, ERR_NICKNAMEINUSE, newname);
        }
        return 0;
    }

    // Only allow channel owners or servers to rename
    struct Membership* memb = find_member_link(chptr, sptr);
    if (!IsServer(cptr) && (!memb || !IsOwner(memb))) {
        if (has_rename_caps(sptr)) {
            send_standard_reply(&me, sptr, "FAIL", "RENAME", cli_name(sptr), "You're not a channel owner");
        } else {
            send_reply(sptr, ERR_CHANOPRIVSNEEDED, oldname);
        }
        return 0;
    }

    if (!IsChannelName(newname)) {
        if (has_rename_caps(sptr)) {
            send_standard_reply(&me, sptr, "FAIL", "RENAME", cli_name(sptr), "Invalid channel name: %s", newname);
        } else {
            send_reply(sptr, ERR_BADCHANNAME, newname);
        }
        return 0;
    }

    // Actually perform the rename in the channel hash table and add to redirect list
    if (channel_rename(chptr, newname) != 0) {
        if (has_rename_caps(sptr)) {
            send_standard_reply(&me, sptr, "FAIL", "RENAME", cli_name(sptr), "Channel rename failed");
        } else {
            send_reply(sptr, ERR_UNKNOWNCOMMAND, "RENAME");
        }
        return 0;
    }

    // Add redirect to the list in channel.c
    add_channel_redirect(oldname, newname);

    // Notify the user who performed the rename
    if (has_rename_caps(sptr)) {
        send_standard_reply(&me, sptr, "NOTE", "RENAME", cli_name(sptr), "Channel successfully renamed from %s to %s", oldname, newname);
    } else {
        send_reply(sptr, RPL_RENAMEOK, oldname, newname);
    }

    // Notify all channel members
    for (struct Membership* member = chptr->members; member; member = member->next_member) {
        struct Client* target = member->user;
        if (has_rename_caps(target)) {
            send_standard_reply(&me, target, "NOTE", "RENAME", cli_name(target), "Channel renamed from %s to %s", oldname, newname);
        } else {
            // Fallback: PART/JOIN for clients without the capability
            sendcmdto_one(target, CMD_PART, target, "%s :Channel renamed to %s", oldname, newname);
            sendcmdto_one(target, CMD_JOIN, target, "%s", newname);

            do_names(target, chptr, NAMES_ALL | NAMES_EON);

            if (chptr->topic[0]) {
                send_reply(target, RPL_TOPIC, chptr->chname, chptr->topic);
                send_reply(target, RPL_TOPICWHOTIME, chptr->chname, chptr->topic_nick, chptr->topic_time);
            } else {
                send_reply(target, RPL_NOTOPIC, chptr->chname);
            }
        }
    }

    // Propagate to other servers
    sendcmdto_serv_butone(&me, CMD_RENAME, cptr, "%s %s", oldname, newname);

    return 0;
}

/*
 * ms_rename - handle channel renaming from other servers
 * parv[0] = sender prefix (server)
 * parv[1] = old channel name
 * parv[2] = new channel name
 */
int ms_rename(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
    struct Channel* chptr;
    struct Channel* newchptr;
    char* oldname;
    char* newname;

    if (parc < 3) {
        // Protocol violation: too few parameters
        return protocol_violation(sptr, "Too few parameters for RENAME");
    }

    oldname = parv[1];
    newname = parv[2];

    chptr = FindChannel(oldname);
    if (!chptr)
        return 0;

    newchptr = FindChannel(newname);
    if (newchptr)
        return 0;

    if (channel_rename(chptr, newname) != 0)
        return 0;

    add_channel_redirect(oldname, newname);

    for (struct Membership* member = chptr->members; member; member = member->next_member) {
        struct Client* target = member->user;
        if (has_rename_caps(target)) {
            send_standard_reply(&me, target, "NOTE", "RENAME", cli_name(target), "Channel renamed from %s to %s", oldname, newname);
        } else {
            sendcmdto_one(target, CMD_PART, target, "%s :Channel renamed to %s", oldname, newname);
            sendcmdto_one(target, CMD_JOIN, target, "%s", newname);

            do_names(target, chptr, NAMES_ALL | NAMES_EON);

            if (chptr->topic[0]) {
                send_reply(target, RPL_TOPIC, chptr->chname, chptr->topic);
                send_reply(target, RPL_TOPICWHOTIME, chptr->chname, chptr->topic_nick, chptr->topic_time);
            } else {
                send_reply(target, RPL_NOTOPIC, chptr->chname);
            }
        }
    }

    sendcmdto_serv_butone(&me, CMD_RENAME, cptr, "%s %s", oldname, newname);

    return 0;
}