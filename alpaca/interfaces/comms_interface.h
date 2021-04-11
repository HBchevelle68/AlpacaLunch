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

#define MAX_RETRIES  5
#define THREE_SECONDS_MILLI (3*1000)


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

#define ALPACACOMMS_INIT_CALLBACK 0
#define ALPACACOMMS_INIT_LISTEN   1


/*
 * Bit flags and helpers
 */
#define KEEP_COMMS_PROTO 0x000F
#define KEEP_COMMS_TYPE  0x0030
#define GET_COMMS_PROTO(flags) (KEEP_COMMS_PROTO & flags)
#define GET_COMMS_TYPE(flags) (KEEP_COMMS_TYPE & flags)


typedef struct AlpacaLunch_CommsCtx Alpaca_commsCtx_t;

struct AlpacaLunch_CommsCtx {

	int fd;
	void* protoCtx;
	uint8_t status;
	uint16_t flags;
	struct sockaddr_in peer;

	ALPACA_STATUS (*connect)(Alpaca_commsCtx_t* ctx);
	ALPACA_STATUS (*listen) (Alpaca_commsCtx_t* ctx, uint16_t port);
	ALPACA_STATUS (*accept) (Alpaca_commsCtx_t* ctx);
	ALPACA_STATUS (*read)   (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
	ALPACA_STATUS (*write)  (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
	ALPACA_STATUS (*close)  (Alpaca_commsCtx_t* ctx);
};
#define COMMS_CTX_SIZE (sizeof(struct AlpacaLunch_CommsCtx))

// Core comms, links to controller
extern Alpaca_commsCtx_t *coreComms;


typedef struct __attribute__((packed)) AlpacaLunch_Protocol_Header {
	uint32_t alpacaID;
	uint8_t  flags;
	uint8_t  reserved; 
	uint16_t cmdNum;
	uint16_t cmdID;
	uint32_t bodySize;
} Alpaca_protoHdr_t;

#define COMMS_HDR_SIZE (sizeof(struct AlpacaLunch_Protocol_Header))
#define COMMS_MAX_BODY ((1024*64) - COMMS_HDR_SIZE)
#define COMMS_HDR_FLAG_TSK 1
#define COMMS_HDR_FLAG_RSP 2
#define COMMS_HDR_FLAG_FWD 4
#define COMMS_HDR_FLAG_OOB 8


#define	NTOH_COMMS_HDR(hdr) \
	do {\
		hdr.alpacaID = ntohl(hdr.alpacaID);\
		hdr.cmdNum = ntohs(hdr.cmdNum);\
		hdr.cmdID = ntohs(hdr.cmdID);\
		hdr.bodySize = ntohl(hdr.bodySize);\
	} while(0)


#ifdef TALKATIVE_ALPACA
#define	DEBUG_COMMS_CTX(comms_ptr) \
	do {\
		char str[INET_ADDRSTRLEN];\
		if(comms_ptr) {\
			inet_ntop(AF_INET, &(comms_ptr->peer.sin_addr.s_addr), str, INET_ADDRSTRLEN);\
			printf("*** DEBUG Comms Ctx ***\n");\
			printf("Alpaca_protoHdr_t { \n");\
			printf("  ->fd = %d (%#x)\n",comms_ptr->fd,comms_ptr->fd);\
			printf("  ->protoCtx = %p\n",comms_ptr->protoCtx);\
			printf("  ->status = %u (%#x)\n",comms_ptr->status,comms_ptr->status);\
			printf("  ->flags = %u (%#x)\n",comms_ptr->flags,comms_ptr->flags);\
			printf("  ->peer {\n");\
			printf("    .sin_family = %u (%#x)\n",comms_ptr->peer.sin_family,comms_ptr->peer.sin_family);\
			printf("    .sin_port = %u (%#x)\n",ntohs(comms_ptr->peer.sin_port),ntohs(comms_ptr->peer.sin_port));\
			printf("    .sin_addr.s_addr = %s (%#x)\n",str,comms_ptr->peer.sin_addr.s_addr);\
			printf("  } \n");\
			printf("} \n");\
		}\
	} while(0)

#define	DEBUG_COMMS_HDR_PTR(hdr_ptr) \
	do {\
		if(hdr_ptr) {\
			printf("*** DEBUG Comms Header ***\n");\
			printf("Alpaca_protoHdr_t { \n");\
			printf("  ->alpacaID = %lu (%#x)\n",hdr_ptr->alpacaID,hdr_ptr->alpacaID);\
			printf("  ->flags = %u (%#x)\n",hdr_ptr->flags,hdr_ptr->flags);\
			printf("  ->reserved = %u (%#x)\n",hdr_ptr->resserved,hdr_ptr->resserved);\
			printf("  ->cmdNum = %u (%#x)\n",hdr_ptr->cmdNum,hdr_ptr->cmdNum);\
			printf("  ->cmdID = %u (%#x)\n",hdr_ptr->cmdID,hdr_ptr->cmdID);\
			printf("  ->bodySize = %lu (%#x)\n",hdr_ptr->bodySize,hdr_ptr->bodySize);\
			printf("} \n");\
		}\
	} while(0)

#define	DEBUG_COMMS_HDR(hdr) \
	do {\
		printf("*** DEBUG Comms Header ***\n");\
		printf("Alpaca_protoHdr_t { \n");\
		printf("  .alpacaID = %u (%#x)\n",hdr.alpacaID,hdr.alpacaID);\
		printf("  .flags = %u (%#x)\n",hdr.flags,hdr.flags);\
		printf("  .resserved = %u (%#x)\n",hdr.reserved,hdr.reserved);\
		printf("  .cmdNum = %u (%#x)\n",hdr.cmdNum,hdr.cmdNum);\
		printf("  .cmdID = %u (%#x)\n",hdr.cmdID,hdr.cmdID);\
		printf("  .bodySize = %u (%#x)\n",hdr.bodySize,hdr.bodySize);\
		printf("} \n");\
	} while(0)

#else
#define	DEBUG_COMMS_CTX(comms_ptr)
#define DEBUG_COMMS_HDR(hdr_ptr)
#define DEBUG_COMMS_HDR_PTR(hdr_ptr)
#endif



// Process networking Init/Cleanup
ALPACA_STATUS AlpacaComms_init	  (uint16_t flags);
ALPACA_STATUS AlpacaComms_cleanUp (void);

// Networking context creation/destruction
ALPACA_STATUS AlpacaComms_initCtx	(Alpaca_commsCtx_t** ctx, uint16_t flags);
ALPACA_STATUS AlpacaComms_destroyCtx(Alpaca_commsCtx_t** ctx);

// Network I/O
ALPACA_STATUS AlpacaComms_connect(Alpaca_commsCtx_t* ctx, char* ipstr, uint16_t port);
ALPACA_STATUS AlpacaComms_listen (Alpaca_commsCtx_t* ctx, uint16_t port);
ALPACA_STATUS AlpacaComms_recv	 (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_send	 (Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);
ALPACA_STATUS AlpacaComms_close  (Alpaca_commsCtx_t* ctx);


// Initial Behavior
ALPACA_STATUS AlpacaComms_initialCallback(Alpaca_commsCtx_t* ctx);
ALPACA_STATUS AlpacaComms_initialListen(Alpaca_commsCtx_t* ctx);

// Network Utils
int32_t AlpacaSock_setNonBlocking(int fd);


#endif