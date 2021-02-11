
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

static pthread_mutex_t read_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t write_lock = PTHREAD_MUTEX_INITIALIZER;
static uint8_t thrd_buffer[FOUR_KB];


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

    /*************************************************************************************
     * Init TLS 1.2
     * Verify all layers of initialization is as expected
     *************************************************************************************/
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


    /*************************************************************************************
     * Error cases
     * Verify expected errors are caught and handled properly
     *************************************************************************************/

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

    /************************************************************************************* 
     * Attempt to initialize obj already initialized 
     *************************************************************************************/
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

    /************************************************************************************* 
     * Attempt to initialize obj with unsupported comms type 
     *************************************************************************************/
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

    /*************************************************************************************
     * Verify Destroy case when unallocated obj passed
     *************************************************************************************/ 
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
    
    
    pthread_mutex_lock(&write_lock);

    result = AlpacaComms_initCtx(&server_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_SERVER);
    if(result){
        LOGERROR("Error in server_thread during init...\n");
        goto exit;
    }

    setsockopt(server_commsCTX->AlpacaSock->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    /*************************************************************************************
     * Set up server 
     *************************************************************************************/
    result = AlpacaComms_listen(&server_commsCTX, UNITTEST_DEFAULT_PORT);
    if(result){
        LOGERROR("Error in server_thread during listen...\n");
        goto exit;
    }


    
    while(!thrd_shutdown){
    //    
        pthread_mutex_unlock(&write_lock);
        //LOGDEBUG("THREAD RELEASED WRITE_LOCK\n");
        pthread_mutex_lock(&read_lock);
        //LOGDEBUG("THREAD ACQUIRED READ_LOCK\n");
        LOGDEBUG("*** READING ***\n");
        memset(&thrd_buffer, 0, sizeof(thrd_buffer));
        result = AlpacaComms_read(&server_commsCTX, &thrd_buffer, sizeof(thrd_buffer), &numread);
        pthread_mutex_unlock(&read_lock);
        //LOGDEBUG("THREAD RELEASED READ_LOCK\n");
        LOGDEBUG("*** WAITING ***\n");
        pthread_mutex_lock(&write_lock);
        
        //LOGDEBUG("THREAD ACQUIRED WRITE_LOCK\n");
    }



exit:
    pthread_mutex_unlock(&write_lock);
    pthread_mutex_unlock(&read_lock);
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
    
    /*************************************************************************************
     * Init TLS 1.2 CLIENT
     * Initiate a VALID connection
     * Verify results, then close
     *************************************************************************************/
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
    thrd_shutdown = 0;


    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    /* Invalid ctx pointer passed */
    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    
    /************************************************************************************* 
     * Invalid ctx pointer passed 
     * invalid IP passed
     *************************************************************************************/
    result = AlpacaComms_connect(&client_commsCTX, "", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);

    /************************************************************************************* 
     * valid ctx pointer passed 
     * Invalid IP passed
     * Invalid port
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(&client_commsCTX, "", 0);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    /************************************************************************************* 
     * valid ctx pointer passed 
     * valid IP passed
     * Invalid port
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", 0);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);


    /*************************************************************************************
     * Valid params but attempt to connect to non-listening port
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", 12345);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_COMMSCONNECT);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    /*************************************************************************************
     * Connect to listening service but with protocol mismatch
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", 22);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_WOLFSSLCONNECT);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);


    /*************************************************************************************
     * Double connect
     *************************************************************************************/

    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);

    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    CU_ASSERT_EQUAL(client_commsCTX->status, ALPACA_COMMSSTATUS_TLSCONN);
    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADSTATE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    CU_ASSERT_EQUAL(client_commsCTX->status, ALPACA_COMMSSTATUS_TLSCONN);

    AlpacaUtilities_mSleep(500);

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
    thrd_shutdown = 0;

    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);
    
    result = AlpacaComms_cleanUp();
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    return;
}

void AlpacaUnit_comms_send(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    Alpaca_commsCtx_t *client_commsCTX = NULL;
    pthread_t server;
    int ret = 0;
    ssize_t out = 0;
    unsigned char* rand_stream = NULL;
    char local_buffer[1024] = {0};
    memset(local_buffer, 0, sizeof(local_buffer));

    result = AlpacaComms_init(ALPACA_COMMSPROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);


    /*************************************************************************************
     * Baseline connect and send. 
     * Verify contents and send values
     *************************************************************************************/

    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);

    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);

    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(client_commsCTX->status, ALPACA_COMMSSTATUS_TLSCONN);

    pthread_mutex_lock(&write_lock);
    strcpy((char*)local_buffer,"WAZZZZZZUP!");
    result = AlpacaComms_write(&client_commsCTX, local_buffer, strlen((char*)local_buffer), &out);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(out, strlen((char*)local_buffer));

    // Grab read lock to ensure buffer is in a readable state
    pthread_mutex_lock(&read_lock);
    LOGINFO("local: %s thrd: %s\n", (char*)local_buffer, (char*)thrd_buffer);
    CU_ASSERT_NSTRING_EQUAL(thrd_buffer,local_buffer,strlen((char*)local_buffer));
    pthread_mutex_unlock(&read_lock);

    thrd_shutdown = 1;
    out = 0;
    pthread_mutex_unlock(&write_lock);
    

    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    pthread_join(server,NULL);
    thrd_shutdown = 0;

    /*************************************************************************************
     * Connect and send full random buffer 
     * Verify contents and send values
     *************************************************************************************/
    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);

    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);

    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(client_commsCTX->status, ALPACA_COMMSSTATUS_TLSCONN);

    pthread_mutex_lock(&write_lock);
    rand_stream = gen_rdm_bytestream(ONE_KB);
    result = AlpacaComms_write(&client_commsCTX, rand_stream, ONE_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(out, ONE_KB);

    // Grab read lock to ensure buffer is in a readable state
    pthread_mutex_lock(&read_lock);
    CU_ASSERT_NSTRING_EQUAL(thrd_buffer, rand_stream, ONE_KB);
    free(rand_stream);
    pthread_mutex_unlock(&read_lock);

    thrd_shutdown = 1;
    pthread_mutex_unlock(&write_lock);
    
    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    pthread_join(server,NULL);
    thrd_shutdown = 0;

    /*************************************************************************************
     * Connect and send MULTIPLE full random buffer 
     * Buffer MUST be greater than the standard MTU size
     * Verify contents and send values
     *************************************************************************************/
    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);

    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);

    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(client_commsCTX->status, ALPACA_COMMSSTATUS_TLSCONN);

    for(int i = 0; i < ONE_KB; i++){
        rand_stream = gen_rdm_bytestream(FOUR_KB);
        pthread_mutex_lock(&write_lock);
        LOGDEBUG("***** Send [%d] ***** \n", i+1);
        result = AlpacaComms_write(&client_commsCTX, rand_stream, FOUR_KB, &out);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(out, FOUR_KB);

        // Grab read lock to ensure buffer is in a readable state
        pthread_mutex_lock(&read_lock);
        CU_ASSERT_NSTRING_EQUAL(thrd_buffer, rand_stream, FOUR_KB);
        
        pthread_mutex_unlock(&write_lock);
        pthread_mutex_unlock(&read_lock);
        free(rand_stream);
        AlpacaUtilities_mSleep(10);
        if(i%64 == 0){printf(".");}
    }
    thrd_shutdown = 1;
    
    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    pthread_join(server,NULL);
    thrd_shutdown = 0;

    /*************************************************************************************
     * Connect then execute a series of sends that should fail gracefully
     * Data should be preserved
     * No data should be sent 
     *************************************************************************************/
    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);

    rand_stream = gen_rdm_bytestream(FOUR_KB);
    // First without initialization occuring
    // Bad ctx
    LOGDEBUG("HERE1\n");
    result = AlpacaComms_write(&client_commsCTX, rand_stream, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_EQUAL(out, 0);

    // Now init
    LOGDEBUG("HERE2\n");
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACA_COMMSPROTO_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);

    // Bad len
    LOGDEBUG("HERE3\n");
    result = AlpacaComms_write(&client_commsCTX, rand_stream, 0, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_EQUAL(out, 0);
    free(rand_stream);
    rand_stream = NULL;

    // Bad buff
    LOGDEBUG("HERE4\n");
    result = AlpacaComms_write(&client_commsCTX, rand_stream, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_EQUAL(out, 0);

    // Bad buff but with out var pre-set
    out = 123;
    LOGDEBUG("HERE5\n");
    result = AlpacaComms_write(&client_commsCTX, rand_stream, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_EQUAL(out, 0);

    // Now connect
    result = AlpacaComms_connect(&client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(client_commsCTX->status, ALPACA_COMMSSTATUS_TLSCONN);
    pthread_mutex_lock(&write_lock);
    rand_stream = gen_rdm_bytestream(FOUR_KB);
    
    // Final send
    result = AlpacaComms_write(&client_commsCTX, rand_stream, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(out, FOUR_KB);
    // Grab read lock to ensure buffer is in a readable state
    pthread_mutex_lock(&read_lock);
    CU_ASSERT_NSTRING_EQUAL(thrd_buffer, rand_stream, FOUR_KB);
    
    pthread_mutex_unlock(&write_lock);
    pthread_mutex_unlock(&read_lock);
    thrd_shutdown = 1;
    free(rand_stream);
    AlpacaUtilities_mSleep(100);

    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    pthread_join(server,NULL);
    thrd_shutdown = 0;

    result = AlpacaComms_cleanUp();
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    return;
}
