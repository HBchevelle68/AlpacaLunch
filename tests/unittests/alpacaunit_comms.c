
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <unittests/alpaca_unit.h>

#include <core/logging.h>
#include <core/codes.h>
#include <interfaces/comms_interface.h>
#include <comms/wolf.h>
#include <interfaces/utility_interface.h>


#define UNITTEST_DEFAULT_PORT 54321
#define CERTFILE ".testcerts/cert.pem"
#define KEYFILE  ".testcerts/private.pem"

static uint32_t thrd_shutdown = 0;
//static uint32_t thrd_reset = 0;

//static pthread_mutex_t read_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t write_lock = PTHREAD_MUTEX_INITIALIZER;



int AlpacaUnit_comms_initSuite(void){
    return 0;
}

int AlpacaUnit_comms_cleanSuite(void){

    return AlpacaComms_cleanUp();
}

void AlpacaUnit_comms_base(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    Alpaca_commsCtx_t *temp_commsCTX = NULL;

    struct sockaddr_in temp_addrin;
    struct sockaddr_in *addrin_ptr = NULL;
    memset(&temp_addrin, 0, sizeof(struct sockaddr_in));
    
    result = AlpacaComms_init(ALPACA_COMMSPROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);

    /*
     * Init TLS 1.2
     * Verify all layers of initialization is as expected
     */
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(temp_commsCTX);

    CU_ASSERT_PTR_NOT_NULL(temp_commsCTX->AlpacaSock);
    CU_ASSERT(temp_commsCTX->AlpacaSock->fd > 2);
    CU_ASSERT_EQUAL(temp_commsCTX->AlpacaSock->type, SOCK_STREAM);

    addrin_ptr = &(temp_commsCTX->AlpacaSock->peer);
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_addr), &temp_addrin.sin_addr, sizeof(temp_addrin.sin_addr)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_family), &temp_addrin.sin_family, sizeof(temp_addrin.sin_family)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_port), &temp_addrin.sin_port, sizeof(temp_addrin.sin_port)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_zero), &temp_addrin.sin_zero, sizeof(temp_addrin.sin_zero)));
    CU_ASSERT_PTR_NOT_NULL(temp_commsCTX->AlpacaSock->ssl);
    
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->connect, AlpacaWolf_connect);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->read, AlpacaWolf_recv);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->write, AlpacaWolf_send);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->close, AlpacaWolf_close);

    CU_ASSERT_EQUAL(temp_commsCTX->status, ALPACA_COMMSSTATUS_NOTCONN);
    result = AlpacaComms_destroyCtx(&temp_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(temp_commsCTX);    


    /*
     * Error cases
     * Verify expected errors are caught and handled properly
     */

    /* 
     * Attempt to initialize ctx with missing required flags && invalid flags
     */
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSTYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSPROTO_UDP);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    /* 
     * Attempt to initialize obj already initialized 
     */
    // Proper
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(temp_commsCTX);

    // Improper
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    // Clean
    result = AlpacaComms_destroyCtx(&temp_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    /* 
     * Attempt to initialize obj with unsupported comms type 
     */
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSPROTO_TLS13 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSPROTO_UDP | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSPROTO_SSH | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSPROTO_SSH | ALPACA_COMMSTYPE_CLIENT | ALPACA_COMMSTYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, 99);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNKNOWN);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, -99);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNKNOWN);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    /*
     * Verify Destroy case when unallocated obj passed
     */ 
    result = AlpacaComms_destroyCtx(&temp_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_cleanUp();
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);

    return;
}

static
void* server_thread(void* args){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    Alpaca_commsCtx_t *server_commsCTX = NULL;
    ssize_t numread = 0;
    uint8_t buffer[1024];
    
    pthread_mutex_lock(&write_lock);

    result = AlpacaComms_initCtx(&server_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_SERVER);
    if(result){
        LOGERROR("Error in server_thread during init...\n");
        goto exit;
    }

    /*
     * Set up server 
     */
    result = AlpacaComms_listen(&server_commsCTX, UNITTEST_DEFAULT_PORT);
    if(result){
        LOGERROR("Error in server_thread during listen...\n");
        goto exit;
    }

    LOGDEBUG("READING...\n");
    while(!thrd_shutdown){
        memset(&buffer, 0, sizeof(buffer));
        pthread_mutex_unlock(&write_lock);
        result = AlpacaComms_read(&server_commsCTX, &buffer, sizeof(buffer), &numread);
        LOGDEBUG("Buffer: %s\n", buffer);
    }



exit:
    result = AlpacaComms_destroyCtx(&server_commsCTX);
    return NULL;

}

void AlpacaUnit_comms_connect(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;

    Alpaca_commsCtx_t *client_commsCTX = NULL;
    struct sockaddr_in temp_addrin;
    struct sockaddr_in *addrin_ptr = NULL;

    pthread_t server;
    int ret = 0;
    


    memset(&temp_addrin, 0, sizeof(struct sockaddr_in));

    result = AlpacaComms_init(ALPACA_COMMSPROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);

    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);
    

    /*
     * Init TLS 1.2 CLIENT
     * Initiate a VALID connection
     * Verify results, then close
     */
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);


    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(client_commsCTX->status, ALPACA_COMMSSTATUS_TLSCONN);

    AlpacaUtilities_mSleep(1000);

    result = AlpacaComms_close(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    CU_ASSERT_PTR_NULL(client_commsCTX->AlpacaSock->ssl);

    addrin_ptr = &(client_commsCTX->AlpacaSock->peer);
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_addr), &temp_addrin.sin_addr, sizeof(temp_addrin.sin_addr)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_family), &temp_addrin.sin_family, sizeof(temp_addrin.sin_family)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_port), &temp_addrin.sin_port, sizeof(temp_addrin.sin_port)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_zero), &temp_addrin.sin_zero, sizeof(temp_addrin.sin_zero)));

    CU_ASSERT_EQUAL(client_commsCTX->AlpacaSock->fd, -1);
    CU_ASSERT_EQUAL(client_commsCTX->AlpacaSock->type, 0);

    thrd_shutdown = 1;
    pthread_join(server,NULL);


    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    /* Invalid ctx pointer passed */
    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    
    /* 
     * Invalid ctx pointer passed 
     * invalid IP passed
     */
    result = AlpacaComms_connect(&client_commsCTX, "", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);


    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);


    result = AlpacaComms_cleanUp();
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    return;
}

/*




*/