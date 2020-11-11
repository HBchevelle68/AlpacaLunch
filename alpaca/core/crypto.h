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

ALPACA_STATUS MyEncrypt(uint8_t* plain, uint32_t plainLength, uint8_t* cipher);
ALPACA_STATUS MyDecrypt(uint8_t* plain, uint32_t plainLength, uint8_t* cipher);


#endif