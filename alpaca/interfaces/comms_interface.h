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

/* 
 * AlpacaComms protocol values
 */ 
#define ALPACACOMMS_PROTO_TLS12 1
#define ALPACACOMMS_PROTO_TLS13 2
#define ALPACACOMMS_PROTO_UDP   4
#define ALPACACOMMS_PROTO_SSH   8
/*
 * AlpacaComms type values
 */
#define ALPACACOMMS_TYPE_CLIENT 16
#define ALPACACOMMS_TYPE_SERVER 32
/* 
 * AlpacaComms Status values
 */
#define ALPACACOMMS_STATUS_NOTCONN 0
#define ALPACACOMMS_STATUS_CONN    1
#define ALPACACOMMS_STATUS_TLSCONN 2
/*
 * AlpacaComms limit values
 */
#define ALPACACOMMS_MAX_BUFF (1024*64)
/*
 * Bit flags and helpers
 */
#define KEEP_COMMS_PROTO 0x000F
#define KEEP_COMMS_TYPE  0x0030

#define GET_COMMS_PROTO(flags) ((KEEP_COMMS_PROTO & flags))
#define GET_COMMS_TYPE(flags) ((KEEP_COMMS_TYPE & flags))

/**
 * @struct AlpacaLunch_CommsCtx
 */
typedef struct AlpacaLunch_CommsCtx {

	Alpaca_sock_t* AlpacaSock;

	ALPACA_STATUS (*connect)(Alpaca_sock_t* sockctx);
	ALPACA_STATUS (*accept) (Alpaca_sock_t* sockctx, int16_t fd);	
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
ALPACA_STATUS AlpacaComms_recv	 (Alpaca_commsCtx_t** ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_send	 (Alpaca_commsCtx_t** ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_close  (Alpaca_commsCtx_t** ctx);

#endif