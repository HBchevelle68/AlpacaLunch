#ifndef WOLF_H
#define WOLF_H

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

ALPACA_STATUS AlpacaWolf_init(uint16_t version);
ALPACA_STATUS AlpacaWolf_cleanUp(void);

ALPACA_STATUS AlpacaWolf_createSSL(Alpaca_sock_t* alpacasock, uint16_t flags);

ALPACA_STATUS AlpacaWolf_connect(Alpaca_sock_t* alpacasock);
ALPACA_STATUS AlpacaWolf_accept (Alpaca_sock_t* alpacasock);
ALPACA_STATUS AlpacaWolf_send   (WOLFSSL* sslCtx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaWolf_recv   (WOLFSSL* sslCtx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaWolf_close  (WOLFSSL** sslCtx);


#endif