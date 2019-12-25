/*
 * AlpacaLunch Comms interface 
 *
 */
#ifndef COMMS_INTERFACE_H
#define COMMS_INTERFACE_H

#include <stdint.h>  
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/socket.h> 

/*
 * Wolfssl headers
 */
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

/*
 * Internal headers
 */
#include <core/codes.h>
#include <core/server.h>

/*
 * TO DO
 * This needs to be configurable at runtime
 */
#define SCERT "../certs/cert.pem"
#define PKEY "../certs/prvt.pem"
 
#define COMMSBUFSIZE 1500


/*
 * Defines base AlapcaLunch comms information
 */
typedef struct AlpacaCommsCtx {

    unsigned short sock;
    struct sockaddr_in *serv_addr;
    WOLFSSL_CTX *tls_ctx;

} ALLU_comms_ctx;


/* 
 * Comms macro's
 */
#define REUSEADDR(sock) \
    (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &(int){1}, sizeof(uint32_t)))

#define BIND(sock, addr) \
    (bind(sock, (struct sockaddr*)addr, sizeof(struct sockaddr_in)))

#define LISTEN(sock, lcnt) \
    (lcnt>0 ? listen(sock, lcnt) : listen(sock, 10))

#define FILLSOCKADDR(serv, port) \
    allu_server_t *temp = (allu_server_t*)serv;     \
    temp->serv_addr->sin_family = AF_INET;          \
    temp->serv_addr->sin_addr.s_addr = INADDR_ANY;  \
    temp->serv_addr->sin_port = BEU16(port);        \


/*
 * Comms module Interface
 *
 */

extern ALPACA_STATUS AlpacaComms_create_listen_sock(ALLU_comms_ctx *ctx, uint16_t port, uint32_t listen_count);

extern void  AlpacaComms_connection_handler(ALLU_comms_ctx *ctx, uint32_t cli_sock, struct sockaddr_in* cliaddr);

extern WOLFSSL* AlpacaComms_wrap_sock(ALLU_comms_ctx *ctx, uint16_t cli_sock);

extern ALPACA_STATUS AlpacaComms_init_TLS(ALLU_comms_ctx *ctx);

extern void AlpacaComms_clean_comms_ctx(ALLU_comms_ctx *ctx);

#endif