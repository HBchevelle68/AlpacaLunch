/*
 * AlpacaLunch Comms interface 
 *
 */
#ifndef COMMS_INTERFACE_H
#define COMMS_INTERFACE_H

#include <stdint.h>  
#include <stdlib.h> 
#include <sys/types.h>
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
#include <comms/sock.h>



typedef enum AlpacaLunch_TLSVersion{
	tls_1_2 = 0,
	tls_1_3 = 1
} Alpaca_tlsVersion_t;

/* Alpaca_commsCtx_t protocol values */ 
#define ALPACA_COMMSPROTO_TLS12 0
#define ALPACA_COMMSPROTO_TLS13 1
#define ALPACA_COMMSPROTO_UDP   2
#define ALPACA_COMMSPROTO_SSH   4

/* Alpaca_commsCtx_t type values */
#define ALPACA_COMMSTYPE_CLIENT 8
#define ALPACA_COMMSTYPE_SERVER 16

#define DISABLE_COMMS_TYPE 0x0007

/* Alpaca_commsCtx_t Status values */ 
#define ALPACA_COMMSSTATUS_NOTCONN 0
#define ALPACA_COMMSSTATUS_CONN    1
#define ALPACA_COMMSSTATUS_TLSCONN 2



/**
 * @struct AlpacaLunch_CommsCtx
 */
typedef struct AlpacaLunch_CommsCtx {

	Alpaca_sock_t* AlpacaSock;

	ALPACA_STATUS (*connect)(Alpaca_sock_t* ctx);
	ALPACA_STATUS (*accept) (Alpaca_sock_t* ctx);	
	ALPACA_STATUS (*read)   (WOLFSSL* ctx, void* buf, size_t len, ssize_t* out);
	ALPACA_STATUS (*write)  (WOLFSSL* ctx, void* buf, size_t len, ssize_t* out);
	ALPACA_STATUS (*close)  (WOLFSSL** ctx);

	uint8_t status;

} Alpaca_commsCtx_t;
extern Alpaca_commsCtx_t *coreComms;

// Process networking Init/Cleanup
ALPACA_STATUS AlpacaComms_init	  (uint16_t flags);
ALPACA_STATUS AlpacaComms_cleanUp (void);

// Networking context creation/destruction
ALPACA_STATUS AlpacaComms_initCtx	(Alpaca_commsCtx_t** ctx, uint16_t flags);
ALPACA_STATUS AlpacaComms_destroyCtx(Alpaca_commsCtx_t** ctx);

// Network I/O
ALPACA_STATUS AlpacaComms_connect(Alpaca_commsCtx_t** ctx, char* ipstr, uint16_t port);
ALPACA_STATUS AlpacaComms_listen (Alpaca_commsCtx_t** ctx, uint16_t port);
ALPACA_STATUS AlpacaComms_read	 (Alpaca_commsCtx_t** ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_write	 (Alpaca_commsCtx_t** ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_close  (Alpaca_commsCtx_t** ctx);

#endif