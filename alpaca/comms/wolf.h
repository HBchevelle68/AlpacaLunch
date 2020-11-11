#ifndef WOLF_H
#define WOLF_H

ALPACA_STATUS AlpacaWolf_init(Alpaca_tlsVersion_t v);
ALPACA_STATUS AlpacaWolf_cleanUp(void);


ALPACA_STATUS AlpacaWolf_create(WOLFSSL* sslobj ,uint8_t type);


//ALPACA_STATUS AlpacaWolf_tls12_send();
//ALPACA_STATUS AlpacaWolf_tls12_recv();


#endif