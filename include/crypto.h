#ifndef CRYPTO_H
#define CRYPTO_H

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/rsa.h>

#include <nscodes.h>
#include <server.h>

NS_STATUS NS_init_TLS(NS_server_t *serv);

WOLFSSL* NS_gen_local_TLS(NS_server_t *serv, uint16_t cli_sock);

NS_STATUS NS_init_RSA(NS_server_t *serv);

NS_STATUS NS_gen_hash(char* buf);







#endif