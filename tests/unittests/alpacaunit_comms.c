
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <unittests/alpaca_unit.h>

#include <core/logging.h>
#include <core/codes.h>
#include <interfaces/comms_interface.h>
#include <comms/wolf.h>

Alpaca_commsCtx_t *testServComms;
Alpaca_commsCtx_t *testClientComms;


static pthread_mutex_t g_lock;
static uint8_t *g_buffer;




void unit_server(void* args)__attribute__((unused));

int AlpacaUnit_comms_initSuite(void){
    ALPACA_STATUS result = ALPACA_SUCCESS;

    /*
     * Global init comms 
     * Keep in mind as it sits 1/29/2021 this init's
     * the process level comms object
     */
    if((result = AlpacaComms_init(ALPACA_COMMSTYPE_TLS12)) != ALPACA_SUCCESS) {
        goto exit;
    }

    /*
     * Now init the static local server comms
     */
    if((result = AlpacaComms_initCtx(&testServComms, ALPACA_COMMSTYPE_TLS12)) != ALPACA_SUCCESS) {
        goto exit;
    }

    // Init mutex
    if(pthread_mutex_init(&g_lock, NULL) != 0){
        result = ALPACA_FAILURE;
        goto exit;
    }

    /*
     * Init server buffer to page size
     * This will be the global buffer to check
     * data was sent recv'd properly 
     */  
    g_buffer = calloc(FOUR_KB, sizeof(uint8_t));
    if(!g_buffer){
        result = ALPACA_FAILURE;
    }



exit:
    return result;
}

int AlpacaUnit_comms_cleanSuite(void){

    if(NULL != testServComms){
        AlpacaComms_destroyCtx(&testServComms);
        testServComms = NULL;
    }
    if(NULL != testClientComms){
        AlpacaComms_destroyCtx(&testClientComms);
        testClientComms = NULL;
    }

    pthread_mutex_destroy(&g_lock);

    if(NULL != g_buffer) {
        free(g_buffer);
    }
    
    return AlpacaComms_cleanUp();;
}

void AlpacaUnit_comms_base(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    Alpaca_commsCtx_t *temp_commsCTX = NULL;

    struct sockaddr_in temp_addrin;
    struct sockaddr_in *addrin_ptr = NULL;
    memset(&temp_addrin, 0, sizeof(struct sockaddr_in));
    
    
    /*
     * Init TLS 1.2
     * Verify all layers of initialization is as expected
     */
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSTYPE_TLS12);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(temp_commsCTX);

    CU_ASSERT_PTR_NOT_NULL(temp_commsCTX->AlpacaSock);
    CU_ASSERT(temp_commsCTX->AlpacaSock->fd > 2);
    CU_ASSERT_EQUAL(temp_commsCTX->AlpacaSock->type, SOCK_STREAM);

    addrin_ptr = &(temp_commsCTX->AlpacaSock->peer);
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_addr), &temp_addrin.sin_addr, sizeof(temp_addrin.sin_addr)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_family), &temp_addrin.sin_family, sizeof(temp_addrin.sin_family)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_port), &temp_addrin.sin_port, sizeof(temp_addrin.sin_port)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_zero), &temp_addrin.sin_zero, sizeof(temp_addrin.sin_zero)));
    CU_ASSERT_PTR_NULL(temp_commsCTX->AlpacaSock->ssl);
    
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->connect, AlpacaWolf_connect);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->read, AlpacaWolf_recv);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->write, AlpacaWolf_send);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->close, AlpacaWolf_close);

    CU_ASSERT_EQUAL(temp_commsCTX->status, ALPACA_COMMSSTATUS_NOTCONN);

    result = AlpacaComms_destroyCtx(&temp_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    /*
     * Verify proper failures cases 
     */
    // Attempt to initialize obj already initialized
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSTYPE_TLS12);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(temp_commsCTX);
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSTYPE_TLS12);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    result = AlpacaComms_destroyCtx(&temp_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    // Attempt to initialize obj with unsupported comms type
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSTYPE_TLS13);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSTYPE_UDP);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACA_COMMSTYPE_SSH);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, 99);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNKNOWN);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, -99);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNKNOWN);
    CU_ASSERT_PTR_NULL(temp_commsCTX);


    // Verify Destroy case when unallocated obj passed
    result = AlpacaComms_destroyCtx(&temp_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    


    return;
}