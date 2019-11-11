#ifndef CRYPTO_H
#define CRYPTO_H

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/rsa.h>

// Internal
#include <core/codes.h>
#include <core/server.h>

ALPACA_STATUS alpacacore_init_TLS(allu_server_t *serv);

WOLFSSL* alpacacore_wrap_sock(allu_server_t *serv, uint16_t cli_sock);

ALPACA_STATUS alpacacore_gen_hash(char* buf);







#endif