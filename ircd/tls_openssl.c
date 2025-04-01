/*
 * IRC - Internet Relay Chat, ircd/tls_openssl.c
 * Copyright (C) 2019 Michael Poole
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
 * @brief ircd TLS functions using OpenSSL.
 */

#include "config.h"
#include "client.h"
#include "ircd_alloc.h"
#include "ircd_features.h"
#include "ircd_log.h"
#include "ircd_string.h"
#include "ircd_tls.h"
#include "listener.h"
#include "s_conf.h"
#include "s_debug.h"

#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>

#include <sys/uio.h> /* IOV_MAX */

#if OPENSSL_VERSION_NUMBER < 0x10100000L
# error ircu2 requires OpenSSL >= 1.1.0
#endif

const char *ircd_tls_version = OPENSSL_VERSION_TEXT;

static SSL_CTX *base_server_ctx;
static SSL_CTX *base_client_ctx;
static const EVP_MD *fp_digest;

static void ssl_log_error(const char *msg)
{
  unsigned long err;
  char buf[BUFSIZE];

  err = ERR_get_error();
  if (err)
  {
    ERR_error_string_n(err, buf, sizeof(buf));
    log_write(LS_SYSTEM, L_ERROR, 0, "OpenSSL %s: %s", msg, buf);

    while ((err = ERR_get_error()) != 0)
    {
      ERR_error_string_n(err, buf, sizeof(buf));
      log_write(LS_SYSTEM, L_ERROR, 0, " ... %s", buf);
    }
  }
  else
  {
    log_write(LS_SYSTEM, L_ERROR, 0, "Unknown OpenSSL failure: %s", msg);
  }
}

void ircd_tls_fingerprint(void *ctx, char *fingerprint)
{
  SSL *tls;
  X509 *cert;
  int res;

  if (!ctx)
  {
  no_fingerprint:
    Debug((DEBUG_DEBUG, "No fingerprint provided by client"));
    memset(fingerprint, 0, 65);
    return;
  }

  unsigned int fp_len = 0;
  unsigned char raw_fp[EVP_MAX_MD_SIZE];
  tls = ctx;
  cert = SSL_get_peer_certificate(tls);
  if (!cert)
    goto no_fingerprint;

  res = X509_digest(cert, fp_digest, raw_fp, &fp_len);

  X509_free(cert);
  if (!res)
    Debug((DEBUG_NOTICE, "X509_digest failed to make fingerprint"));

  for (unsigned int i = 0; i < fp_len; i++) {
      sprintf(&fingerprint[i * 2], "%02x", raw_fp[i]);
  }
  fingerprint[64] = '\0';

Debug((DEBUG_DEBUG, "Generated Fingerprint inside ircd_tls_fingerprint(): %s", fingerprint));
}

static void ssl_set_ciphers(SSL_CTX *ctx, SSL *tls, const char *text)
{
  const char *sep;

  if (!text)
    return;

  sep = strchr(text, ' ');
  if (sep != NULL)
  {
    char *tmp;
#if HAVE_SSL_SET_CIPHERSUITES
    if (ctx)
      SSL_CTX_set_ciphersuites(ctx, sep + 1);
    if (tls)
      SSL_set_ciphersuites(tls, sep + 1);
#endif

    tmp = MyMalloc(sep + 1 - text);
    ircd_strncpy(tmp, text, sep - text);
    if (ctx)
      SSL_CTX_set_cipher_list(ctx, tmp);
    if (tls)
      SSL_set_cipher_list(tls, tmp);
    MyFree(tmp);
  }
  else if (*text != '\0')
  {
    if (ctx)
      SSL_CTX_set_cipher_list(ctx, text);
    if (tls)
      SSL_set_cipher_list(tls, text);
  }
  else
  {
    if (ctx)
      SSL_CTX_set_cipher_list(ctx, SSL_DEFAULT_CIPHER_LIST);
    if (tls)
      SSL_set_cipher_list(tls, SSL_DEFAULT_CIPHER_LIST);
#if HAVE_SSL_SET_CIPHERSUITES
    if (ctx)
      SSL_CTX_set_ciphersuites(ctx, TLS_DEFAULT_CIPHERSUITES);
    if (tls)
      SSL_set_ciphersuites(tls, TLS_DEFAULT_CIPHERSUITES);
#endif
  }
}

int ircd_verify_peer(int preverify_ok, X509_STORE_CTX *ctx) {
    int err = X509_STORE_CTX_get_error(ctx);

    if (err == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT) {
        return 1; // Accept self-signed client certificates TODO: Consider making this configurable?
    }

    return preverify_ok; // Default verification for other errors
}

int ircd_tls_init(void)
{
  static int openssl_init;
  SSL_CTX *server_ctx = NULL, *client_ctx = NULL;
  const char *str, *s_2;
  int res;

  /* Early out if no private key or certificate file was given. */
  if (EmptyString(ircd_tls_keyfile) || EmptyString(ircd_tls_certfile))
    goto done;

  if (!openssl_init)
  {
    openssl_init = 1;
    SSL_library_init();
    SSL_load_error_strings();

    if (!RAND_poll())
    {
      ssl_log_error("RAND_poll failed");
      return 1;
    }

    fp_digest = EVP_sha256();
  }

  server_ctx = SSL_CTX_new(TLS_server_method());
  if (!server_ctx)
  {
    ssl_log_error("SSL_CTX_new (server) failed");
    return 2;
  }

  client_ctx = SSL_CTX_new(TLS_client_method());
  if (!client_ctx)
  {
    ssl_log_error("SSL_CTX_new (client) failed");
    return 2;
  }

  if (!feature_bool(FEAT_TLS_SSLV2)) {
    SSL_CTX_set_options(server_ctx, SSL_OP_NO_SSLv2);
    SSL_CTX_set_options(client_ctx, SSL_OP_NO_SSLv2);
  }
  if (!feature_bool(FEAT_TLS_SSLV3)) {
    SSL_CTX_set_options(server_ctx, SSL_OP_NO_SSLv3);
    SSL_CTX_set_options(client_ctx, SSL_OP_NO_SSLv3);
  }
  if (!feature_bool(FEAT_TLS_V1P0)) {
    SSL_CTX_set_options(server_ctx, SSL_OP_NO_TLSv1);
    SSL_CTX_set_options(client_ctx, SSL_OP_NO_TLSv1);
  }
  if (!feature_bool(FEAT_TLS_V1P1)) {
    SSL_CTX_set_options(server_ctx, SSL_OP_NO_TLSv1_1);
    SSL_CTX_set_options(client_ctx, SSL_OP_NO_TLSv1_1);
  }
  if (!feature_bool(FEAT_TLS_V1P2)) {
    SSL_CTX_set_options(server_ctx, SSL_OP_NO_TLSv1_2);
    SSL_CTX_set_options(client_ctx, SSL_OP_NO_TLSv1_2);
  }

  SSL_CTX_set_verify(server_ctx, SSL_VERIFY_PEER, ircd_verify_peer);
  SSL_CTX_set_verify(client_ctx, SSL_VERIFY_PEER, ircd_verify_peer);

  /* OpenSSL only defines this macro if it supports TLS 1.3. */
#if defined(SSL_OP_NO_TLSv1_3)
  if (!feature_bool(FEAT_TLS_V1P3)) {
    SSL_CTX_set_options(server_ctx, SSL_OP_NO_TLSv1_3);
    SSL_CTX_set_options(client_ctx, SSL_OP_NO_TLSv1_3);
  }
#endif

  res = SSL_CTX_use_certificate_chain_file(server_ctx, ircd_tls_certfile);
  if (res != 1)
  {
    ssl_log_error("unable to load certificate file");
    SSL_CTX_free(server_ctx);
    return 3;
  }

  res = SSL_CTX_use_certificate_chain_file(client_ctx, ircd_tls_certfile);
  if (res != 1)
  {
    ssl_log_error("unable to load certificate file");
    SSL_CTX_free(client_ctx);
    return 3;
  }

  res = SSL_CTX_use_PrivateKey_file(server_ctx, ircd_tls_keyfile, SSL_FILETYPE_PEM);
  if (res != 1)
  {
    ssl_log_error("unable to load private key");
    SSL_CTX_free(server_ctx);
    return 4;
  }

  res = SSL_CTX_use_PrivateKey_file(client_ctx, ircd_tls_keyfile, SSL_FILETYPE_PEM);
  if (res != 1)
  {
    ssl_log_error("unable to load private key");
    SSL_CTX_free(client_ctx);
    return 4;
  }

  res = SSL_CTX_check_private_key(server_ctx);
  if (res != 1)
  {
    ssl_log_error("private key did not check out");
    SSL_CTX_free(server_ctx);
    return 5;
  }

  res = SSL_CTX_check_private_key(client_ctx);
  if (res != 1)
  {
    ssl_log_error("private key did not check out");
    SSL_CTX_free(client_ctx);
    return 5;
  }

  ssl_set_ciphers(server_ctx, NULL, feature_str(FEAT_TLS_CIPHERS));
  ssl_set_ciphers(client_ctx, NULL, feature_str(FEAT_TLS_CIPHERS));

  str = feature_str(FEAT_TLS_CACERTFILE);
  s_2 = feature_str(FEAT_TLS_CACERTDIR);
  if (!EmptyString(str) || !EmptyString(s_2))
  {
    res = SSL_CTX_load_verify_locations(server_ctx, str, s_2);
    if (res != 1)
    {
      ssl_log_error("using TLS_CACERTFILE/TLS_CACERTDIR failed");
      /* but keep going */
    }

    res = SSL_CTX_load_verify_locations(client_ctx, str, s_2);
    if (res != 1)
    {
      ssl_log_error("using TLS_CACERTFILE/TLS_CACERTDIR failed");
      /* but keep going */
    }
  }

done:
  if (base_server_ctx)
    SSL_CTX_free(base_server_ctx);
  if (base_client_ctx)
    SSL_CTX_free(base_client_ctx);
  base_server_ctx = server_ctx;
  base_client_ctx = client_ctx;
  return 0;
}

void ircd_tls_close(void *ctx, const char *message)
{
  /* TODO: handle blocked I/O for shutdown */
  SSL_set_shutdown(ctx, SSL_RECEIVED_SHUTDOWN);
  if (SSL_shutdown(ctx) == 0)
    SSL_shutdown(ctx);
  SSL_free(ctx);
}

static void ssl_set_fd(SSL *tls, int fd)
{
  SSL_set_fd(tls, fd);

  BIO_set_nbio(SSL_get_rbio(tls), 1);

  BIO_set_nbio(SSL_get_wbio(tls), 1);
}

void *ircd_tls_accept(struct Listener *listener, int fd)
{
  SSL *tls;

  tls = SSL_new(base_server_ctx);
  if (!tls)
  {
    ssl_log_error("unable to create SSL session");
    return NULL;
  }

  if (listener->tls_ciphers)
    ssl_set_ciphers(NULL, tls, listener->tls_ciphers);

  ssl_set_fd(tls, fd);

  SSL_set_accept_state(tls);

  return tls;
}

void *ircd_tls_connect(struct ConfItem *aconf, int fd)
{
  SSL *tls;

  tls = SSL_new(base_client_ctx);
  if (!tls)
  {
    ssl_log_error("unable to create SSL session");
    return NULL;
  }

  if (aconf->tls_ciphers)
    ssl_set_ciphers(NULL, tls, aconf->tls_ciphers);

  ssl_set_fd(tls, fd);

  SSL_set_connect_state(tls);

  /* TODO: This is a gross fix, but the handshake has to be retried. */
  int cnt_retry = 0;
  while (SSL_connect(tls) != 1)  // Perform the handshake
  {
    cnt_retry++;
    ssl_log_error("SSL_connect() failed");
    if (cnt_retry > 1000) {
      SSL_free(tls);
      return NULL;
    }
  }

  /* TODO: Should we call ircd_tls_fingerprint() here? And should we be checking the fingerpint of our peer? */
  return tls;
}

static int ssl_handle_error(struct Client *cptr, SSL *tls, int res, int orig_errno)
{
  int err = SSL_get_error(tls, res);
  switch (err)
  {
  case SSL_ERROR_WANT_READ:
    socket_events(&cli_socket(cptr), SOCK_EVENT_READABLE);
    return 0;
  case SSL_ERROR_WANT_WRITE:
    socket_events(&cli_socket(cptr), SOCK_EVENT_WRITABLE);
    return 0;
  case SSL_ERROR_SYSCALL:
    if (orig_errno == EINTR || orig_errno == EAGAIN || orig_errno == EWOULDBLOCK)
      return 0;
    break;
  default:
    /* Fatal error (or EOF). */
    SSL_set_shutdown(tls, SSL_RECEIVED_SHUTDOWN);
    SSL_set_quiet_shutdown(tls, 1);
    SSL_shutdown(tls);
    break;
  }
  SSL_free(tls);
  s_tls(&cli_socket(cptr)) = NULL;
  return -1;
}

int ircd_tls_negotiate(struct Client *cptr)
{
  SSL *tls;
  int res;

  tls = s_tls(&cli_socket(cptr));
  if (!tls)
    return 1;

  res = SSL_accept(tls);
  if (res == 1)
  {
    ClearNegotiatingTLS(cptr);
    char tls_fp[65];
    ircd_tls_fingerprint(tls, tls_fp);
    strcpy(cli_tls_fingerprint(cptr), tls_fp);
  }
  else
  {
    int orig_errno = errno;
    /* Handshake in progress. */
    res = (ssl_handle_error(cptr, tls, res, orig_errno) < 0) ? -1 : 0;
  }

  return res;
}

IOResult ircd_tls_recv(struct Client *cptr, char *buf,
                       unsigned int length, unsigned int *count_out)
{
  SSL *tls;
  int res, orig_errno;

  tls = s_tls(&cli_socket(cptr));
  if (!tls)
    return IO_FAILURE;

  res = SSL_read(tls, buf, length);
  if (res > 0)
  {
    *count_out = res;
    return IO_SUCCESS;
  }

  orig_errno = errno;
  *count_out = 0;

  return (ssl_handle_error(cptr, tls, res, orig_errno) < 0) ? IO_FAILURE : IO_BLOCKED;
}

IOResult ircd_tls_sendv(struct Client *cptr, struct MsgQ *buf,
                        unsigned int *count_in,
                        unsigned int *count_out)
{
  struct iovec iov[512];
  SSL *tls;
  struct Connection *con;
  IOResult result = IO_BLOCKED;
  int ii, count, res, orig_errno;

  con = cli_connect(cptr);
  tls = s_tls(&con_socket(con));
  if (!tls)
    return IO_FAILURE;

  *count_out = 0;

  // Handling retransmission if needed
  if (con->con_rexmit)
  {
    res = SSL_write(tls, con->con_rexmit, con->con_rexmit_len);
    if (res <= 0)
    {
      orig_errno = errno;
      if (errno == EAGAIN || errno == EWOULDBLOCK)
      {
        Debug((DEBUG_TLS, "ircd_tls_sendv: SSL write blocked, retrying send for retransmission."));
        cli_connect(cptr)->con_rexmit = con->con_rexmit;  // Keep the pointer for retry
        cli_connect(cptr)->con_rexmit_len = con->con_rexmit_len;
        return IO_BLOCKED;  // Mark this operation as blocked and return
      }

      // If the error is fatal (e.g., connection reset or closed), handle accordingly
      Debug((DEBUG_TLS, "ircd_tls_sendv: SSL write failed (res=-1, errno=%d).", errno));
      if (ssl_handle_error(cptr, tls, res, orig_errno) < 0)
        return IO_FAILURE;
    }

    // Message has been sent successfully, so excise it
    msgq_excise(buf, con->con_rexmit, con->con_rexmit_len);
    con->con_rexmit_len = 0;
    con->con_rexmit = NULL;
    result = IO_SUCCESS;

    // Debug message to confirm retransmission completion
    Debug((DEBUG_TLS, "ircd_tls_sendv: Successfully sent retransmission of %d bytes.", con->con_rexmit_len));
  }

  // Process remaining messages in the queue
  count = msgq_mapiov(buf, iov, sizeof(iov) / sizeof(iov[0]), count_in);
  for (ii = 0; ii < count; ++ii)
  {
    res = SSL_write(tls, iov[ii].iov_base, iov[ii].iov_len);
    if (res > 0)
    {
      *count_out += res;
      result = IO_SUCCESS;

      // Debug message to confirm that the message was successfully sent
      Debug((DEBUG_TLS, "ircd_tls_sendv: Successfully sent %d bytes from message %d.", res, ii));

      continue;
    }

    // If the write was blocked, handle the blocking situation
    if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
    {
      Debug((DEBUG_TLS, "ircd_tls_sendv: SSL write blocked, retrying send for message %d.", ii));
      cli_connect(cptr)->con_rexmit = iov[ii].iov_base;  // Store the pointer for retransmission
      cli_connect(cptr)->con_rexmit_len = iov[ii].iov_len;
      return IO_BLOCKED;  // Mark this operation as blocked and return
    }

    // Handle SSL errors
    orig_errno = errno;
    if (ssl_handle_error(cptr, tls, res, orig_errno) < 0)
      return IO_FAILURE;

    // If the operation failed, set the retransmit pointer
    cli_connect(cptr)->con_rexmit = iov[ii].iov_base;
    cli_connect(cptr)->con_rexmit_len = iov[ii].iov_len;
    break;
  }

  // Debug message to confirm the overall operation completion
  if (*count_out == *count_in)
  {
    Debug((DEBUG_TLS, "ircd_tls_sendv: All messages sent successfully. Total bytes sent: %d.", *count_out));
  }
  else
  {
    Debug((DEBUG_ERROR, "ircd_tls_sendv: Not all messages were sent. Total bytes sent: %d, expected: %d.", *count_out, *count_in));
  }

  return result;
}
