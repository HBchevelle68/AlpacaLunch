#ifndef SOCK_H
#define SOCK_H

#include <sys/socket.h>

typedef AlpacaLunch_Socket{

	int 	 sock; // system returned fd
	uint8_t  type; // socket type
	wolfSSL* ctx;  	

} Alpaca_sock_t;



ALPACA_STATUS AlpacaSock_createSocket(void);


#endif