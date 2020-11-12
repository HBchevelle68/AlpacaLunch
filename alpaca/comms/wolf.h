#ifndef WOLF_H
#define WOLF_H

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

ALPACA_STATUS AlpacaWolf_init(Alpaca_tlsVersion_t version);
ALPACA_STATUS AlpacaWolf_cleanUp(void);


ALPACA_STATUS AlpacaWolf_createClientSSL(Alpaca_sock_t* alpacasock , uint8_t type);


ALPACA_STATUS AlpacaWolf_connect(Alpaca_sock_t* alpacasock);
ALPACA_STATUS AlpacaWolf_send   (Alpaca_sock_t* alpacasock, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaWolf_recv   (Alpaca_sock_t* alpacasock, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaWolf_close  (Alpaca_sock_t* alpacasock);

//ALPACA_STATUS AlpacaWolf_tls12_send();
//ALPACA_STATUS AlpacaWolf_tls12_recv();


#endif