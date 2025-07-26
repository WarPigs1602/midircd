/*
 * IRC - Internet Relay Chat, ircd/m_sasl.c
 * Copyright (C) 2002 Alex Badea <vampire@p16.pub.ro>
 *
 * Siehe Datei AUTHORS im IRC-Paket für weitere Entwickler.
 *
 * Dieses Programm ist freie Software; Sie können es unter den Bedingungen
 * der GNU General Public License weitergeben und/oder modifizieren.
 *
 * $Id: m_sasl.c,v 1.3.2.1 2025/04/01 04:04:32 entrope Exp $
 */

#include "config.h"
#include "client.h"
#include "hash.h"
#include "ircd.h"
#include "ircd_alloc.h"
#include "ircd_features.h"
#include "ircd_log.h"
#include "ircd_relay.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "ircd_snprintf.h"
#include "list.h"
#include "msg.h"
#include "numeric.h"
#include "numnicks.h"
#include "send.h"
#include "struct.h"
#include "s_auth.h"
#include "s_conf.h"
#include "s_misc.h"
#include "s_user.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stddef.h>

static int is_service_available(const char* service_name)
{
    if (!service_name || !*service_name)
        return 0;

    struct Client* service = FindClient(service_name);
    if (!service)
        return 0;

    // Service must be registered and not marked as dead/disconnected
    if (!IsChannelService(service) || IsDead(service) || !IsRegistered(service))
        return 0;

    return 1;
}

/*
 * Handles SASL authentication commands.
 * Forwards all SASL parameters and the sender's nickname to the SASL service using relay_private_message.
 * If the service is not available, falls back to iauthd and notifies the client using send_reply.
 */
int m_sasl(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
    if (parc < 2) return 0;

    // Check if SASL capability is set for the client
    if (!CapHas(cli_active(sptr), CAP_SASL)) {
        return 0;
    }

    char* sasl_nick = (char*)feature_str(FEAT_SASL_SERVICE_NAMES);
    const char* sender_nick = cli_name(cptr);
    int service_available = is_service_available(sasl_nick);

    if (service_available) {
        // Build message: SASL <sender_nick> <parv[1]> <parv[2]> ... <parv[parc-1]>
        char msgbuf[1024];
        int offset = ircd_snprintf(NULL, msgbuf, sizeof(msgbuf), "SASL %s", sender_nick);
        for (int i = 1; i < parc && offset < (int)(sizeof(msgbuf) - 1); ++i) {
            offset += ircd_snprintf(NULL, msgbuf + offset, sizeof(msgbuf) - offset, " %s", parv[i] ? parv[i] : "");
        }
        relay_private_message(cptr, sasl_nick, msgbuf);
        return 0;
    }

    send_reply(sptr, ERR_SERVICESDOWN, ":SASL service unavailable");
	auth_set_sasl(cli_auth(sptr), parv[1]); // Mark SASL as attempted
    return 0;
}