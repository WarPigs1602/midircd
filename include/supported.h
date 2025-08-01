/*
 * IRC - Internet Relay Chat, include/supported.h
 * Copyright (C) 1999 Perry Lorier.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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
 * $Id: supported.h,v 1.20.2.2 2007/05/20 13:02:51 entrope Exp $
 *
 * Description: This file has the featureset that ircu announces on connecting
 *              a client.  It's in this .h because it's likely to be appended
 *              to frequently and s_user.h is included by basically everyone.
 *
 * RFC 2811 (IRC Channel Management) and RFC 1459 (original IRC) specify
 * that channel names may begin with '!', which denotes a network-safe channel
 * with a unique ID. This is signaled to clients via CHANTYPES and other
 * ISUPPORT tokens.
 */
#ifndef INCLUDED_supported_h
#define INCLUDED_supported_h

#include "channel.h"
#include "ircd_defs.h"

/* 
 * 'Features' supported by this ircd
 * RFC 2811: CHANTYPES must include '!' for network-safe channels.
 *           CHANID and !CHAN are non-standard but widely used ISUPPORT tokens.
 */
#define FEATURES1 \
                "WHOX"\
                " WALLCHOPS"\
                " WALLVOICES"\
                " USERIP"\
                " CPRIVMSG"\
                " CNOTICE"\
                " SILENCE=%i" \
                " MODES=%i" \
                " MAXCHANNELS=%i" \
                " MAXBANS=%i" \
                " NICKLEN=%i" \
                " EXCEPTS" \
                " CHANID"

#define FEATURES2 "MAXNICKLEN=%i" \
                " TOPICLEN=%i" \
                " AWAYLEN=%i" \
                " KICKLEN=%i" \
                " CHANNELLEN=%i" \
                " MAXCHANNELLEN=%i" \
                " CHANTYPES=%s" \
                " PREFIX=%s" \
                " STATUSMSG=%s" \
                " CHANMODES=%s" \
                " CASEMAPPING=%s" \
                " NETWORK=%s" \
                " !CHAN"

#define FEATURESVALUES1 feature_int(FEAT_MAXSILES), MAXMODEPARAMS, \
			feature_int(FEAT_MAXCHANNELSPERUSER), \
                        feature_int(FEAT_MAXBANS), feature_int(FEAT_NICKLEN)

#define FEATURESVALUES2 NICKLEN, TOPICLEN, AWAYLEN, TOPICLEN, \
                        feature_int(FEAT_CHANNELLEN), CHANNELLEN, \
                        (feature_bool(FEAT_LOCAL_CHANNELS) ? "#&!+" : "#!+"), \
                        "(Sqaohv)!~&@%+", "!~&@%+", \
                        "b,e,j,k,l,L,imnpstrDducCNMTZ", \
                        "rfc2811", feature_str(FEAT_NETWORK)

#endif /* INCLUDED_supported_h */

