/*
 * IRC - Internet Relay Chat, ircd/m_knock.c
 * Implements the /knock command to request entry to +i channels.
 */

#include "config.h"
#include "client.h"
#include "channel.h"
#include "hash.h"
#include "ircd.h"
#include "ircd_log.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "msg.h"
#include "numeric.h"
#include "send.h"


#define KNOCK_RATE_LIMIT 60 // seconds

static time_t last_knock_time = 0; // global, for demo; per-user/channel in real code

/*
 * m_knock - client message handler
 *   parv[0] - sender prefix
 *   parv[1] - channel name
 */
int m_knock(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
    struct Channel *chptr;

    if (parc < 2 || EmptyString(parv[1]))
        return need_more_params(sptr, "KNOCK");

    if (!IsChannelName(parv[1]) || !(chptr = FindChannel(parv[1]))) {
        send_reply(sptr, ERR_NOSUCHCHANNEL, parv[1]);
        return 0;
    }

    int restricted = 0;
    if (chptr->mode.mode & MODE_INVITEONLY)
        restricted = 1;
    else if ((chptr->mode.mode & MODE_LIMIT) && chptr->users >= chptr->mode.limit)
        restricted = 1;
    else if ((chptr->mode.mode & MODE_KEY) && *chptr->mode.key)
        restricted = 1;
    else if ((chptr->mode.mode & MODE_REGONLY) && !IsAccount(sptr))
        restricted = 1;
    else if ((chptr->mode.mode & MODE_TLSONLY) && !IsTLS(sptr))
        restricted = 1;
    else if ((chptr->mode.mode & MODE_LINK))
        restricted = 1;
    else if (find_ban(sptr, chptr->banlist) && !find_ban(sptr, chptr->exceptlist))
        restricted = 1;

    if (!restricted) {
        send_reply(sptr, ERR_KNOCKNOTINVITE, chptr->chname);
        return 0;
    }

    if (find_channel_member(sptr, chptr)) {
        send_reply(sptr, ERR_KNOCKONCHAN, chptr->chname);
        return 0;
    }

    // Optional: Rate limit knock requests (global demo, not per-user/channel)
    if (last_knock_time && (TStime() - last_knock_time < KNOCK_RATE_LIMIT)) {
        send_reply(sptr, ERR_TOOMANYKNOCK, chptr->chname);
        return 0;
    }
    last_knock_time = TStime();

    // Notify channel operators (send notice)
    sendcmdto_channel_butserv_butone(&me, CMD_NOTICE, chptr, NULL, SKIP_NONOPS,
        ":%s has knocked on %s requesting an invite.", cli_name(sptr), chptr->chname);

    // Inform user
    send_reply(sptr, RPL_KNOCK, chptr->chname, ":Your knock has been delivered");

    // Optionally, inform channel operators with a numeric
    send_reply_to_channel_ops(chptr, RPL_KNOCKDLVR, chptr->chname, cli_name(sptr), ":Knock delivered");

    // Forward knock to other servers
    sendcmdto_serv_butone(sptr, CMD_KNOCK, sptr, "%H :%s", chptr, cli_name(sptr));

    return 0;
}

/*
 * ms_knock - server message handler
 *   parv[0] - sender prefix
 *   parv[1] - channel name
 */
int ms_knock(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
    struct Channel *chptr;

    if (parc < 2 || EmptyString(parv[1]))
        return need_more_params(sptr, "KNOCK");

    if (!IsChannelName(parv[1]) || !(chptr = FindChannel(parv[1]))) {
        return 0;
    }

    int restricted = 0;
    if (!chptr)
		return 0; // Channel not found, silently ignore
    if (chptr->mode.mode & MODE_INVITEONLY)
        restricted = 1;
    else if ((chptr->mode.mode & MODE_LIMIT) && chptr->users >= chptr->mode.limit)
        restricted = 1;
    else if ((chptr->mode.mode & MODE_KEY) && *chptr->mode.key)
        restricted = 1;
    else if ((chptr->mode.mode & MODE_REGONLY) && !IsAccount(sptr))
        restricted = 1;
    else if ((chptr->mode.mode & MODE_TLSONLY) && !IsTLS(sptr))
        restricted = 1;
    else if ((chptr->mode.mode & MODE_LINK))
        restricted = 1;
    else if (find_ban(sptr, chptr->banlist) && !find_ban(sptr, chptr->exceptlist))
        restricted = 1;

    if (!restricted) {
        return 0;
    }

    if (find_channel_member(sptr, chptr)) {
        return 0;
    }

    // Notify channel operators (send notice)
    sendcmdto_channel_butserv_butone(&me, CMD_NOTICE, chptr, NULL, SKIP_NONOPS,
        ":%s has knocked on %s requesting an invite.", cli_name(sptr), chptr->chname);

    // Optionally, inform channel operators with a numeric
    send_reply_to_channel_ops(chptr, RPL_KNOCKDLVR, chptr->chname, cli_name(sptr), ":Knock delivered");

    return 0;
}