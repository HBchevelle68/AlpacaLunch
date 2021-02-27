#ifndef WOLF_H
#define WOLF_H

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

#include <interfaces/comms_interface.h>

ALPACA_STATUS AlpacaWolf_init(uint16_t version);
ALPACA_STATUS AlpacaWolf_cleanUp(void);

ALPACA_STATUS AlpacaWolf_createSSL(Alpaca_commsCtx_t* ctx, int16_t flags);

ALPACA_STATUS AlpacaWolf_connect(Alpaca_commsCtx_t* ctx);
ALPACA_STATUS AlpacaWolf_accept (Alpaca_commsCtx_t* ctx, int16_t fd);
ALPACA_STATUS AlpacaWolf_send   (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaWolf_recv   (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaWolf_close  (Alpaca_commsCtx_t* ctx);


#endif