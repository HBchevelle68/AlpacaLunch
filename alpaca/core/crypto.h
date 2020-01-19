#ifndef CRYPTO_H
#define CRYPTO_H

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/rsa.h>

/*
 * Internal headers
 */ 
#include <core/codes.h>
#include <core/server.h>


ALPACA_STATUS alpacacore_gen_hash(char* buf);


#endif