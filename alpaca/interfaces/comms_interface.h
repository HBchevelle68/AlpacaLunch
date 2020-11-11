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

#define ALPACA_COMMSTYPE_TLS12 0
#define ALPACA_COMMSTYPE_TLS13 1
#define ALPACA_COMMSTYPE_UDP   2
#define ALPACA_COMMSTYPE_SSH   3

 
typedef struct AlpacaLunch_CommsCtx {

	Alpaca_sock_t* AlpacaSock;

	ALPACA_STATUS (* connect)(void* ctx);
	ALPACA_STATUS (* read) 	 (void* ctx, void* buf, size_t len, ssize_t* out);
	ALPACA_STATUS (* write)	 (void* ctx, void* buf, size_t len, ssize_t* out);
	ALPACA_STATUS (* close)	 (void* ctx);

} Alpaca_commsCtx_t;

// Process networking Init/Cleanup
ALPACA_STATUS AlpacaComms_init	  (Alpaca_tlsVersion_t version);
ALPACA_STATUS AlpacaComms_cleanUp (void);

// Networking context creation/destruction
ALPACA_STATUS AlpacaComms_initCtx	(Alpaca_commsCtx_t** ctx, uint8_t type);
ALPACA_STATUS AlpacaComms_destroyCtx(Alpaca_commsCtx_t** ctx);

// Network I/O
ALPACA_STATUS AlpacaComms_connect (Alpaca_commsCtx_t** ctx);
ALPACA_STATUS AlpacaComms_read	  (Alpaca_commsCtx_t** ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_write	  (Alpaca_commsCtx_t** ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_close   (Alpaca_commsCtx_t** ctx);


#endif