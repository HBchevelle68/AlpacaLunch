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

 
#define COMMSBUFSIZE 1500

/*
 * Defines base AlapcaLunch comms information
 */
typedef struct AlpacaNetInfo {
	uint8_t padding[2];
	struct utsname  host_uname;	
	struct hostent* host_entry; 
	struct ifaddrs* interfaces;
} ALLU_net_info; 


/*
 * A connection in Alpaca
 */
typedef struct AlpacaComms_ConnCtx {
    int16_t  sock;
    WOLFSSL* ssl;
} ALPACA_connCtx_t;

/*
 * Comms module Interface
 */






// NetInfo
ALLU_net_info* AlpacaNetInfo_init(void);
void		   AlpacaNetInfo_clean(ALLU_net_info* allu_ni);
ALPACA_STATUS  AlpacaNetInfo_getUname(ALLU_net_info** allu_ni);
ALPACA_STATUS  AlpacaNetInfo_getHostEntry(ALLU_net_info* allu_ni);
ALPACA_STATUS  AlpacaNetInfo_getIfAddrs(ALLU_net_info** allu_ni);



#endif