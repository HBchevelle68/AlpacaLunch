#ifndef SOCK_H
#define SOCK_H

#include <sys/socket.h>

#include <wolfssl/ssl.h>

typedef struct AlpacaLunch_Socket{

	int 	 		   fd;
	uint8_t  		   type;
	struct sockaddr_in peer;
	WOLFSSL* 		   ssl;

} Alpaca_sock_t;




ALPACA_STATUS AlpacaSock_create(void** ctx);
ALPACA_STATUS AlpacaSock_close(void** ctx);


int32_t AlpacaSock_setNonBlocking(int fd);


#endif