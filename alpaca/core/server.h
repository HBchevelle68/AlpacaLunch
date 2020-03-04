#ifndef SERVER_H
#define SERVER_H

// Standard
#include <stdint.h>  
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/socket.h> 

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>


// Internal
#include <core/codes.h>



 /* Funcs */
ALPACA_STATUS AlpacaCore_serverRunST(uint16_t port, uint32_t listen_count);
void AlpacaCore_serverCleanST();


#endif