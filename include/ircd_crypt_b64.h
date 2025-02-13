/*
 * IRC - Internet Relay Chat, include/ircd_crypt.h
 * Copyright (C) 2002 hikari
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
 * @brief Core password encryption and hashing APIs.
 * @version $Id: ircd_crypt.h,v 1.3 2004/11/07 21:04:58 entrope Exp $
 */
#ifndef INCLUDED_ircd_crypt_b64_h
#define INCLUDED_ircd_crypt_b64_h

#include <stdlib.h>
#include <memory.h>


/***********************************************
Encodes ASCCI string into base64 format string
@param plain ASCII string to be encoded
@return encoded base64 format string
***********************************************/
char* base64_encode(char* plain);


/***********************************************
decodes base64 format string into ASCCI string
@param plain encoded base64 format string
@return ASCII string to be encoded
***********************************************/
char* base64_decode(char* cipher);

#endif /* INCLUDED_ircd_crypt_b64_h */

