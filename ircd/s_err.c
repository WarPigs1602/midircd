/*
 * IRC - Internet Relay Chat, ircd/s_err.c
 * Copyright (C) 1992 Darren Reed
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
 */
/** @file
 * @brief Error handling support.
 * @version $Id: s_err.c,v 1.72.2.4 2007/02/25 14:32:41 entrope Exp $
 */
#include "config.h"

#include "numeric.h"
#include "ircd_log.h"
#include "s_debug.h"

/* #include <assert.h> -- Now using assert in ircd_log.h */
#include <string.h>

/** Array of Numeric replies, indexed by numeric. */
static Numeric replyTable[] = {
/* 000 */
  { 0 },
/* 001 */
  { RPL_WELCOME, ":Welcome to the %s IRC Network%s%s, %s", "001" },
/* 002 */
  { RPL_YOURHOST, ":Your host is %s, running version %s", "002" },
/* 003 */
  { RPL_CREATED, ":This server was created %s", "003" },
/* 004 */
  { RPL_MYINFO, "%s %s %s %s %s", "004" },
/* 005 */
  { RPL_ISUPPORT, "%s :are supported by this server", "005" },
/* 006 */
  { 0 },
/* 007 */
  { 0 },
/* 008 */
  { RPL_SNOMASK, "%u :: Server notice mask (%#x)", "008" },
/* 009 */
  { 0 },
/* 010 */
  { 0 },
/* 011 */
  { 0 },
/* 012 */
  { 0 },
/* 013 */
  { 0 },
/* 014 */
  { 0 },
/* 015 */
  { RPL_MAP, ":%s%s%s %s [%u clients]", "015" },
/* 016 */
  { RPL_MAPMORE, ":%s%s --> *more*", "016" },
/* 017 */
  { RPL_MAPEND, ":End of /MAP", "017" },
/* 018 */
  { 0 },
/* 019 */
  { 0 },
/* 020 */
  { 0 },
/* 021 */
  { 0 },
/* 022 */
  { 0 },
/* 023 */
  { 0 },
/* 024 */
  { 0 },
/* 025 */
  { 0 },
/* 026 */
  { 0 },
/* 027 */
  { 0 },
/* 028 */
  { 0 },
/* 029 */
  { 0 },
/* 030 */
  { RPL_APASSWARN_SET, ":Channel Admin password (+A) set to '%s'.  Are you SURE you want to use this as Admin password? You will NOT be able to change this password anymore once the channel is more than 48 hours old!", "030" },
/* 031 */
  { RPL_APASSWARN_SECRET, ":Use \"/MODE %s -A %s\" to remove the password and then immediately set a new one.  IMPORTANT: YOU CANNOT RECOVER THIS PASSWORD, EVER; WRITE THE PASSWORD DOWN (don't store this rescue password on disk)! Now set the channel user password (+U).", "031" },
/* 032 */
  { RPL_APASSWARN_CLEAR, ":WARNING: You removed the channel Admin password (+A). If you disconnect or leave the channel without setting a new password then you will not be able to set it again!  SET A NEW PASSWORD NOW!", "032" },
/* 033 */
  { 0 },
/* 034 */
  { 0 },
/* 035 */
  { 0 },
/* 036 */
  { 0 },
/* 037 */
  { 0 },
/* 038 */
  { 0 },
/* 039 */
  { 0 },
/* 040 */
  { 0 },
/* 041 */
  { 0 },
/* 042 */
  { 0 },
/* 043 */
  { 0 },
/* 044 */
  { 0 },
/* 045 */
  { 0 },
/* 046 */
  { 0 },
/* 047 */
  { 0 },
/* 048 */
  { 0 },
/* 049 */
  { 0 },
/* 050 */
  { 0 },
/* 051 */
  { 0 },
/* 052 */
  { 0 },
/* 053 */
  { 0 },
/* 054 */
  { 0 },
/* 055 */
  { 0 },
/* 056 */
  { 0 },
/* 057 */
  { 0 },
/* 058 */
  { 0 },
/* 059 */
  { 0 },
/* 060 */
  { 0 },
/* 061 */
  { 0 },
/* 062 */
  { 0 },
/* 063 */
  { 0 },
/* 064 */
  { 0 },
/* 065 */
  { 0 },
/* 066 */
  { 0 },
/* 067 */
  { 0 },
/* 068 */
  { 0 },
/* 069 */
  { 0 },
/* 070 */
  { 0 },
/* 071 */
  { 0 },
/* 072 */
  { 0 },
/* 073 */
  { 0 },
/* 074 */
  { 0 },
/* 075 */
  { 0 },
/* 076 */
  { 0 },
/* 077 */
  { 0 },
/* 078 */
  { 0 },
/* 079 */
  { 0 },
/* 080 */
  { 0 },
/* 081 */
  { 0 },
/* 082 */
  { 0 },
/* 083 */
  { 0 },
/* 084 */
  { 0 },
/* 085 */
  { 0 },
/* 086 */
  { 0 },
/* 087 */
  { 0 },
/* 088 */
  { 0 },
/* 089 */
  { 0 },
/* 090 */
  { 0 },
/* 091 */
  { 0 },
/* 092 */
  { 0 },
/* 093 */
  { 0 },
/* 094 */
  { 0 },
/* 095 */
  { 0 },
/* 096 */
  { 0 },
/* 097 */
  { 0 },
/* 098 */
  { 0 },
/* 099 */
  { 0 },
/* 100 */
  { 0 },
/* 101 */
  { 0 },
/* 102 */
  { 0 },
/* 103 */
  { 0 },
/* 104 */
  { 0 },
/* 105 */
  { 0 },
/* 106 */
  { 0 },
/* 107 */
  { 0 },
/* 108 */
  { 0 },
/* 109 */
  { 0 },
/* 110 */
  { 0 },
/* 111 */
  { 0 },
/* 112 */
  { 0 },
/* 113 */
  { 0 },
/* 114 */
  { 0 },
/* 115 */
  { 0 },
/* 116 */
  { 0 },
/* 117 */
  { 0 },
/* 118 */
  { 0 },
/* 119 */
  { 0 },
/* 120 */
  { 0 },
/* 121 */
  { 0 },
/* 122 */
  { 0 },
/* 123 */
  { 0 },
/* 124 */
  { 0 },
/* 125 */
  { 0 },
/* 126 */
  { 0 },
/* 127 */
  { 0 },
/* 128 */
  { 0 },
/* 129 */
  { 0 },
/* 130 */
  { 0 },
/* 131 */
  { 0 },
/* 132 */
  { 0 },
/* 133 */
  { 0 },
/* 134 */
  { 0 },
/* 135 */
  { 0 },
/* 136 */
  { 0 },
/* 137 */
  { 0 },
/* 138 */
  { 0 },
/* 139 */
  { 0 },
/* 140 */
  { 0 },
/* 141 */
  { 0 },
/* 142 */
  { 0 },
/* 143 */
  { 0 },
/* 144 */
  { 0 },
/* 145 */
  { 0 },
/* 146 */
  { 0 },
/* 147 */
  { 0 },
/* 148 */
  { 0 },
/* 149 */
  { 0 },
/* 150 */
  { 0 },
/* 151 */
  { 0 },
/* 152 */
  { 0 },
/* 153 */
  { 0 },
/* 154 */
  { 0 },
/* 155 */
  { 0 },
/* 156 */
  { 0 },
/* 157 */
  { 0 },
/* 158 */
  { 0 },
/* 159 */
  { 0 },
/* 160 */
  { 0 },
/* 161 */
  { 0 },
/* 162 */
  { 0 },
/* 163 */
  { 0 },
/* 164 */
  { 0 },
/* 165 */
  { 0 },
/* 166 */
  { 0 },
/* 167 */
  { 0 },
/* 168 */
  { 0 },
/* 169 */
  { 0 },
/* 170 */
  { 0 },
/* 171 */
  { 0 },
/* 172 */
  { 0 },
/* 173 */
  { 0 },
/* 174 */
  { 0 },
/* 175 */
  { 0 },
/* 176 */
  { 0 },
/* 177 */
  { 0 },
/* 178 */
  { 0 },
/* 179 */
  { 0 },
/* 180 */
  { 0 },
/* 181 */
  { 0 },
/* 182 */
  { 0 },
/* 183 */
  { 0 },
/* 184 */
  { 0 },
/* 185 */
  { 0 },
/* 186 */
  { 0 },
/* 187 */
  { 0 },
/* 188 */
  { 0 },
/* 189 */
  { 0 },
/* 190 */
  { 0 },
/* 191 */
  { 0 },
/* 192 */
  { 0 },
/* 193 */
  { 0 },
/* 194 */
  { 0 },
/* 195 */
  { 0 },
/* 196 */
  { 0 },
/* 197 */
  { 0 },
/* 198 */
  { 0 },
/* 199 */
  { 0 },
/* 200 */
  { RPL_TRACELINK, "Link %s.%s %s %s", "200" },
/* 201 */
  { RPL_TRACECONNECTING, "Try. %s %s", "201" },
/* 202 */
  { RPL_TRACEHANDSHAKE, "H.S. %s %s", "202" },
/* 203 */
  { RPL_TRACEUNKNOWN, "???? %s %s", "203" },
/* 204 */
  { RPL_TRACEOPERATOR, "Oper %s %s %ld", "204" },
/* 205 */
  { RPL_TRACEUSER, "User %s %s %ld", "205" },
/* 206 */
  { RPL_TRACESERVER, "Serv %s %dS %dC %s %s!%s@%s %ld %ld", "206" },
/* 207 */
  { 0 },
/* 208 */
  { RPL_TRACENEWTYPE, "<newtype> 0 %s", "208" },
/* 209 */
  { RPL_TRACECLASS, "Class %s %u", "209" },
/* 210 */
  { 0 },
/* 211 */
  { RPL_STATSLINKINFO, 0, "211" },
/* 212 */
  { RPL_STATSCOMMANDS, "%s %u %u", "212" },
/* 213 */
  { RPL_STATSCLINE, "C %s * %d %d %s %s", "213" },
/* 214 */
  { 0 },
/* 215 */
  { RPL_STATSILINE, "I %s%s%s %d %s%s %d %s", "215" },
/* 216 */
  { RPL_STATSKLINE, "%c %s@%s \"%s\" \"%s\" 0 0", "216" },
/* 217 */
  { RPL_STATSPLINE, "P %d %d %s %s", "217" },
/* 218 */
  { RPL_STATSYLINE, "%c %s %d %d %u %u %u", "218" },
/* 219 */
  { RPL_ENDOFSTATS, "%s :End of /STATS report", "219" },
/* 220 */
  { RPL_STATSWLINE, "W %s %d :%s", "220" },
/* 221 */
  { RPL_UMODEIS, "%s", "221" },
/* 222 */
  { RPL_STATSJLINE, "J %s", "222" },
/* 223 */
  { 0 },
/* 224 */
  { 0 },
/* 225 */
  { 0 },
/* 226 */
  { RPL_STATSALINE, "%s", "226" },
/* 227 */
  { 0 },
/* 228 */
  { RPL_STATSQLINE, "Q %s :%s", "228" },
/* 229 */
  { 0 },
/* 230 */
  { 0 },
/* 231 */
  { 0 },
/* 232 */
  { 0 },
/* 233 */
  { 0 },
/* 234 */
  { 0 },
/* 235 */
  { 0 },
/* 236 */
  { RPL_STATSVERBOSE, "V :Sent as explicit", "236" },
/* 237 */
  { RPL_STATSENGINE, "%s :Event loop engine", "237" },
/* 238 */
  { RPL_STATSFLINE, "F %s %s", "238" },
/* 239 */
  { 0 },
/* 240 */
  { 0 },
/* 241 */
  { RPL_STATSLLINE, "Module Description EntryPoint", "241" },
/* 242 */
  { RPL_STATSUPTIME, ":Server Up %d days, %d:%02d:%02d", "242" },
/* 243 */
  { RPL_STATSOLINE, "%c %s@%s * %s %s", "243" },
/* 244 */
  { 0 },
/* 245 */
  { 0 },
/* 246 */
  { RPL_STATSTLINE, "%c %s %s", "246" },
/* 247 */
  { RPL_STATSGLINE, "%c %s%s%s%s%s %Tu %c :%s", "247" },
/* 248 */
  { RPL_STATSULINE, "U %s", "248" },
/* 249 */
  { RPL_STATSDEBUG, 0, "249" },
/* 250 */
  { RPL_STATSCONN, ":Highest connection count: %u (%u clients)", "250" },
/* 251 */
  { RPL_LUSERCLIENT, ":There are %u users and %u invisible on %u servers", "251" },
/* 252 */
  { RPL_LUSEROP, "%u :operator(s) online", "252" },
/* 253 */
  { RPL_LUSERUNKNOWN, "%u :unknown connection(s)", "253" },
/* 254 */
  { RPL_LUSERCHANNELS, "%u :channels formed", "254" },
/* 255 */
  { RPL_LUSERME, ":I have %u clients and %u servers", "255" },
/* 256 */
  { RPL_ADMINME, ":Administrative info about %s", "256" },
/* 257 */
  { RPL_ADMINLOC1, ":%s", "257" },
/* 258 */
  { RPL_ADMINLOC2, ":%s", "258" },
/* 259 */
  { RPL_ADMINEMAIL, ":%s", "259" },
/* 260 */
  { 0 },
/* 261 */
  { 0 },
/* 262 */
  { RPL_TRACEEND, ":End of TRACE", "262" },
/* 263 */
  { 0 },
/* 264 */
  { 0 },
/* 265 */
  { 0 },
/* 266 */
  { 0 },
/* 267 */
  { 0 },
/* 268 */
  { 0 },
/* 269 */
  { 0 },
/* 270 */
  { RPL_PRIVS, "%s :", "270" },
/* 271 */
  { RPL_SILELIST, "%s %s%s", "271" },
/* 272 */
  { RPL_ENDOFSILELIST, "%s :End of Silence List", "272" },
/* 273 */
  { 0 },
/* 274 */
  { 0 },
/* 275 */
  { RPL_STATSDLINE, "%c %s %s", "275" },
/* 276 */
  { RPL_STATSRLINE, "%-9s %-9s %-10s %s", "276" },
/* 277 */
  { 0 },
/* 278 */
  { 0 },
/* 279 */
  { 0 },
/* 280 */
  { RPL_GLIST, "%s%s%s%s%s %Tu %s %c :%s", "280" },
/* 281 */
  { RPL_ENDOFGLIST, ":End of G-line List", "281" },
/* 282 */
  { RPL_JUPELIST, "%s %Tu %s %c :%s", "282" },
/* 283 */
  { RPL_ENDOFJUPELIST, ":End of Jupe List", "283" },
/* 284 */
  { RPL_FEATURE, 0, "284" },
/* 285 */
  { RPL_NEWHOSTIS, "%s: %s host %s - [%s@%s]" },
/* 286 */
  { RPL_CHKHEAD, ":Information for %s %s", "286" },
/* 287 */
  { RPL_CHANUSER, ":    %s%s (%s@%s) %s  (%s) %s", "287" },
/* 288 */
  { 0 },
/* 289 */
  { 0 },
/* 290 */
  { RPL_DATASTR, ":%s", "290" },
/* 291 */
  { RPL_ENDOFCHECK, ":%s", "291" },
/* 292 */
  { 0 },
/* 293 */
  { 0 },
/* 294 */
  { 0 },
/* 295 */
  { 0 },
/* 296 */
  { 0 },
/* 297 */
  { 0 },
/* 298 */
  { 0 },
/* 299 */
  { 0 },
/* 300 */
  { 0 },
/* 301 */
  { RPL_AWAY, "%s :%s", "301" },
/* 302 */
  { RPL_USERHOST, ":", "302" },
/* 303 */
  { RPL_ISON, ":", "303" },
/* 304 */
  { RPL_TEXT, "%s", "304" },
/* 305 */
  { RPL_UNAWAY, ":You are no longer marked as being away", "305" },
/* 306 */
  { RPL_NOWAWAY, ":You have been marked as being away", "306" },
/* 307 */
  { 0 },
/* 308 */
  { 0 },
/* 309 */
  { 0 },
/* 310 */
  { 0 },
/* 311 */
  { RPL_WHOISUSER, "%s %s %s * :%s", "311" },
/* 312 */
  { RPL_WHOISSERVER, "%s %s :%s", "312" },
/* 313 */
  { RPL_WHOISOPERATOR, "%s :is an IRC Operator", "313" },
/* 314 */
  { RPL_WHOWASUSER, "%s %s %s * :%s", "314" },
/* 315 */
  { RPL_ENDOFWHO, "%s :End of /WHO list.", "315" },
/* 316 */
  { 0 },
/* 317 */
  { RPL_WHOISIDLE, "%s %ld %ld :seconds idle, signon time", "317" },
/* 318 */
  { RPL_ENDOFWHOIS, "%s :End of /WHOIS list.", "318" },
/* 319 */
  { RPL_WHOISCHANNELS, "%s :%s", "319" },
/* 320 */
  { RPL_WHOISWEBIRC, "%s :is connected via %s", "320" },
/* 321 */
  { RPL_LISTSTART, "Channel :Users  Name", "321" },
/* 322 */
  { RPL_LIST, "%s %u :%s", "322" },
/* 323 */
  { RPL_LISTEND, ":End of /LIST", "323" },
/* 324 */
  { RPL_CHANNELMODEIS, "%s %s %s", "324" },
/* 325 */
  { 0 },
/* 326 */
  { 0 },
/* 327 */
  { 0 },
/* 328 */
  { 0 },
/* 329 */
  { RPL_CREATIONTIME, "%s %Tu", "329" },
/* 330 */
  { RPL_WHOISACCOUNT, "%s %s :is authed as", "330" },
/* 331 */
  { RPL_NOTOPIC, "%s :No topic is set.", "331" },
/* 332 */
  { RPL_TOPIC, "%s :%s", "332" },
/* 333 */
  { RPL_TOPICWHOTIME, "%s %s %Tu", "333" },
/* 334 */
  { RPL_LISTUSAGE, ":%s", "334" },
/* 335 */
  { 0 },
/* 336 */
  { 0 },
/* 337 */
  { 0 },
/* 338 */
  { RPL_WHOISACTUALLY, "%s %s@%s %s :Actual user@host, Actual IP", "338" },
/* 339 */
  { 0 },
/* 340 */
  { RPL_USERIP, ":", "340" },
/* 341 */
  { RPL_INVITING, "%s %s", "341" },
/* 342 */
  { 0 },
/* 343 */
  { RPL_WHOISOPERNAME, "%s %s :is opered as", "343" },
/* 344 */
  { 0 },
/* 345 */
  { RPL_ISSUEDINVITE, "%s %s %s :%s has been invited by %s", "345" },
/* 346 */
  { RPL_INVITELIST, ":%s", "346" },
/* 347 */
  { RPL_ENDOFINVITELIST, ":End of Invite List", "347" },
/* 348 */
  { RPL_EXCEPTLIST, "%s %s %s %Tu", "348" },
/* 349 */
  { RPL_ENDOFEXCEPTLIST, "%s :End of Channel Ban Exception List", "349" },
/* 350 */
  { 0 },
/* 351 */
  { RPL_VERSION, "%s.%s %s :%s", "351" },
/* 352 */
  { RPL_WHOREPLY, "%s", "352" },
/* 353 */
  { RPL_NAMREPLY, "%s", "353" },
/* 354 */
  { RPL_WHOSPCRPL, "%s", "354" },
/* 355 */
  { RPL_DELNAMREPLY, "%s", "355" },
/* 356 */
  { 0 },
/* 357 */
  { 0 },
/* 358 */
  { 0 },
/* 359 */
  { 0 },
/* 360 */
  { 0 },
/* 361 */
  { 0 },
/* 362 */
  { RPL_CLOSING, "%s :Operator enforced Close", "362" },
/* 363 */
  { RPL_CLOSEEND, "%d :Connections Closed", "363" },
/* 364 */
  { RPL_LINKS, "%s %s :%u P%u %s", "364" },
/* 365 */
  { RPL_ENDOFLINKS, "%s :End of /LINKS list.", "365" },
/* 366 */
  { RPL_ENDOFNAMES, "%s :End of /NAMES list.", "366" },
/* 367 */
  { RPL_BANLIST, "%s %s %s %Tu", "367" },
/* 368 */
  { RPL_ENDOFBANLIST, "%s :End of Channel Ban List", "368" },
/* 369 */
  { RPL_ENDOFWHOWAS, "%s :End of WHOWAS", "369" },
/* 370 */
  { 0 },
/* 371 */
  { RPL_INFO, ":%s", "371" },
/* 372 */
  { RPL_MOTD, ":- %s", "372" },
/* 373 */
  { 0 },
/* 374 */
  { RPL_ENDOFINFO, ":End of /INFO list.", "374" },
/* 375 */
  { RPL_MOTDSTART, ":- %s Message of the Day - ", "375" },
/* 376 */
  { RPL_ENDOFMOTD, ":End of /MOTD command.", "376" },
/* 377 */
  { 0 },
/* 378 */
  { 0 },
/* 379 */
  { 0 },
/* 380 */
  { 0 },
/* 381 */
  { RPL_YOUREOPER, ":You are now an IRC Operator", "381" },
/* 382 */
  { RPL_REHASHING, "%s :Rehashing", "382" },
/* 383 */
  { 0 },
/* 384 */
  { 0 },
/* 385 */
  { 0 },
/* 386 */
  { 0 },
/* 387 */
  { 0 },
/* 388 */
  { 0 },
/* 389 */
  { 0 },
/* 390 */
  { 0 },
/* 391 */
  { RPL_TIME, "%s %Tu %ld :%s", "391" },
/* 392 */
  { 0 },
/* 393 */
  { 0 },
/* 394 */
  { 0 },
/* 395 */
  { 0 },
/* 396 */
  { RPL_HOSTHIDDEN, "%s :is now your hidden host", "396" },
/* 397 */
  { 0 },
/* 398 */
  { RPL_STATSSLINE, "%d %s %s %s %s", "398" },
/* 399 */
  { RPL_USINGSLINE, ":Using S-line privilege", "399" },
/* 400 */
  { 0 },
/* 401 */
  { ERR_NOSUCHNICK, "%s :No such nick", "401" },
/* 402 */
  { ERR_NOSUCHSERVER, "%s :No such server", "402" },
/* 403 */
  { ERR_NOSUCHCHANNEL, "%s :No such channel", "403" },
/* 404 */
  { ERR_CANNOTSENDTOCHAN, "%s :Cannot send to channel", "404" },
/* 405 */
  { ERR_TOOMANYCHANNELS, "%s :You have joined too many channels", "405" },
/* 406 */
  { ERR_WASNOSUCHNICK, "%s :There was no such nickname", "406" },
/* 407 */
  { ERR_TOOMANYTARGETS, "%s :Duplicate recipients. No message delivered", "407" },
/* 408 */
  { ERR_SEARCHNOMATCH, ":%s %s No matching record(s) found", "408" },
/* 409 */
  { ERR_NOORIGIN, ":No origin specified", "409" },
/* 410 */
  { ERR_UNKNOWNCAPCMD, "%s :Unknown CAP subcommand", "410" },
/* 411 */
  { ERR_NORECIPIENT, ":No recipient given (%s)", "411" },
/* 412 */
  { ERR_NOTEXTTOSEND, ":No text to send", "412" },
/* 413 */
  { ERR_NOTOPLEVEL, "%s :No toplevel domain specified", "413" },
/* 414 */
  { ERR_WILDTOPLEVEL, "%s :Wildcard in toplevel Domain", "414" },
/* 415 */
  { 0 },
/* 416 */
  { ERR_QUERYTOOLONG, "%s :Too many lines in the output, restrict your query", "416" },
/* 417 */
  { ERR_INPUTTOOLONG, ":Input line was too long", "417" },
/* 418 */
  { 0 },
/* 419 */
  { 0 },
/* 420 */
  { 0 },
/* 421 */
  { ERR_UNKNOWNCOMMAND, "%s :Unknown command", "421" },
/* 422 */
  { ERR_NOMOTD, ":MOTD File is missing", "422" },
/* 423 */
  { ERR_NOADMININFO, "%s :No administrative info available", "423" },
/* 424 */
  { 0 },
/* 425 */
  { 0 },
/* 426 */
  { 0 },
/* 427 */
  { 0 },
/* 428 */
  { 0 },
/* 429 */
  { 0 },
/* 430 */
  { 0 },
/* 431 */
  { ERR_NONICKNAMEGIVEN, ":No nickname given", "431" },
/* 432 */
  { ERR_ERRONEUSNICKNAME, "%s :Erroneous Nickname", "432" },
/* 433 */
  { ERR_NICKNAMEINUSE, "%s :Nickname is already in use.", "433" },
/* 434 */
  { 0 },
/* 435 */
  { 0 },
/* 436 */
  { ERR_NICKCOLLISION, "%s :Nickname collision KILL", "436" },
/* 437 */
  { ERR_BANNICKCHANGE, "%s :Cannot change nickname while banned on channel or channel is moderated", "437" },
/* 438 */
  { ERR_NICKTOOFAST, "%s :Nick change too fast. Please wait %d seconds.", "438" },
/* 439 */
  { ERR_TARGETTOOFAST, "%s :Target change too fast. Please wait %d seconds.", "439" },
/* 440 */
  { ERR_SERVICESDOWN, "%s :Services are currently unavailable.", "440" },
/* 441 */
  { ERR_USERNOTINCHANNEL, "%s %s :They aren't on that channel", "441" },
/* 442 */
  { ERR_NOTONCHANNEL, "%s :You're not on that channel", "442" },
/* 443 */
  { ERR_USERONCHANNEL, "%s %s :is already on channel", "443" },
/* 444 */
  { 0 },
/* 445 */
  { 0 },
/* 446 */
  { 0 },
/* 447 */
  { 0 },
/* 448 */
  { 0 },
/* 449 */
  { 0 },
/* 450 */
  { 0 },
/* 451 */
  { ERR_NOTREGISTERED, ":You have not registered", "451" },
/* 452 */
  { 0 },
/* 453 */
  { 0 },
/* 454 */
  { 0 },
/* 455 */
  { 0 },
/* 456 */
  { 0 },
/* 457 */
  { 0 },
/* 458 */
  { 0 },
/* 459 */
  { 0 },
/* 460 */
  { 0 },
/* 461 */
  { ERR_NEEDMOREPARAMS, "%s :Not enough parameters", "461" },
/* 462 */
  { ERR_ALREADYREGISTRED, ":You may not reregister", "462" },
/* 463 */
  { ERR_NOPERMFORHOST, ":Your host isn't among the privileged", "463" },
/* 464 */
  { ERR_PASSWDMISMATCH, ":Password Incorrect", "464" },
/* 465 */
  { ERR_YOUREBANNEDCREEP, ":You are banned from this server", "465" },
/* 466 */
  { ERR_YOUWILLBEBANNED, "", "466" },
/* 467 */
  { ERR_KEYSET, "%s :Channel key already set", "467" },
/* 468 */
  { ERR_INVALIDUSERNAME, 0, "468" },
/* 469 */
  { ERR_TLSONLYCHAN, "%s :Cannot join channel (+Z)", "469" },
/* 470 */
  { ERR_LINKCHANNEL, ":[Link] Cannot join channel %s (%s) -- transferring you to %s", "470" },
/* 471 */
  { ERR_CHANNELISFULL, "%s :Cannot join channel, Channel is full (+l)", "471" },
/* 472 */
  { ERR_UNKNOWNMODE, "%c :is unknown mode char to me", "472" },
/* 473 */
  { ERR_INVITEONLYCHAN, "%s :Cannot join channel, you must be invited (+i)", "473" },
/* 474 */
  { ERR_BANNEDFROMCHAN, "%s :Cannot join channel, you are banned (+b)", "474" },
/* 475 */
  { ERR_BADCHANNELKEY, "%s :Cannot join channel, you need the correct key (+k)", "475" },
/* 476 */
  { ERR_BADCHANMASK, "%s :Bad Channel Mask", "476" },
/* 477 */
  { ERR_NEEDREGGEDNICK, "%s :Cannot join channel (+r): this channel requires authentication -- For details of how to obtain an account visit %s", "477" },
/* 478 */
  { ERR_BANLISTFULL, "%s %s :Channel ban/ignore list is full", "478" },
/* 479 */
  { ERR_BADCHANNAME, "%s :Cannot join channel (G-lined: %s)", "479" },
/* 480 */
  { 0 },
/* 481 */
  { ERR_NOPRIVILEGES, ":Permission Denied: Insufficient privileges", "481" },
/* 482 */
  { ERR_CHANOPRIVSNEEDED, "%s :You're not channel operator", "482" },
/* 483 */
  { ERR_CANTKILLSERVER, ":You cant kill a server!", "483" },
/* 484 */
  { ERR_ISCHANSERVICE, "%s %s :Cannot kill, kick or deop an IRC operator", "484" },
/* 485 */
  { ERR_ISREALSERVICE, "%s %s :Cannot kill, kick or deop a network service", "485" },
/* 486 */
  { ERR_ACCOUNTONLY, "%s :You must be authed in order to message this user -- For details of how to obtain an account visit %s", "486" },
/* 487 */
  { ERR_UNIQOPRIVSNEEDED, "%s :You’re not the original channel operator", "487" },
/* 488 */
  { ERR_NOANON, "%s :The requested channel doesn't support anonymous channel mode", "488" },
/* 489 */
  { ERR_VOICENEEDED, "%s :You're neither voiced nor channel operator", "489" },
/* 490 */
  { ERR_LINKSET, "%s %s :Cannot add channel (%s)", "490" },
/* 491 */
  { ERR_NOOPERHOST, ":No Operator block for your host", "491" },
/* 492 */
  { 0 },
/* 493 */
  { ERR_NOFEATURE, "%s :No such feature", "493" },
/* 494 */
  { ERR_BADFEATVALUE, "%s :Bad value for feature %s", "494" },
/* 495 */
  { ERR_BADLOGTYPE, "%s :No such log type", "495" },
/* 496 */
  { ERR_BADLOGSYS, "%s :No such log subsystem", "496" },
/* 497 */
  { ERR_BADLOGVALUE, "%s :Bad value for log type", "497" },
/* 498 */
  { ERR_ISOPERLCHAN, "%s %s :Cannot kick or deop an IRC Operator on a local channel", "498" },
/* 499 */
  { 0 },
/* 500 */
  { 0 },
/* 501 */
  { ERR_UMODEUNKNOWNFLAG, "%c :Unknown user MODE flag", "501" },
/* 502 */
  { ERR_USERSDONTMATCH, ":Cant change mode for other users", "502" },
/* 503 */
  { 0 },
/* 504 */
  { 0 },
/* 505 */
  { 0 },
/* 506 */
  { 0 },
/* 507 */
  { 0 },
/* 508 */
  { 0 },
/* 509 */
  { 0 },
/* 510 */
  { 0 },
/* 511 */
  { ERR_SILELISTFULL, "%s :Your silence list is full", "511" },
/* 512 */
  { ERR_NOSUCHGLINE, "%s :No such gline", "512" },
/* 513 */
  { ERR_BADPING, 0, "513" },
/* 514 */
  { ERR_NOSUCHJUPE, "%s :No such jupe", "514" },
/* 515 */
  { ERR_BADEXPIRE, "%Tu :Bad expire time", "515" },
/* 516 */
  { ERR_DONTCHEAT, "%s :Don't Cheat.", "516" },
/* 517 */
  { ERR_DISABLED, "%s :Command disabled.", "517" },
/* 518 */
  { ERR_LONGMASK, " :Mask is too long", "518" },
/* 519 */
  { ERR_TOOMANYUSERS, "%d :Too many users affected by mask", "519" },
/* 520 */
  { ERR_MASKTOOWIDE, "%s :Mask is too wide", "520" },
/* 521 */
  { 0 },
/* 522 */
  { 0 },
/* 523 */
  { 0 },
/* 524 */
  { ERR_QUARANTINED, "%s :Channel is quarantined : %s", "524" },
/* 525 */
  { 0 },
/* 526 */
  { 0 },
/* 527 */
  { 0 },
/* 528 */
  { 0 },
/* 529 */
  { 0 },
/* 530 */
  { ERR_BADHOSTMASK, "%s :Invalid username/hostmask", "530" },
/* 531 */
  { ERR_HOSTUNAVAIL, "%s :sethost not found", "531" },
/* 532 */
  { ERR_TLSCLIFINGERPRINT, ":TLS certificate fingerprint did not match", "532" },
/* 533 */
  { ERR_TLSMODE, ":Cannot set mode on %s, you are not connected with TLS", "533" },
/* 534 */
  { 0 },
/* 535 */
  { 0 },
/* 536 */
  { 0 },
/* 537 */
  { 0 },
/* 538 */
  { 0 },
/* 539 */
  { 0 },
/* 540 */
  { 0 },
/* 541 */
  { 0 },
/* 542 */
  { 0 },
/* 543 */
  { 0 },
/* 544 */
  { 0 },
/* 545 */
  { 0 },
/* 546 */
  { 0 },
/* 547 */
  { 0 },
/* 548 */
  { 0 },
/* 549 */
  { 0 },
/* 550 */
  { 0 },
/* 551 */
  { 0 },
/* 552 */
  { 0 },
/* 553 */
  { 0 },
/* 554 */
  { 0 },
/* 555 */
  { 0 },
/* 556 */
  { 0 },
/* 557 */
  { 0 },
/* 558 */
  { 0 },
/* 559 */
  { 0 },
/* 560 */
  { ERR_NOTLOWEROPLEVEL, "%s %s %hu %hu :Cannot %s someone with %s op-level", "560" },
/* 561 */
  { ERR_NOTMANAGER, "%s :You must be channel Admin to add or remove a password. Use /JOIN %s <AdminPass>.", "561" },
/* 562 */
  { ERR_CHANSECURED, "%s :Channel is older than 48 hours and secured. Cannot change Admin pass anymore", "562" },
/* 563 */
  { ERR_UPASSSET, "%s :Cannot remove Admin pass (+A) while User pass (+U) is still set. First use /MODE %s -U <userpass>", "563" },
/* 564 */
  { ERR_UPASSNOTSET, "%s :Cannot set user pass (+U) until Admin pass (+A) is set. First use /MODE %s +A <adminpass>", "564" },
/* 565 */
  { 0 },  
/* 566 */
  { ERR_NOMANAGER, "%s :Re-create the channel. The channel must be completely empty for a period of %s before it can be recreated.", "566" },
/* 567 */
  { ERR_UPASS_SAME_APASS, "%s :Cannot use the same pass for both admin (+A) and user (+U) pass.", "567" },
/* 568 */
  { 0 },
/* 569 */
  { 0 },
/* 570 */
  { 0 },
/* 571 */
  { 0 },
/* 572 */
  { 0 },
/* 573 */
  { 0 },
/* 574 */
  { 0 },
/* 575 */
  { 0 },
/* 576 */
  { 0 },
/* 577 */
  { 0 },
/* 578 */
  { 0 },
/* 579 */
  { 0 },
/* 580 */
  { 0 },
/* 581 */
  { 0 },
/* 582 */
  { 0 },
/* 583 */
  { 0 },
/* 584 */
  { 0 },
/* 585 */
  { 0 },
/* 586 */
  { 0 },
/* 587 */
  { 0 },
/* 588 */
  { 0 },
/* 589 */
  { 0 },
/* 590 */
  { 0 },
/* 591 */
  { 0 },
/* 592 */
  { 0 },
/* 593 */
  { 0 },
/* 594 */
  { 0 },
/* 595 */
  { 0 },
/* 596 */
  { 0 },
/* 597 */
  { 0 },
/* 598 */
  { 0 },
/* 599 */
  { 0 },
/* 600 */
  { 0 },
/* 601 */
  { 0 },
/* 602 */
  { 0 },
/* 603 */
  { 0 },
/* 604 */
  { 0 },
/* 605 */
  { 0 },
/* 606 */
  { 0 },
/* 607 */
  { 0 },
/* 608 */
  { 0 },
/* 609 */
  { 0 },
/* 610 */
  { 0 },
/* 611 */
  { 0 },
/* 612 */
  { 0 },
/* 613 */
  { 0 },
/* 614 */
  { 0 },
/* 615 */
  { 0 },
/* 616 */
  { 0 },
/* 617 */
  { 0 },
/* 618 */
  { 0 },
/* 619 */
  { 0 },
/* 620 */
  { 0 },
/* 621 */
  { 0 },
/* 622 */
  { 0 },
/* 623 */
  { 0 },
/* 624 */
  { 0 },
/* 625 */
  { 0 },
/* 626 */
  { 0 },
/* 627 */
  { 0 },
/* 628 */
  { 0 },
/* 629 */
  { 0 },
/* 630 */
  { 0 },
/* 631 */
  { 0 },
/* 632 */
  { 0 },
/* 633 */
  { 0 },
/* 634 */
  { 0 },
/* 635 */
  { 0 },
/* 636 */
  { 0 },
/* 637 */
  { 0 },
/* 638 */
  { 0 },
/* 639 */
  { 0 },
/* 640 */
  { 0 },
/* 641 */
  { 0 },
/* 642 */
  { 0 },
/* 643 */
  { 0 },
/* 644 */
  { 0 },
/* 645 */
  { 0 },
/* 646 */
  { 0 },
/* 647 */
  { 0 },
/* 648 */
  { 0 },
/* 649 */
  { 0 },
/* 650 */
  { 0 },
/* 651 */
  { 0 },
/* 652 */
  { 0 },
/* 653 */
  { 0 },
/* 654 */
  { 0 },
/* 655 */
  { 0 },
/* 656 */
  { 0 },
/* 657 */
  { 0 },
/* 658 */
  { 0 },
/* 659 */
  { 0 },
/* 660 */
  { 0 },
/* 667 */
  { 0 },
/* 668 */
  { 0 },
/* 669 */
  { 0 },
/* 664 */
  { 0 },
/* 665 */
  { 0 },
/* 666 */
  { 0 },
/* 667 */
  { 0 },
/* 668 */
  { 0 },
/* 669 */
  { 0 },
/* 670 */
  { RPL_STARTTLS, ":STARTTLS successful, proceed with TLS handshake", "670" },
/* 671 */
  { 0 },
/* 672 */
  { 0 },
/* 673 */
  { 0 },
/* 674 */
  { 0 },
/* 675 */
  { 0 },
/* 676 */
  { 0 },
/* 677 */
  { 0 },
/* 678 */
  { 0 },
/* 679 */
  { 0 },
/* 680 */
  { 0 },
/* 681 */
  { 0 },
/* 682 */
  { 0 },
/* 683 */
  { 0 },
/* 684 */
  { 0 },
/* 685 */
  { 0 },
/* 686 */
  { 0 },
/* 687 */
  { 0 },
/* 688 */
  { 0 },
/* 689 */
  { 0 },
/* 690 */
  { 0 },
/* 691 */
  { ERR_STARTTLS, ":STARTTLS failed (%s)", "691" },
/* 692 */
  { 0 },
/* 693 */
  { 0 },
/* 694 */
  { 0 },
/* 695 */
  { 0 },
/* 696 */
  { 0 },
/* 697 */
  { 0 },
/* 698 */
  { 0 },
/* 699 */
  { 0 },
/* 700 */
  { 0 },
/* 701 */
  { 0 },
/* 702 */
  { 0 },
/* 703 */
  { 0 },
/* 704 */
  { 0 },
/* 705 */
  { 0 },
/* 707 */
  { 0 },
/* 707 */
  { 0 },
/* 708 */
  { 0 },
/* 709 */
  { 0 },
/* 710 */
  { 0 },
/* 711 */
  { 0 },
/* 712 */
  { 0 },
/* 713 */
  { 0 },
/* 714 */
  { 0 },
/* 715 */
  { 0 },
/* 717 */
  { 0 },
/* 717 */
  { 0 },
/* 718 */
  { 0 },
/* 719 */
  { 0 },
/* 720 */
  { 0 },
/* 721 */
  { 0 },
/* 722 */
  { 0 },
/* 723 */
  { 0 },
/* 724 */
  { 0 },
/* 725 */
  { 0 },
/* 727 */
  { 0 },
/* 727 */
  { 0 },
/* 728 */
  { 0 },
/* 729 */
  { 0 },
/* 730 */
  { 0 },
/* 731 */
  { 0 },
/* 732 */
  { 0 },
/* 733 */
  { 0 },
/* 734 */
  { 0 },
/* 735 */
  { 0 },
/* 737 */
  { 0 },
/* 737 */
  { 0 },
/* 738 */
  { 0 },
/* 739 */
  { 0 },
/* 740 */
  { 0 },
/* 741 */
  { 0 },
/* 742 */
  { 0 },
/* 743 */
  { 0 },
/* 744 */
  { 0 },
/* 745 */
  { 0 },
/* 747 */
  { 0 },
/* 747 */
  { 0 },
/* 748 */
  { 0 },
/* 749 */
  { 0 },
/* 750 */
  { 0 },
/* 751 */
  { 0 },
/* 752 */
  { 0 },
/* 753 */
  { 0 },
/* 754 */
  { 0 },
/* 755 */
  { 0 },
/* 757 */
  { 0 },
/* 757 */
  { 0 },
/* 758 */
  { 0 },
/* 759 */
  { 0 },
/* 770 */
  { 0 },
/* 777 */
  { 0 },
/* 778 */
  { 0 },
/* 779 */
  { 0 },
/* 774 */
  { 0 },
/* 775 */
  { 0 },
/* 777 */
  { 0 },
/* 777 */
  { 0 },
/* 778 */
  { 0 },
/* 779 */
  { 0 },
/* 770 */
  { 0 },
/* 771 */
  { 0 },
/* 772 */
  { 0 },
/* 773 */
  { 0 },
/* 774 */
  { 0 },
/* 775 */
  { 0 },
/* 777 */
  { 0 },
/* 777 */
  { 0 },
/* 778 */
  { 0 },
/* 779 */
  { 0 },
/* 780 */
  { 0 },
/* 781 */
  { 0 },
/* 782 */
  { 0 },
/* 783 */
  { 0 },
/* 784 */
  { 0 },
/* 785 */
  { 0 },
/* 787 */
  { 0 },
/* 787 */
  { 0 },
/* 788 */
  { 0 },
/* 789 */
  { 0 },
/* 790 */
  { 0 },
/* 791 */
  { 0 },
/* 792 */
  { 0 },
/* 793 */
  { 0 },
/* 794 */
  { 0 },
/* 795 */
  { 0 },
/* 797 */
  { 0 },
/* 797 */
  { 0 },
/* 798 */
  { 0 },
/* 799 */
  { 0 },
/* 800 */
  { 0 },
/* 801 */
  { 0 },
/* 802 */
  { 0 },
/* 803 */
  { 0 },
/* 804 */
  { 0 },
/* 805 */
  { 0 },
/* 808 */
  { 0 },
/* 807 */
  { 0 },
/* 808 */
  { 0 },
/* 809 */
  { 0 },
/* 810 */
  { 0 },
/* 811 */
  { 0 },
/* 812 */
  { 0 },
/* 813 */
  { 0 },
/* 814 */
  { 0 },
/* 815 */
  { 0 },
/* 818 */
  { 0 },
/* 817 */
  { 0 },
/* 818 */
  { 0 },
/* 819 */
  { 0 },
/* 820 */
  { 0 },
/* 821 */
  { 0 },
/* 822 */
  { 0 },
/* 823 */
  { 0 },
/* 824 */
  { 0 },
/* 825 */
  { 0 },
/* 828 */
  { 0 },
/* 827 */
  { 0 },
/* 828 */
  { 0 },
/* 829 */
  { 0 },
/* 830 */
  { 0 },
/* 831 */
  { 0 },
/* 832 */
  { 0 },
/* 833 */
  { 0 },
/* 834 */
  { 0 },
/* 835 */
  { 0 },
/* 838 */
  { 0 },
/* 837 */
  { 0 },
/* 838 */
  { 0 },
/* 839 */
  { 0 },
/* 840 */
  { 0 },
/* 841 */
  { 0 },
/* 842 */
  { 0 },
/* 843 */
  { 0 },
/* 844 */
  { 0 },
/* 845 */
  { 0 },
/* 848 */
  { 0 },
/* 847 */
  { 0 },
/* 848 */
  { 0 },
/* 849 */
  { 0 },
/* 850 */
  { 0 },
/* 851 */
  { 0 },
/* 852 */
  { 0 },
/* 853 */
  { 0 },
/* 854 */
  { 0 },
/* 855 */
  { 0 },
/* 858 */
  { 0 },
/* 857 */
  { 0 },
/* 858 */
  { 0 },
/* 859 */
  { 0 },
/* 880 */
  { 0 },
/* 887 */
  { 0 },
/* 888 */
  { 0 },
/* 889 */
  { 0 },
/* 884 */
  { 0 },
/* 885 */
  { 0 },
/* 888 */
  { 0 },
/* 887 */
  { 0 },
/* 888 */
  { 0 },
/* 889 */
  { 0 },
/* 870 */
  { 0 },
/* 871 */
  { 0 },
/* 872 */
  { 0 },
/* 873 */
  { 0 },
/* 874 */
  { 0 },
/* 875 */
  { 0 },
/* 878 */
  { 0 },
/* 877 */
  { 0 },
/* 878 */
  { 0 },
/* 879 */
  { 0 },
/* 880 */
  { 0 },
/* 881 */
  { 0 },
/* 882 */
  { 0 },
/* 883 */
  { 0 },
/* 884 */
  { 0 },
/* 885 */
  { 0 },
/* 888 */
  { 0 },
/* 887 */
  { 0 },
/* 888 */
  { 0 },
/* 889 */
  { 0 },
/* 890 */
  { 0 },
/* 891 */
  { 0 },
/* 892 */
  { 0 },
/* 893 */
  { 0 },
/* 894 */
  { 0 },
/* 895 */
  { 0 },
/* 898 */
  { 0 },
/* 897 */
  { 0 },
/* 898 */
  { 0 },
/* 899 */
  { 0 },
/* 900 */
  { RPL_LOGGEDIN, "%C %s :You are now logged in as %s", "900" },
/* 901 */
  { RPL_LOGGEDOUT, "%C :You are now logged out", "901" },
/* 902 */
  { ERR_NICKLOCKED, ":You must use a nick assigned to you", "902" },
/* 903 */
  { RPL_SASLSUCCESS, ":SASL authentication successful", "903" },
/* 904 */
  { ERR_SASLFAIL, ":SASL authentication failed", "904" },
/* 905 */
  { ERR_SASLTOOLONG, ":SASL message too long", "905" },
/* 909 */
  { ERR_SASLABORTED, ":SASL authentication aborted", "906" },
/* 907 */
  { ERR_SASLALREADY, ":You have already authenticated using SASL", "907" },
/* 908 */
  { RPL_SASLMECHS, "%s :are available SASL mechanisms", "908" },
/* 909 */
  { 0 },
/* 910 */
  { 0 },
/* 911 */
  { 0 },
/* 912 */
  { 0 },
/* 913 */
  { 0 },
/* 914 */
  { 0 },
/* 915 */
  { 0 },
/* 919 */
  { 0 },
/* 917 */
  { 0 },
/* 918 */
  { 0 },
/* 919 */
  { 0 },
/* 920 */
  { 0 },
/* 921 */
  { 0 },
/* 922 */
  { 0 },
/* 923 */
  { 0 },
/* 924 */
  { 0 },
/* 925 */
  { 0 },
/* 929 */
  { 0 },
/* 927 */
  { 0 },
/* 928 */
  { 0 },
/* 929 */
  { 0 },
/* 930 */
  { 0 },
/* 931 */
  { 0 },
/* 932 */
  { 0 },
/* 933 */
  { 0 },
/* 934 */
  { 0 },
/* 935 */
  { 0 },
/* 939 */
  { 0 },
/* 937 */
  { 0 },
/* 938 */
  { 0 },
/* 939 */
  { 0 },
/* 940 */
  { 0 },
/* 941 */
  { 0 },
/* 942 */
  { 0 },
/* 943 */
  { 0 },
/* 944 */
  { 0 },
/* 945 */
  { 0 },
/* 949 */
  { 0 },
/* 947 */
  { 0 },
/* 948 */
  { 0 },
/* 949 */
  { 0 },
/* 950 */
  { 0 },
/* 951 */
  { 0 },
/* 952 */
  { 0 },
/* 953 */
  { 0 },
/* 954 */
  { 0 },
/* 955 */
  { 0 },
/* 959 */
  { 0 },
/* 957 */
  { 0 },
/* 958 */
  { 0 },
/* 959 */
  { 0 },
/* 990 */
  { 0 },
/* 997 */
  { 0 },
/* 998 */
  { 0 },
/* 999 */
  { 0 },
/* 994 */
  { 0 },
/* 995 */
  { 0 },
/* 999 */
  { 0 },
/* 997 */
  { 0 },
/* 998 */
  { 0 },
/* 999 */
  { 0 },
/* 970 */
  { 0 },
/* 971 */
  { 0 },
/* 972 */
  { 0 },
/* 973 */
  { 0 },
/* 974 */
  { ERR_CANNOTCHANGECHANMODE, "%s :%s", "974" },
/* 975 */
  { 0 },
/* 979 */
  { 0 },
/* 977 */
  { 0 },
/* 978 */
  { 0 },
/* 979 */
  { 0 },
/* 980 */
  { 0 },
/* 981 */
  { 0 },
/* 982 */
  { 0 },
/* 983 */
  { 0 },
/* 984 */
  { 0 },
/* 985 */
  { 0 },
/* 989 */
  { 0 },
/* 987 */
  { 0 },
/* 988 */
  { 0 },
/* 989 */
  { 0 },
/* 990 */
  { 0 },
/* 991 */
  { 0 },
/* 992 */
  { 0 },
/* 993 */
  { 0 },
/* 994 */
  { 0 },
/* 995 */
  { 0 },
/* 999 */
  { 0 },
/* 997 */
  { 0 },
/* 998 */
  { 0 },
/* 999 */
  { 0 }
};

/** Return a pointer to the Numeric for a particular code.
 * @param n %Numeric to look up.
 * @return Numeric structure.
 */
const struct Numeric* get_error_numeric(int n)
{
  assert(0 < n);
  assert(n < ERR_LASTERROR);
  assert(0 != replyTable[n].value);

  return &replyTable[n];
}

/** Return a format string for a numeric response.
 * @param n %Numeric to look up.
 * @return Pointer to a static buffer containing the format string.
 */
char* rpl_str(int n)
{
  static char numbuff[512];
  Numeric* p;

  assert(0 < n);
  assert(n < ERR_LASTERROR);
  assert(0 != replyTable[n].value);

  p = &replyTable[n];
  strcpy(numbuff, ":%s 000 %s ");
  if (p->str) {
    numbuff[4] = p->str[0];
    numbuff[5] = p->str[1];
    numbuff[6] = p->str[2];
    strcpy(numbuff + 11, p->format);
  }

  return numbuff;
}

