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
#include <fcntl.h>

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
 
#define COMMSBUFSIZE 1500


/*
 * Defines base AlapcaLunch comms information
 */
typedef struct AlpacaCommsServerCtx {

    unsigned short serverSock;
    struct sockaddr_in *servAddr;
    WOLFSSL_CTX *wolf_ctx;

} ALLU_server_ctx;


/* 
 * Comms macro's
 */
#define REUSEADDR(sock) \
    (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &(int){1}, sizeof(uint32_t)))

#define BIND(sock, addr) \
    (bind(sock, (struct sockaddr*)addr, sizeof(struct sockaddr_in)))

#define LISTEN(sock, lcnt) \
    (lcnt>0 ? listen(sock, lcnt) : listen(sock, 10))

/*
 * Comms module Interface
 *
 */


extern ALPACA_STATUS AlpacaComms_initComms(void);
extern ALPACA_STATUS AlpacaComms_cleanComms(void);

extern ALPACA_STATUS AlpacaComms_createServSock(ALLU_server_ctx** serverCtx);




//extern ALPACA_STATUS AlpacaComms_create_listen_sock(ALLU_comms_ctx *ctx, uint16_t port, uint32_t listen_count);

//extern void  AlpacaComms_connection_handler(ALLU_comms_ctx *ctx, uint32_t cli_sock, struct sockaddr_in* cliaddr);

//extern WOLFSSL* AlpacaComms_wrap_sock(ALLU_comms_ctx *ctx, uint16_t cli_sock);

//extern ALPACA_STATUS AlpacaComms_init_TLS(ALLU_comms_ctx *ctx);

//extern void AlpacaComms_clean_comms_ctx(ALLU_comms_ctx *ctx);

#endif