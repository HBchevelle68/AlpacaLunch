#ifndef WOLF_H
#define WOLF_H

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

#include <interfaces/comms_interface.h>

/**
 *  @brief Initializes the global WolfSSL contexts. These wolf contexts
 *         can be found in wolf.c. Contexts are used to generate new
 *         ssl objects for client or server objects
 *  
 *  @param version bit flag version of wolfssl comms  
 *  
 *  @return ALPACA_STATUS of ALPACA_SUCCESS, 
 *          ALPACA_ERROR_BADPARAM, or ALAPCA_ERROR_WOLFINIT
 */
ALPACA_STATUS AlpacaWolf_init(uint16_t version);

/**
 *  @brief Performs global cleanup of process wolfSSL contexts
 *         found in wolf.c. Should only be called on full comms
 *         teardown
 * 
 *  @return ALPACA_SUCCESS
 */
ALPACA_STATUS AlpacaWolf_cleanUp(void);

/**
 *  @brief Create a underlying SSL object of the type specified
 *         using the global comms ctx's generated from init
 *  @param ctx ptr to comms Ctx obj
 * 
 *  @return ALPACA_STATUS of ALPACA_SUCCESS,
 *          ALPACA_ERROR_BADPARAM, ALPACA_ERROR_WOLFSSLCREATE,
 *          ALPACA_ERROR_UNSUPPORTED, or ALPACA_ERROR_UNKNOWN
 */
ALPACA_STATUS AlpacaWolf_createSSL(Alpaca_commsCtx_t* ctx);

/**
 *  @brief Performs TCP connection, upon success, will perform
 *         TLS handshake. On error returns.
 * 
 *  @param ctx ptr to comms Ctx obj
 * 
 *  @return ALPACA_STATUS of ALPACA_SUCCESS,
 *          ALPACA_ERROR_BADPARAM, ALPACA_ERROR_SOCKCREATE,
 *          ALPACA_ERROR_COMMSCONNECT, or ALPACA_ERROR_WOLFSSLFDSET
 */
ALPACA_STATUS AlpacaWolf_connect(Alpaca_commsCtx_t* ctx);

/**
 *  @brief 
 * 
 *  @param 
 *  @param
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_listen(Alpaca_commsCtx_t* ctx, uint16_t port);

/**
 *  @brief 
 * 
 *  @param 
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_accept(Alpaca_commsCtx_t* ctx);

/**
 *  @brief 
 * 
 *  @param 
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_send(Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);

/**
 *  @brief 
 * 
 *  @param 
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_recv(Alpaca_commsCtx_t* ctx, void* buf, size_t len, ssize_t* out);

/**
 *  @brief 
 * 
 *  @param 
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_close(Alpaca_commsCtx_t* ctx);


#endif