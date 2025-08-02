/*
 * IRC - Internet Relay Chat, ircd/channel.h
 * Copyright (C) 1990 Jarkko Oikarinen
 * Copyright (C) 1996 - 1997 Carlo Wood
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
 */
/** @file
 * @brief Channel management and maintenance.
 * @version $Id: channel.h,v 1.55.2.2 2006/08/03 03:01:50 entrope Exp $
 */
#ifndef INCLUDED_channel_h
#define INCLUDED_channel_h
#ifndef INCLUDED_ircd_defs_h
#include "ircd_defs.h"        /* NICKLEN */
#endif
#ifndef INCLUDED_sys_types_h
#include <sys/types.h>
#define INCLUDED_sys_types_h
#endif
#ifndef INCLUDED_res_h
#include "res.h"
#endif

struct SLink;
struct Client;

/*
 * General defines
 */

#define MAXMODEPARAMS   6	/**< Maximum number of mode parameters */
#define MODEBUFLEN      200	/**< Maximum length of a mode */

#define KEYLEN          23	/**< Maximum length of a key */
#define CHANNELLEN      200	/**< Maximum length of a channel */

#define MAXJOINARGS	15 	/**< number of slots for join buffer */
#define STARTJOINLEN	10 	/**< fuzzy numbers */
#define STARTCREATELEN	20

/*
 * Macro's
 */

#define ChannelExists(n)        (0 != FindChannel(n))

/* Erweiterte Channel-User-Modi und Präfixe */

/* Channel user status flags */
#define CHFL_CHANSERVICE  0x00001  /* +S, ChanService, prefix '!' */
#define CHFL_OWNER        0x00002  /* +q, Owner, prefix '~' */
#define CHFL_ADMIN        0x00004  /* +a, Admin, prefix '&' */
#define CHFL_CHANOP       0x00008  /* +o, Chanop, prefix '@' */
#define CHFL_HALFOP       0x00010  /* +h, Halfop, prefix '%' */
#define CHFL_VOICE        0x00020  /* +v, Voice, prefix '+' */

#define CHFL_DEOPPED            0x00040  /**< Is de-opped by a server */
#define CHFL_SERVOPOK           0x00080  /**< Server op allowed */
#define CHFL_ZOMBIE             0x00100  /**< Kicked from channel */
#define CHFL_BURST_JOINED       0x01000  /**< Just joined by net.junction */
#define CHFL_BANVALID           0x08000  /**< CHFL_BANNED bit is valid */
#define CHFL_BANNED             0x10000  /**< Channel member is banned */
#define CHFL_SILENCE_IPMASK     0x20000  /**< silence mask is a CIDR */
#define CHFL_BURST_ALREADY_OPPED	0x040000  
					/**< In oob BURST, but was already 
					 * joined and opped 
					 */
#define CHFL_BURST_ALREADY_VOICED	0x080000  
					/**, In oob BURST, but was already 
					 * joined and voiced 
					 */
#define CHFL_CHANNEL_MANAGER	CHFL_OWNER	/**< Set when creating channel or using 
					 * Apass 
					 */
#define CHFL_USER_PARTING       0x200000 /**< User is already parting that 
					 * channel 
					 */
#define CHFL_DELAYED            0x400000 /**< User's join message is delayed */

#define CHFL_BURST_ALREADY_ADMIN	0x800000  /**< In oob BURST, but was already 
					 * joined and admin 
					 */
#define CHFL_BURST_ALREADY_HALFOP	0x1000000  /**< In oob BURST, but was already
					 * joined and halfop 
					 */
#define CHFL_BURST_ALREADY_SERVICE	0x2000000  /**< In oob BURST, but was already
					 * joined and service 
					 */
#define CHFL_BURST_ALREADY_OWNER	0x4000000  /**< In oob BURST, but was already
					 * joined and owner 
					 */

#define CHFL_OVERLAP         (CHFL_CHANSERVICE | CHFL_OWNER | CHFL_ADMIN | CHFL_CHANOP | CHFL_HALFOP | CHFL_VOICE)
#define CHFL_BANVALIDMASK    (CHFL_BANVALID | CHFL_BANNED)
#define CHFL_VOICED_OR_OPPED (CHFL_CHANSERVICE | CHFL_OWNER | CHFL_ADMIN | CHFL_CHANOP | CHFL_HALFOP | CHFL_VOICE)

/* Präfixe für die Anzeige */
#define PREFIX_CHANSERVICE '!'
#define PREFIX_OWNER       '~'
#define PREFIX_ADMIN       '&'
#define PREFIX_CHANOP      '@'
#define PREFIX_HALFOP      '%'
#define PREFIX_VOICE       '+'

/* Hierarchie: S > q > a > o > h > v */
#define STATUS_HIERARCHY (CHFL_CHANSERVICE | CHFL_OWNER | CHFL_ADMIN | CHFL_CHANOP | CHFL_HALFOP | CHFL_VOICE)

/** Channel visibility macros */

#define MODE_CHANOP     CHFL_CHANOP	/**< +o Chanop */
#define MODE_VOICE      CHFL_VOICE	/**< +v Voice */
#define MODE_HALFOP    CHFL_HALFOP	/**< +h Halfop */
#define MODE_OWNER      CHFL_OWNER	/**< +q Owner */
#define MODE_ADMIN      CHFL_ADMIN	/**< +a Admin */
#define MODE_CHANSERVICE CHFL_CHANSERVICE /**< +S ChanService */
#define MODE_PRIVATE    0x0004		/**< +p Private */
#define MODE_SECRET     0x0008		/**< +s Secret */
#define MODE_MODERATED  0x0010		/**< +m Moderated */
#define MODE_TOPICLIMIT 0x0020		/**< +t Topic Limited */
#define MODE_INVITEONLY 0x0040		/**< +i Invite only */
#define MODE_NOPRIVMSGS 0x0080		/**< +n No Private Messages */
#define MODE_KEY        0x0100		/**< +k Keyed */
#define MODE_BAN        0x0200		/**< +b Ban */
#define MODE_LIMIT      0x0400		/**< +l Limit */
#define MODE_REGONLY    0x0800  	/**< Only +r users may join */
#define MODE_DELJOINS   0x1000  	/**< New join messages are delayed */
#define MODE_NOCOLOUR   0x2000          /**< No mIRC/ANSI colors/bold */
#define MODE_NOCTCP     0x4000          /**< No channel CTCPs */
#define MODE_NONOTICE   0x8000          /**< No channel notices */
#define MODE_SAVE	0x20000		/**< save this mode-with-arg 'til 
					 * later */
#define MODE_FREE	0x40000 	/**< string needs to be passed to 
					 * MyFree() */
#define MODE_BURSTADDED	0x80000		/**< channel was created by a BURST */
#define MODE_UPASS	0x100000
#define MODE_APASS	0x200000
#define MODE_WASDELJOINS 0x400000 	/**< Not DELJOINS, but some joins 
					 * pending */
#define MODE_NOQUITPARTS 0x800000

#define MODE_NOMULTITARGET 0x1000000    /**< +T No multiple targets */
#define MODE_MODERATENOREG 0x2000000    /**< +M Moderate unauthed users */
#define MODE_TLSONLY       0x4000000    /**< +Z TLS users only */
#define MODE_LINK      0x8000000    /**< +L Channel redirect */
#define MODE_JOINFLOOD 0x10000000 /**< +j Join flood protection */
#define MODE_EXCEPT    0x20000000 /**< +e Exception list */

/** mode flags which take another parameter (With PARAmeterS)
 */
#define MODE_WPARAS     (MODE_CHANSERVICE|MODE_OWNER|MODE_ADMIN|MODE_CHANOP|MODE_HALFOP|MODE_VOICE|MODE_BAN|MODE_KEY|MODE_LIMIT|MODE_APASS|MODE_UPASS|MODE_LINK)

 /** Available Channel modes */
#define infochanmodes feature_bool(FEAT_OPLEVELS) ? "AbeijklmnopstUvrDcCNuMTqSahL" : "beijklmnopstvrDcCNuMTqSahL"
/** Available Channel modes that take parameters */
#define infochanmodeswithparams feature_bool(FEAT_OPLEVELS) ? "AbejkloUvqSahL" : "bekjlovqSahL"

#define HoldChannel(x)          (!(x))
/** name invisible */
#define SecretChannel(x)        ((x) && ((x)->mode.mode & MODE_SECRET))
/** channel not shown but names are */
#define HiddenChannel(x)        ((x) && ((x)->mode.mode & MODE_PRIVATE))
/** channel visible */
#define ShowChannel(v,c)        (PubChannel(c) || find_channel_member((v),(c)))
#define PubChannel(x)           ((!x) || ((x)->mode.mode & \
                                    (MODE_PRIVATE | MODE_SECRET)) == 0)

#define IsNetSecureChannel(name)   (*(name) == '!')
#define IsGlobalChannel(name)   (*(name) == '#')
#define IsLocalChannel(name) ((name) && (*(name) == '&' || *(name) == '+'))
#define IsChannelName(name)     (IsGlobalChannel(name) || IsLocalChannel(name) || IsNetSecureChannel(name))

typedef enum ChannelGetType {
  CGT_NO_CREATE,
  CGT_CREATE
} ChannelGetType;

/* used in SetMode() in channel.c and m_umode() in s_msg.c */

#define MODE_NULL      0
#define MODE_ADD       0x40000000
#define MODE_DEL       0x20000000

/* used in ListingArgs.flags */

#define LISTARG_TOPICLIMITS     0x0001
#define LISTARG_SHOWSECRET      0x0002
#define LISTARG_NEGATEWILDCARD  0x0004

/**
 * Maximum acceptable lag time in seconds: A channel younger than
 * this is not protected against hacking admins.
 * Mainly here to check if the TS clocks really sync (otherwise this
 * will start causing HACK notices.
 * This value must be the same on all servers.
 *
 * This value has been increased to 1 day in order to distinguish this
 * "normal" type of HACK wallops / desyncs, from possiblity still
 * existing bugs.
 */
#define TS_LAG_TIME 86400



extern const char* const PartFmt1;
extern const char* const PartFmt2;
extern const char* const PartFmt1serv;
extern const char* const PartFmt2serv;


/*
 * Structures
 */

/** Information about a client on one channel
 *
 * This structure forms a sparse matrix with users down the side, and
 * channels across the top.  This matrix holds all the information about
 * which users are on what channels, and what modes that user has on that
 * channel (if they are op'd, voice'd and cached information if they are
 * banned or not)
 */
struct Membership {
  struct Client*     user;		/**< The user */
  struct Channel*    channel;		/**< The channel */
  struct Membership* next_member;	/**< The next user on this channel */
  struct Membership* prev_member;	/**< The previous user on this channel*/
  struct Membership* next_channel;	/**< Next channel this user is on */
  struct Membership* prev_channel;	/**< Previous channel this user is on*/
  unsigned int       status;		/**< Flags for op'd, voice'd, etc */
  unsigned short     oplevel;		/**< Op level */
};

#define MAXOPLEVELDIGITS    3
#define MAXOPLEVEL          999
#define MINOPLEVEL          100 /* minimum oplevel that ordinary users can set */

#define IsZombie(x)         ((x)->status & CHFL_ZOMBIE) /**< see \ref zombie */
#define IsDeopped(x)        ((x)->status & CHFL_DEOPPED)
#define IsBanned(x)         ((x)->status & CHFL_BANNED)
#define IsBanValid(x)       ((x)->status & CHFL_BANVALID)
#define IsChanOp(x)         ((x)->status & CHFL_CHANOP)
#define OpLevel(x)          ((x)->oplevel)
#define HasVoice(x)         ((x)->status & CHFL_VOICE)
#define IsChanService(x)   ((x)->status & CHFL_CHANSERVICE)
#define IsOwner(x)         ((x)->status & CHFL_OWNER)
#define IsAdmin(x)         ((x)->status & CHFL_ADMIN)
#define IsHalfOp(x)        ((x)->status & CHFL_HALFOP)
#define IsServOpOk(x)       ((x)->status & CHFL_SERVOPOK)
#define IsBurstJoined(x)    ((x)->status & CHFL_BURST_JOINED)
#define IsVoicedOrOpped(x)  ((x)->status & CHFL_VOICED_OR_OPPED)
#define IsChannelManager(x) ((x)->status & CHFL_CHANNEL_MANAGER)
#define IsUserParting(x)    ((x)->status & CHFL_USER_PARTING)
#define IsDelayedJoin(x)    ((x)->status & CHFL_DELAYED)

#define SetBanned(x)        ((x)->status |= CHFL_BANNED)
#define SetBanValid(x)      ((x)->status |= CHFL_BANVALID)
#define SetDeopped(x)       ((x)->status |= CHFL_DEOPPED)
#define SetServOpOk(x)      ((x)->status |= CHFL_SERVOPOK)
#define SetBurstJoined(x)   ((x)->status |= CHFL_BURST_JOINED)
#define SetZombie(x)        ((x)->status |= CHFL_ZOMBIE)
#define SetChannelManager(x) ((x)->status |= CHFL_CHANNEL_MANAGER)
#define SetOpLevel(x, v)    (void)((x)->oplevel = (v))
#define SetUserParting(x)   ((x)->status |= CHFL_USER_PARTING)
#define SetDelayedJoin(x)   ((x)->status |= CHFL_DELAYED)

#define ClearBanned(x)      ((x)->status &= ~CHFL_BANNED)
#define ClearBanValid(x)    ((x)->status &= ~CHFL_BANVALID)
#define ClearDeopped(x)     ((x)->status &= ~CHFL_DEOPPED)
#define ClearServOpOk(x)    ((x)->status &= ~CHFL_SERVOPOK)
#define ClearBurstJoined(x) ((x)->status &= ~CHFL_BURST_JOINED)
#define ClearDelayedJoin(x) ((x)->status &= ~CHFL_DELAYED)

struct JoinFloodState {
	int max_joins;         // Maximum allowed joins
	int time_window;       // Time window in seconds
	int join_count;        // Current join count in window
	time_t window_start;   // Start time of current window
};

/** Mode information for a channel */
struct Mode {
  unsigned int mode;
  unsigned int limit;
  char key[KEYLEN + 1];
  char upass[KEYLEN + 1];
  char apass[KEYLEN + 1];
  char linkchan[CHANNELLEN + 1]; /**< Channel redirect, if any */
  char joinflood[32]; /**< Join flood protection, if any */
  struct JoinFloodState jflood; /**< Join flood state */
};


#define BAN_IPMASK         0x0001  /**< ban mask is an IP-number mask */
#define BAN_OVERLAPPED     0x0002  /**< ban overlapped, need bounce */
#define BAN_BURSTED        0x0004  /**< Ban part of last BURST */
#define BAN_BURST_WIPEOUT  0x0008  /**< Ban will be wiped at EOB */
#define BAN_EXCEPTION      0x0010  /**< Ban is an exception */
#define BAN_DEL            0x4000  /**< Ban is being removed */
#define BAN_ADD            0x8000  /**< Ban is being added */

/** A single ban for a channel. */
struct Ban {
  struct Ban* next;           /**< next ban in the channel */
  struct irc_in_addr address; /**< address for BAN_IPMASK bans */
  time_t when;                /**< timestamp when ban was added */
  unsigned short flags;       /**< modifier flags for the ban */
  unsigned char nu_len;       /**< length of nick!user part of banstr */
  unsigned char addrbits;     /**< netmask length for BAN_IPMASK bans */
  char who[NICKLEN+1];        /**< name of client that set the ban */
  char banstr[NICKLEN+USERLEN+HOSTLEN+3];  /**< hostmask that the ban matches */
};

/** Information about a channel */
struct Channel {
  struct Channel*    next;	/**< next channel in the global channel list */
  struct Channel*    prev;	/**< previous channel */
  struct Channel*    hnext;	/**< Next channel in the hash table */
  struct DestructEvent* destruct_event;	
  time_t             creationtime; /**< Creation time of this channel */
  time_t             topic_time;   /**< Modification time of the topic */
  unsigned int       users;	   /**< Number of clients on this channel */
  struct Membership* members;	   /**< Pointer to the clients on this channel*/
  struct SLink*      invites;	   /**< List of invites on this channel */
  struct Ban*        banlist;      /**< List of bans on this channel */
  struct Ban*        exceptlist;   /**< List of ban exceptions on this channel */
  struct Mode        mode;	   /**< This channels mode */
  char               topic[TOPICLEN + 1]; /**< Channels topic */
  char               topic_nick[NICKLEN + 1]; /**< Nick of the person who set
						*  The topic
						*/
  char relinkchan[CHANNELLEN + 1]; /**< Channel redirect, if any */
  char               chname[1];	   /**< Dynamically allocated string of the 
				     * channel name
				     */
};

/** Information about a /list in progress */
struct ListingArgs {
  time_t max_time;
  time_t min_time;
  unsigned int max_users;
  unsigned int min_users;
  unsigned int flags;
  time_t max_topic_time;
  time_t min_topic_time;
  unsigned int bucket;
  char wildcard[CHANNELLEN];
};

struct ModeBuf {
  unsigned int		mb_add;		/**< Modes to add */
  unsigned int		mb_rem;		/**< Modes to remove */
  struct Client	       *mb_source;	/**< Source of MODE changes */
  struct Client	       *mb_connect;	/**< Connection of MODE changes */
  struct Channel       *mb_channel;	/**< Channel they affect */
  unsigned int		mb_dest;	/**< Destination of MODE changes */
  unsigned int		mb_count;	/**< Number of modes w/args */
  struct {
    unsigned int	mbm_type;	/**< Type of argument */
    union {
      unsigned int	mbma_uint;	/**< A limit */
      char	       *mbma_string;	/**< A string */
      struct Client    *mbma_client;	/**< A client */
    }			mbm_arg;	/**< The mode argument */
    unsigned short      mbm_oplevel;    /**< Oplevel for a bounce */
  }			mb_modeargs[MAXMODEPARAMS];
					/**< A mode w/args */
};

#define MODEBUF_DEST_CHANNEL	0x00001	/**< Mode is flushed to channel */
#define MODEBUF_DEST_SERVER	0x00002	/**< Mode is flushed to server */

#define MODEBUF_DEST_OPMODE	0x00100	/**< Send server mode as OPMODE */
#define MODEBUF_DEST_DEOP	0x00200	/**< Deop the offender */
#define MODEBUF_DEST_BOUNCE	0x00400	/**< Bounce the modes */
#define MODEBUF_DEST_LOG	0x00800	/**< Log the mode changes to OPATH */

#define MODEBUF_DEST_HACK2	0x02000	/**< Send a HACK(2) notice, reverse */
#define MODEBUF_DEST_HACK3	0x04000	/**< Send a HACK(3) notice, TS == 0 */
#define MODEBUF_DEST_HACK4	0x08000	/**< Send a HACK(4) notice, TS == 0 */

#define MODEBUF_DEST_NOKEY	0x10000	/**< Don't send the real key */

#define MB_TYPE(mb, i)		((mb)->mb_modeargs[(i)].mbm_type)
#define MB_UINT(mb, i)		((mb)->mb_modeargs[(i)].mbm_arg.mbma_uint)
#define MB_STRING(mb, i)	((mb)->mb_modeargs[(i)].mbm_arg.mbma_string)
#define MB_CLIENT(mb, i)	((mb)->mb_modeargs[(i)].mbm_arg.mbma_client)
#define MB_OPLEVEL(mb, i)       ((mb)->mb_modeargs[(i)].mbm_oplevel)

/** A buffer represeting a list of joins to send */
struct JoinBuf {
  struct Client	       *jb_source;	/**< Source of joins (ie, joiner) */
  struct Client	       *jb_connect;	/**< Connection of joiner */
  unsigned int		jb_type;	/**< Type of join (JOIN or CREATE) */
  char		       *jb_comment;	/**< part comment */
  time_t		jb_create;	/**< Creation timestamp */
  unsigned int		jb_count;	/**< Number of channels */
  unsigned int		jb_strlen;	/**< length so far */
  struct Channel       *jb_channels[MAXJOINARGS];
					/**< channels joined or whatever */
};

#define JOINBUF_TYPE_JOIN	0	/**< send JOINs */
#define JOINBUF_TYPE_CREATE	1	/**< send CREATEs */
#define JOINBUF_TYPE_PART	2	/**< send PARTs */
#define JOINBUF_TYPE_PARTALL	3	/**< send local PARTs, but not remote */

extern struct Channel* GlobalChannelList;
extern int             LocalChanOperMode;


struct ChannelRedirect {
	char oldname[CHANNELLEN + 1];
	char newname[CHANNELLEN + 1];
	struct ChannelRedirect* next;
};

extern int channel_rename(struct Channel* chptr, const char* newname);
extern const char* find_channel_redirect(const char* oldname);
extern void add_channel_redirect(const char* oldname, const char* newname);

/*
 * Proto types
 */
extern void channel_modes(struct Client *cptr, char *mbuf, char *pbuf,
                          int buflen, struct Channel *chptr,
			  struct Membership *member);
extern int set_mode(struct Client* cptr, struct Client* sptr,
                    struct Channel* chptr, int parc, char* parv[],
                    char* mbuf, char* pbuf, char* npbuf, int* badop);
extern void send_hack_notice(struct Client *cptr, struct Client *sptr,
                             int parc, char *parv[], int badop, int mtype);
extern struct Channel *get_channel(struct Client *cptr,
                                   char *chname, ChannelGetType flag);
extern int SetAutoChanModes(struct Channel *chptr);
extern struct Membership* find_member_link(struct Channel * chptr,
                                           const struct Client* cptr);
extern int sub1_from_channel(struct Channel* chptr);
extern int destruct_channel(struct Channel* chptr);
extern void add_user_to_channel(struct Channel* chptr, struct Client* who,
                                unsigned int flags, int oplevel);
extern void make_zombie(struct Membership* member, struct Client* who,
                        struct Client* cptr, struct Client* sptr,
                        struct Channel* chptr);
extern struct Client* find_chasing(struct Client* sptr, const char* user, int* chasing);
void add_invite(struct Client *cptr, struct Channel *chptr);
int number_of_zombies(struct Channel *chptr);

extern int has_channel_permission(struct Membership* setter, struct Membership* target, unsigned int required_flag);

extern const char* find_no_nickchange_channel(struct Client* cptr);
extern struct Membership* find_channel_member(struct Client* cptr, struct Channel* chptr);
extern int is_privileged_member(struct Membership* member);
extern int is_privileged_user(struct Client* cptr, struct Channel* chptr);
extern int member_can_send_to_channel(struct Membership* member, int reveal);
extern int client_can_send_to_channel(struct Client *cptr, struct Channel *chptr, int reveal);

extern void remove_user_from_channel(struct Client *sptr, struct Channel *chptr);
extern void remove_user_from_all_channels(struct Client* cptr);

extern int is_chan_op(struct Client *cptr, struct Channel *chptr);
extern int is_zombie(struct Client *cptr, struct Channel *chptr);
extern int has_voice(struct Client *cptr, struct Channel *chptr);
/*
   NOTE: pointer is compared, and not dereferenced, called by
   add_target with a void*, since targets could be anything,
   this function can't make any assumptions that it has a channel
*/
extern int IsInvited(struct Client* cptr, const void* chptr);
extern const char* NetSecureChannelID(const char* name);
extern int NetSecureChannelIDExists(const char* id);
extern void GenerateNetSecureChannelID(char* out_id, size_t out_size);
extern const char* get_channel_redirect(const char* newname);
extern int IsNetSecureChannelName(const char* name);
extern void send_channel_modes(struct Client *cptr, struct Channel *chptr);
extern char *pretty_mask(char *mask);
extern void del_invite(struct Client *cptr, struct Channel *chptr);
extern void list_set_default(void); /* this belongs elsewhere! */

extern void RevealDelayedJoinIfNeeded(struct Client *sptr, struct Channel *chptr);
extern void RevealDelayedJoin(struct Membership *member);
extern void CheckDelayedJoins(struct Channel *chan);

extern void modebuf_init(struct ModeBuf *mbuf, struct Client *source,
			 struct Client *connect, struct Channel *chan,
			 unsigned int dest);
extern void modebuf_mode(struct ModeBuf *mbuf, unsigned int mode);
extern void modebuf_mode_uint(struct ModeBuf *mbuf, unsigned int mode,
			      unsigned int uint);
extern void modebuf_mode_string(struct ModeBuf *mbuf, unsigned int mode,
				char *string, int free);
extern void modebuf_mode_client(struct ModeBuf *mbuf, unsigned int mode,
				struct Client *client, int oplevel);
extern int modebuf_flush(struct ModeBuf *mbuf);
extern void modebuf_extract(struct ModeBuf *mbuf, char *buf);

extern void mode_ban_invalidate(struct Channel *chan);
extern void mode_invite_clear(struct Channel *chan);

extern int mode_parse(struct ModeBuf *mbuf, struct Client *cptr,
		      struct Client *sptr, struct Channel *chptr,
		      int parc, char *parv[], unsigned int flags,
		      struct Membership* member);

#define MODE_PARSE_SET		0x01	/**< actually set channel modes */
#define MODE_PARSE_STRICT	0x02	/**< +m +n +t style not supported */
#define MODE_PARSE_FORCE	0x04	/**< force the mode to be applied */
#define MODE_PARSE_BOUNCE	0x08	/**< we will be bouncing the modes */
#define MODE_PARSE_NOTOPER	0x10	/**< send "not chanop" to user */
#define MODE_PARSE_NOTMEMBER	0x20	/**< send "not member" to user */
#define MODE_PARSE_WIPEOUT	0x40	/**< wipe out +k and +l during burst */
#define MODE_PARSE_BURST	0x80	/**< be even more strict w/extra args */

extern void joinbuf_init(struct JoinBuf *jbuf, struct Client *source,
			 struct Client *connect, unsigned int type,
			 char *comment, time_t create);
extern void joinbuf_join(struct JoinBuf *jbuf, struct Channel *chan,
			 unsigned int flags);
extern int joinbuf_flush(struct JoinBuf *jbuf);
extern struct Ban *make_ban(const char *banstr);
extern struct Ban *find_ban(struct Client *cptr, struct Ban *banlist);
extern int apply_ban(struct Ban **banlist, struct Ban *newban, int free);
extern void free_ban(struct Ban *ban);

#endif /* INCLUDED_channel_h */
