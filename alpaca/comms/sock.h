#ifndef SOCK_H
#define SOCK_H

#include <sys/socket.h>

#include <wolfssl/ssl.h>

/**
 * @struct AlpacaLunch_Socket (Alpaca_sock_t)
 * @brief  Alpaca_sock_t represent a socket to include
 * 		   the TCP/UDP sock and SSL object
 */
typedef struct AlpacaLunch_Socket{

	int 	 		   fd; /**< Some documentation for the member BoxStruct#fd. */
	uint8_t  		   type;
	struct sockaddr_in peer;
	WOLFSSL* 		   ssl;

} Alpaca_sock_t;




ALPACA_STATUS AlpacaSock_create (Alpaca_sock_t* alpacasock);
ALPACA_STATUS AlpacaSock_close  (Alpaca_sock_t* alpacasock);

int32_t AlpacaSock_setNonBlocking(int fd);


#endif