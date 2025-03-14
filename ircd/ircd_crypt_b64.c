/*
 * IRC - Internet Relay Chat, ircd/ircd_crypt_smd5.c
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
 
/** 
 * @file
 * @brief Routines for Salted MD5 passwords
 * @version $Id: ircd_crypt_smd5.c,v 1.7 2005/03/20 16:06:17 entrope Exp $
 * 
 * ircd_crypt_smd5 is largely taken from md5_crypt.c from the Linux PAM 
 * source code.  it's been modified to fit in with ircu and some of the 
 * unneeded code has been removed.  the source file md5_crypt.c has the 
 * following license, so if any of our opers or admins are in Denmark
 * they better go buy them a drink ;) -- hikari
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 */
#include "config.h"
#include "ircd_alloc.h"
#include "ircd_crypt_b64.h"
//Decodes Base64
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string.h>
#include <stdio.h>

static int calcDecodeLength(const char* b64input) { //Calculates the length of a decoded base64 string
  int len = strlen(b64input);
  int padding = 0;

  if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len-1] == '=') //last char is =
    padding = 1;

  return (int)len*0.75 - padding;
}

int Base64Decode(char* b64message, char** buffer) { //Decodes a base64 encoded string
  BIO *bio, *b64;
  int decodeLen = calcDecodeLength(b64message),
      len = 0;
  *buffer = (char*)MyMalloc(decodeLen+1);
  FILE* stream = fmemopen(b64message, strlen(b64message), "r");

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  bio = BIO_push(b64, bio);
  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
  len = BIO_read(bio, *buffer, strlen(b64message));
    //Can test here if len == decodeLen - if not, then return an error
  (*buffer)[len] = '\0';

  BIO_free_all(bio);
  fclose(stream);

  return (0); //success
}