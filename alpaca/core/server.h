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
ALPACA_STATUS alpacacore_server_run(uint16_t port);
void alpacacore_server_clean();


#endif