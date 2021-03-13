#ifndef WOLF_H
#define WOLF_H

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

#include <interfaces/comms_interface.h>

/**
 *  @brief Initializes the global WolfSSL contexts. These wolf contexts
 *         can be found in wolf.c. contexts are used to generate new
 *         ssl objects for client or server objects
 *  
 *  @param version version of wolfssl comms  
 *  
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaWolf_init(uint16_t version);

/**
 *  @brief 
 * 
 *  @param 
 * 
 *  @return ALPACA_STATUS 
 */
ALPACA_STATUS AlpacaWolf_cleanUp(void);

/**
 *  @brief Create a underlying SSL object of the type specified
 *  
 *  @param 
 *  @param 
 * 
 *  @return ALPACA_STATUS
 */
ALPACA_STATUS AlpacaWolf_createSSL(Alpaca_commsCtx_t* ctx, uint16_t flags);

/**
 *  @brief 
 * 
 *  @param 
 * 
 *  @return ALPACA_STATUS 
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