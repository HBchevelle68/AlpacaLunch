#ifndef SERVER_H
#define SERVER_H

// Standard
#include <stdint.h>  
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/socket.h> 

// Internal
#include <nscodes.h>
#include <crypto.h>


/*
 * Defines server information
 */
typedef struct nss {

    unsigned short sock;
    struct sockaddr_in *serv_addr;
    WOLFSSL_CTX *ctx;

} NS_server_t;


/* Server macro's */
#define REUSEADDR(sock) \
    (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &(int){1}, sizeof(uint32_t)))

#define BIND(sock, addr) \
    (bind(sock, (struct sockaddr*)addr, sizeof(struct sockaddr_in)))

#define LISTEN(sock, lcnt) \
    (lcnt>0 ? listen(sock, lcnt) : listen(sock, 10))



 /* Func */
NS_STATUS NS_server_run(uint16_t port);
void NS_server_clean();


#endif