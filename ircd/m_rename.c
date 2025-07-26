#include "channel.h"
#include "client.h"
#include "hash.h"
#include "ircd.h"
#include "send.h"
#include "ircd_reply.h"
#include "capab.h"
#include "ircd_features.h"
#include "ircd_string.h"
#include "numeric.h"
#include "msg.h"
#include "s_user.h"

/*
 * Handler for the IRCv3 RENAME command.
 * Usage: RENAME <oldchan> <newchan>
 */
int m_rename(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
    if (parc < 3) {
        if (CapHas(cli_active(sptr), CAP_STANDARDREPLIES)) {
            send_fail_reply(&me, sptr, "RENAME", cli_name(sptr), "Not enough parameters");
        } else {
            send_reply(sptr, ERR_NEEDMOREPARAMS, "RENAME");
        }
        return -1;
    }

    const char* oldname = parv[1];
    const char* newname = parv[2];

    if (!IsGlobalChannel(oldname) || !IsGlobalChannel(newname)) {
        if (CapHas(cli_active(sptr), CAP_STANDARDREPLIES)) {
            send_fail_reply(&me, sptr, "RENAME", cli_name(sptr), "Invalid channel name");
        } else {
            send_reply(sptr, ERR_NOSUCHCHANNEL, oldname);
        }
        return -1;
	}

    // Find the channel to rename
    struct Channel* chptr = FindChannel(oldname);
    if (!chptr) {
        if (CapHas(cli_active(sptr), CAP_STANDARDREPLIES)) {
            send_fail_reply(&me, sptr, "RENAME", cli_name(sptr), "No such channel");
        } else {
            send_reply(sptr, ERR_NOSUCHCHANNEL, oldname);
        }
        return -1;
    }

    // Check if new channel name is already in use
    if (FindChannel(newname)) {
        if (CapHas(cli_active(sptr), CAP_STANDARDREPLIES)) {
            send_fail_reply(&me, sptr, "RENAME", cli_name(sptr), "Channel name is already in use");
        }
        else {
            send_reply(sptr, ERR_CHANNELNAMEINUSE, newname);
        }
        return -1;
    }

    // Permission check: only privileged members may rename
    struct Membership* member = find_channel_member(sptr, chptr);
    if (!member || !IsOwner(member)) {
        if (CapHas(cli_active(sptr), CAP_STANDARDREPLIES)) {
            send_fail_reply(&me, sptr, "RENAME", cli_name(sptr), "You do not have permission to rename this channel");
        }
        else {
            send_reply(sptr, ERR_CHANOPRIVSNEEDED, oldname);
        }
        return -1;
    }

    // Save old name for notification
    char oldnamebuf[CHANNELLEN + 1];
    ircd_strncpy(oldnamebuf, chptr->chname, CHANNELLEN);

    // Update channel hash table and name
    hRemChannel(chptr);
    ircd_strncpy(chptr->chname, newname, CHANNELLEN);
    hAddChannel(chptr);

    // Add to rename list for future join redirection
    add_channel_rename(oldnamebuf, newname);
    remove_reverse_rename(newname);

    // Notify all channel members individually
    struct Membership* m;
    for (m = chptr->members; m; m = m->next_member) {
        capset_t caps = cli_active(m->user);
        if (CapHas(caps, CAP_STANDARDREPLIES) && CapHas(caps, CAP_CHANNELRENAME)) {
            // Both capabilities set: send only standard reply
            send_note_reply(&me, m->user, "RENAME", cli_name(m->user), "%s", newname);
        } else {
            // At least one capability missing: simulate PART and JOIN
            sendcmdto_one(m->user, CMD_PART, m->user, "%s :Channel renamed to %s", oldnamebuf, newname);
            sendcmdto_one(m->user, CMD_JOIN, m->user, "%s", newname);
        }
        // Always send NAMES and TOPIC
        do_names(m->user, chptr, NAMES_ALL | NAMES_EON);
        if (chptr->topic[0]) {
            // If the channel has a topic, send it 
            sendcmdto_one(m->user, CMD_TOPIC, &me, "%s :%s", newname, chptr->topic);
        }
    }

    // Additionally: send IRCv3 RENAME reply to all channel members (for clients that support it)
    sendcmdto_channel_butserv_butone(sptr, "RENAME", NULL, chptr, NULL, 0,
        "%s :Channel renamed to %s", oldnamebuf, newname);

    // Confirmation to the user who performed the rename
    capset_t selfcaps = cli_active(sptr);
    if (CapHas(selfcaps, CAP_STANDARDREPLIES) && CapHas(selfcaps, CAP_CHANNELRENAME)) {
        send_note_reply(&me, sptr, "RENAME", cli_name(sptr), "%s", newname);
    } else {
        sendcmdto_one(sptr, CMD_PART, sptr, "%s :Channel renamed to %s", oldnamebuf, newname);
        sendcmdto_one(sptr, CMD_JOIN, sptr, "%s", newname);
        do_names(sptr, chptr, NAMES_ALL | NAMES_EON);
        sendcmdto_one(sptr, CMD_TOPIC, &me, "%s :%s", newname, chptr->topic);
    }

    // Forward the rename command to other servers
    sendcmdto_serv_butone(sptr, CMD_RENAME, NULL, "%s %s", oldnamebuf, newname);

    return 0;
}

/*
 * Handler for the IRCv3 RENAME command received from other servers.
 * Usage: RENAME <oldchan> <newchan>
 * This function is called for remote servers.
 */
int ms_rename(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
    if (parc < 3) {
        // Not enough parameters, silently ignore
        return -1;
    }

    const char* oldname = parv[1];
    const char* newname = parv[2];

    // Find the channel to rename
    struct Channel* chptr = FindChannel(oldname);
    if (!chptr) {
		chptr = get_channel(sptr, (char*)oldname, CGT_CREATE);
        if (!chptr) {
            // Channel does not exist, silently ignore
            return -1;
		}
    }

    // Check if new channel name is already in use
    if (FindChannel(newname)) {
        // New channel name already exists, ignore
        return -1;
    }

    // Save old name for notification
    char oldnamebuf[CHANNELLEN + 1];
    ircd_strncpy(oldnamebuf, chptr->chname, CHANNELLEN);

    // Update the channel hash table before renaming
    hRemChannel(chptr);
    ircd_strncpy(chptr->chname, newname, CHANNELLEN);
    hAddChannel(chptr);

    // Add the rename entry for join redirection
    add_channel_rename(oldnamebuf, newname);
    remove_reverse_rename(newname);

    // Notify all channel members:
    struct Membership* m;
    for (m = chptr->members; m; m = m->next_member) {
        capset_t caps = cli_active(m->user);
        if (CapHas(caps, CAP_STANDARDREPLIES) && CapHas(caps, CAP_CHANNELRENAME)) {
            // IRCv3: send note reply
            send_note_reply(&me, sptr, "RENAME", cli_name(sptr), "%s", newname);
        }
        else {
            // Classic: simulate PART and JOIN
            sendcmdto_one(m->user, CMD_PART, m->user, "%s :Channel renamed to %s", oldnamebuf, newname);
            sendcmdto_one(m->user, CMD_JOIN, m->user, "%s", newname);
        }
        // Always send NAMES and TOPIC
        do_names(m->user, chptr, NAMES_ALL | NAMES_EON);
        if (chptr->topic[0]) {
            sendcmdto_one(m->user, CMD_TOPIC, &me, "%s :%s", newname, chptr->topic);
        }
    }

    // Additionally: send IRCv3 RENAME reply to all channel members
    sendcmdto_channel_butserv_butone(sptr, "RENAME", NULL, chptr, NULL, 0,
        "%s :Channel renamed to %s", oldnamebuf, newname);

    // Forward the rename command to other servers except the source
    sendcmdto_serv_butone(sptr, CMD_RENAME, NULL, "%s %s", oldnamebuf, newname);

    return 0;
}