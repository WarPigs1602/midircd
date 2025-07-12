/*
 * IRC - Internet Relay Chat, ircd/cloak.c
 * Copyright (C) 1990 Jarkko Oikarinen and
 *                    University of Oulu, Computing Center
 *
 * See file AUTHORS in IRC package for additional names of
 * the programmers.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
 * @brief Implementation of style 2 host hiding
 * @version $Id$
 */
#include "client.h"
#include "cloak.h"
#include "ircd.h"
#include "ircd_alloc.h"
#include "ircd_chattr.h"
#include "ircd_defs.h"
#include "ircd_features.h"
#include "ircd_string.h"
#include "ircd_snprintf.h"
#include "match.h"
#include "md5.h"
#include "s_bsd.h"
#include "s_debug.h"
#include "struct.h"

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define KEY1 feature_str(FEAT_HOST_HIDING_KEY1) /**< Cloak key 1 */
#define KEY2 feature_str(FEAT_HOST_HIDING_KEY2) /**< Cloak key 2 */
#define KEY3 feature_str(FEAT_HOST_HIDING_KEY3) /**< Cloak key 3 */

#define RESULT_BUF_SIZE 128
#define MD5_BUF_SIZE 512


static void to_base36(unsigned int value, char* out, size_t out_size)
{
    const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    char buf[16];
    int i = 0;

    if (out_size < 2) {
        if (out_size > 0) out[0] = '\0';
        return;
    }

    do {
        buf[i++] = digits[value % 36];
        value /= 36;
    } while (value && i < (int)sizeof(buf) - 1);

    int j = 0;
    while (i > 0 && j < (int)out_size - 1)
        out[j++] = buf[--i];
    out[j] = '\0';
}

/** Downsamples a 128bit result to 32bits (md5 -> unsigned int).
 * @param[in] i 128bit result to downsample.
 * @return downsampled result.
 */
static inline unsigned int downsample(const unsigned char *i)
{
    return ((unsigned int)(i[0] ^ i[1] ^ i[2] ^ i[3]) << 24) |
           ((unsigned int)(i[4] ^ i[5] ^ i[6] ^ i[7]) << 16) |
           ((unsigned int)(i[8] ^ i[9] ^ i[10] ^ i[11]) << 8) |
           ((unsigned int)(i[12] ^ i[13] ^ i[14] ^ i[15]));
}


static int cloak_md5(const char* keyA, const char* input, const char* keyB, unsigned int* out)
{
    char buf[MD5_BUF_SIZE];
    unsigned char res[MD5_BUF_SIZE], res2[MD5_BUF_SIZE];
    unsigned long n;

    ircd_snprintf(0, buf, sizeof(buf), "%s:%s:%s", keyA, input, keyB);
    DoMD5(res, (unsigned char*)buf, strlen(buf));
    strncpy((char*)res + 16, keyB, sizeof(res) - 16 - 1);
    n = strlen((char*)res + 16) + 16;
    DoMD5(res2, res, n);
    *out = downsample(res2);
    return 0;
}

/** Cloak an IPv4 IP
 * @param[in] host IP to cloak.
 * @return result Cloaked IP.
 */
static char *hidehost_ipv4(const char *host)
{
    unsigned int a, b, c, d;
    unsigned int alpha, beta, gamma;
    char *result;

    if (sscanf(host, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
        return NULL;

    cloak_md5(KEY2, host, KEY3, &alpha);

    char abc[32];
    snprintf(abc, sizeof(abc), "%u.%u.%u", a, b, c);
    cloak_md5(KEY3, abc, KEY1, &beta);

    char ab[16];
    snprintf(ab, sizeof(ab), "%u.%u", a, b);
    cloak_md5(KEY1, ab, KEY2, &gamma);

    result = malloc(RESULT_BUF_SIZE);
    if (!result)
        return NULL;
    char alpha36[16], beta36[16], gamma36[16];
    to_base36(alpha, alpha36, sizeof(alpha36));
    to_base36(beta, beta36, sizeof(beta36));
    to_base36(gamma, gamma36, sizeof(gamma36));
    ircd_snprintf(0, result, RESULT_BUF_SIZE, "%s.%s.%s.IP", alpha36, beta36, gamma36);

    return result;
}


/** Cloak a hostname
 * @param[in] host to cloak.
 * @return result Cloaked host.
 */
static char *hidehost_normalhost(char *host)
{
char *p;
static char buf[512], res[512], res2[512], result[HOSTLEN+1];
unsigned int alpha, n;

	ircd_snprintf(0, buf, 512, "%s:%s:%s", KEY1, host, KEY2);
	DoMD5(res, buf, strlen(buf));
        strcpy(res+16, KEY3);
	n = strlen(res+16) + 16;
	DoMD5(res2, res, n);
	alpha = downsample(res2);

	for (p = host; *p; p++)
		if (*p == '.')
			if (IsAlpha(*(p + 1)))
				break;

    char alpha36[16];
    to_base36(alpha, alpha36, sizeof(alpha36));
	if (*p)
	{
		unsigned int len;
		p++;

		ircd_snprintf(0, result, HOSTLEN, "%s-%s.", feature_str(FEAT_HOST_HIDING_PREFIX), alpha36);
		len = strlen(result) + strlen(p);
		if (len <= HOSTLEN)
			strcat(result, p);
		else
			strcat(result, p + (len - HOSTLEN));
	} else
		ircd_snprintf(0, result, HOSTLEN, "%s-%X",  feature_str(FEAT_HOST_HIDING_PREFIX), alpha);

	return result;
}

/** Cloak an IPv6 IP
 * @param[in] host IP to cloak.
 * @return result Cloaked IP.
 */
static char *hidehost_ipv6(const char *host)
{
    char buf[512];
    unsigned char res[512], res2[512];
    unsigned long n;
    unsigned int alpha, beta, gamma, delta;
    char *result;

    if (!host || strchr(host, ':') == NULL)
        return NULL;

    // ALPHA
    ircd_snprintf(0, buf, sizeof(buf), "%s:%s:%s", KEY2, host, KEY3);
    DoMD5(res, (unsigned char*) buf, strlen(buf));
    strncpy((char *)res+16, KEY1, sizeof(res)-16-1);
    n = strlen((char *)res+16) + 16;
    DoMD5(res2, res, n);
    alpha = downsample(res2);

    // BETA
    ircd_snprintf(0, buf, sizeof(buf), "%s:%s:%s", KEY3, host, KEY1);
    DoMD5(res, (unsigned char*) buf, strlen(buf));
    strncpy((char *)res+16, KEY2, sizeof(res)-16-1);
    n = strlen((char *)res+16) + 16;
    DoMD5(res2, res, n);
    beta = downsample(res2);

    // GAMMA
    ircd_snprintf(0, buf, sizeof(buf), "%s:%s:%s", KEY1, host, KEY2);
    DoMD5(res, (unsigned char*) buf, strlen(buf));
    strncpy((char *)res+16, KEY3, sizeof(res)-16-1);
    n = strlen((char *)res+16) + 16;
    DoMD5(res2, res, n);
    gamma = downsample(res2);

    // DELTA
    ircd_snprintf(0, buf, sizeof(buf), "%s:%s:%s", KEY2, host, KEY1);
    DoMD5(res, (unsigned char*) buf, strlen(buf));
    strncpy((char *)res+16, KEY3, sizeof(res)-16-1);
    n = strlen((char *)res+16) + 16;
    DoMD5(res2, res, n);
    delta = downsample(res2);

    result = malloc(128);
    if (!result)
        return NULL;
    char alpha36[16], beta36[16], gamma36[16], delta36[16];
    to_base36(alpha, alpha36, sizeof(alpha36));
    to_base36(beta, beta36, sizeof(beta36));
    to_base36(gamma, gamma36, sizeof(gamma36));
    to_base36(delta, delta36, sizeof(delta36));
    ircd_snprintf(0, result, 128, "%s.%s.%s.%s.IPv6", alpha36, beta36, gamma36, delta36);

    return result;
}

char *cloak_host(const char *host)
{
    if (!host)
        return NULL;

    if (strchr(host, ':'))
        return hidehost_ipv6(host);

    unsigned int a, b, c, d;
    if (sscanf(host, "%u.%u.%u.%u", &a, &b, &c, &d) == 4)
        return hidehost_ipv4(host);

    return hidehost_normalhost((char *)host);
}


