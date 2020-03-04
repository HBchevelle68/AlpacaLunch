/*
 * AlpacaLunch Comms interface 
 *
 */
#ifndef COMMS_INTERFACE_H
#define COMMS_INTERFACE_H
#define _GNU_SOURCE

#include <stdint.h>  
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/socket.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <ifaddrs.h>

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

typedef struct AlpacaNetInfo {
	struct utsname  host_uname;
	struct hostent* host_entry; 
	struct ifaddrs* interfaces;
} ALLU_net_info; 

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
 */
// COMMS
extern ALPACA_STATUS  AlpacaComms_initComms(void);
extern ALPACA_STATUS  AlpacaComms_cleanComms(void);

// SOCK
extern ALPACA_STATUS  AlpacaSock_createServSock(ALLU_server_ctx** serverCtx);

// NetInfo
extern ALLU_net_info* AlpacaNetInfo_init(void);
extern 			void  AlpacaNetInfo_clean(ALLU_net_info* allu_ni);
extern ALPACA_STATUS  AlpacaNetInfo_getUname(ALLU_net_info* allu_ni);
extern ALPACA_STATUS  AlpacaNetInfo_getHostEntry(ALLU_net_info* allu_ni);
extern ALPACA_STATUS  AlpacaNetInfo_getIfAddrs(ALLU_net_info** allu_ni);



#endif