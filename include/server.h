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
#include <nscodes.h>



/*
 * Defines server information
 */
typedef struct nss {

    unsigned short sock;
    struct sockaddr_in *serv_addr;
    WOLFSSL_CTX *tls_ctx;

} NS_server_t;


/* Server macro's */
#define REUSEADDR(sock) \
    (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &(int){1}, sizeof(uint32_t)))

#define BIND(sock, addr) \
    (bind(sock, (struct sockaddr*)addr, sizeof(struct sockaddr_in)))

#define LISTEN(sock, lcnt) \
    (lcnt>0 ? listen(sock, lcnt) : listen(sock, 10))

#define FILLSOCKADDR(serv, port) \
    NS_server_t *temp = (NS_server_t*)serv;         \
    temp->serv_addr->sin_family = AF_INET;          \
    temp->serv_addr->sin_addr.s_addr = INADDR_ANY;  \
    temp->serv_addr->sin_port = BEU16(port);        \
 



 /* Func */
NS_STATUS NS_server_run(uint16_t port);
void NS_server_clean();


#endif