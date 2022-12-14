
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
static uint32_t ready = 0;

static pthread_mutex_t read_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t write_lock = PTHREAD_MUTEX_INITIALIZER;
static uint8_t thrd_serv_buffer[FOUR_KB];
static unsigned char* thrd_cli_buffer = NULL;

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
    
    result = AlpacaComms_init(ALPACACOMMS_PROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);

    /*************************************************************************************
     * Init TLS 1.2
     * Verify all layers of initialization is as expected
     *************************************************************************************/
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(temp_commsCTX);
    CU_ASSERT(temp_commsCTX->fd == 0);
    CU_ASSERT_PTR_NULL(temp_commsCTX->protoCtx);
    CU_ASSERT_EQUAL(temp_commsCTX->status, ALPACACOMMS_STATUS_NOTCONN);
    CU_ASSERT_EQUAL(GET_COMMS_PROTO(temp_commsCTX->flags), ALPACACOMMS_PROTO_TLS12);

    // Check peer structure
    addrin_ptr = &(temp_commsCTX->peer);
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_addr), &temp_addrin.sin_addr, sizeof(temp_addrin.sin_addr)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_family), &temp_addrin.sin_family, sizeof(temp_addrin.sin_family)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_port), &temp_addrin.sin_port, sizeof(temp_addrin.sin_port)));
    CU_ASSERT(!memcmp(&(addrin_ptr->sin_zero), &temp_addrin.sin_zero, sizeof(temp_addrin.sin_zero)));
    
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->connect, AlpacaWolf_connect);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->listen, AlpacaWolf_listen);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->accept, AlpacaWolf_accept);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->read, AlpacaWolf_recv);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->write, AlpacaWolf_send);
    CU_ASSERT_PTR_EQUAL(temp_commsCTX->close, AlpacaWolf_close);

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
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_TYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_PROTO_UDP);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    /************************************************************************************* 
     * Attempt to initialize obj already initialized 
     *************************************************************************************/
    // Proper
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(temp_commsCTX);

    // Improper
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    // Clean
    result = AlpacaComms_destroyCtx(&temp_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    /************************************************************************************* 
     * Attempt to initialize obj with unsupported comms type 
     *************************************************************************************/
    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_PROTO_TLS13 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_PROTO_UDP | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_PROTO_SSH | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_UNSUPPORTED);
    CU_ASSERT_PTR_NULL(temp_commsCTX);

    result = AlpacaComms_initCtx(&temp_commsCTX, ALPACACOMMS_PROTO_SSH | ALPACACOMMS_TYPE_CLIENT | ALPACACOMMS_TYPE_SERVER);
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

    result = AlpacaComms_initCtx(&server_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_SERVER);
    if(result){
        LOGERROR("Error in server_thread during init...\n");
        goto exit;
    }

    //setsockopt(server_commsCTX->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    /*************************************************************************************
     * Set up server 
     *************************************************************************************/
    result = AlpacaComms_listen(server_commsCTX, UNITTEST_DEFAULT_PORT);
    if(result){
        LOGERROR("Error in server_thread during listen...\n");
        goto exit;
    }

    while(!thrd_shutdown){
        pthread_mutex_unlock(&write_lock);
        //LOGDEBUG("THREAD RELEASED WRITE_LOCK\n");
        pthread_mutex_lock(&read_lock);
        //LOGDEBUG("THREAD ACQUIRED READ_LOCK\n");
        LOGDEBUG("*** READING ***\n");
        memset(&thrd_serv_buffer, 0, sizeof(thrd_serv_buffer));
        result = AlpacaComms_recv(server_commsCTX, &thrd_serv_buffer, sizeof(thrd_serv_buffer), &numread);
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
//    struct sockaddr_in *addrin_ptr = NULL;

    pthread_t server;
    int ret = 0;
    


    memset(&temp_addrin, 0, sizeof(struct sockaddr_in));

    result = AlpacaComms_init(ALPACACOMMS_PROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);

    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);
    
    /*************************************************************************************
     * Init TLS 1.2 CLIENT
     * Initiate a VALID connection
     * Verify results, then close
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);


    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_FATAL(client_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);

    AlpacaUtilities_mSleep(1000);

    result = AlpacaComms_close(client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    CU_ASSERT_PTR_NULL(client_commsCTX->protoCtx);
    CU_ASSERT_EQUAL(client_commsCTX->fd, -1);
    CU_ASSERT_EQUAL(GET_COMMS_TYPE(client_commsCTX->flags), ALPACACOMMS_TYPE_CLIENT);

    thrd_shutdown = 1;
    pthread_join(server,NULL);
    thrd_shutdown = 0;


    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    /* Invalid ctx pointer passed */
    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    
    /************************************************************************************* 
     * Invalid ctx pointer passed 
     * invalid IP passed
     *************************************************************************************/
    result = AlpacaComms_connect(client_commsCTX, "", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);

    /************************************************************************************* 
     * valid ctx pointer passed 
     * Invalid IP passed
     * Invalid port
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(client_commsCTX, "", 0);
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
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", 0);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);


    /*************************************************************************************
     * Valid params but attempt to connect to non-listening port
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", 12345);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_COMMSCONNECT);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    /*************************************************************************************
     * Connect to listening service but with protocol mismatch
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", 22);
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

    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    CU_ASSERT(client_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);
    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADSTATE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    CU_ASSERT(client_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);

    AlpacaUtilities_mSleep(500);

    result = AlpacaComms_close(client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    CU_ASSERT_PTR_NULL(client_commsCTX->protoCtx);

    CU_ASSERT_EQUAL(client_commsCTX->fd, -1);
    CU_ASSERT(GET_COMMS_TYPE(client_commsCTX->flags) & ALPACACOMMS_TYPE_CLIENT);

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

    result = AlpacaComms_init(ALPACACOMMS_PROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);

    /*************************************************************************************
     * Baseline connect and send. 
     * Verify contents and send values
     *************************************************************************************/
    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);

    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);

    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT(client_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);

    pthread_mutex_lock(&write_lock);
    strcpy((char*)local_buffer,"WAZZZZZZUP!");
    result = AlpacaComms_send(client_commsCTX, local_buffer, strlen((char*)local_buffer), &out);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(out, strlen((char*)local_buffer));

    // Grab read lock to ensure buffer is in a readable state
    pthread_mutex_lock(&read_lock);
    LOGINFO("local: %s thrd: %s\n", (char*)local_buffer, (char*)thrd_serv_buffer);
    CU_ASSERT_NSTRING_EQUAL(thrd_serv_buffer,local_buffer,strlen((char*)local_buffer));
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

    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);

    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT(client_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);

    pthread_mutex_lock(&write_lock);
    rand_stream = gen_rdm_bytestream(ONE_KB);
    result = AlpacaComms_send(client_commsCTX, rand_stream, ONE_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(out, ONE_KB);

    // Grab read lock to ensure buffer is in a readable state
    pthread_mutex_lock(&read_lock);
    CU_ASSERT_NSTRING_EQUAL(thrd_serv_buffer, rand_stream, ONE_KB);
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

    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);

    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT(client_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);

    /* @TODO
     * This is far past what unit tests should be doing
     * Blending into functional testing
     */

    for(int i = 0; i < ONE_KB; i++){
        rand_stream = gen_rdm_bytestream(FOUR_KB);
        pthread_mutex_lock(&write_lock);
        LOGDEBUG("***** Send [%d] ***** \n", i+1);
        result = AlpacaComms_send(client_commsCTX, rand_stream, FOUR_KB, &out);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(out, FOUR_KB);

        // Grab read lock to ensure buffer is in a readable state
        pthread_mutex_lock(&read_lock);
        CU_ASSERT_NSTRING_EQUAL(thrd_serv_buffer, rand_stream, FOUR_KB);
        
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
    result = AlpacaComms_send(client_commsCTX, rand_stream, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_EQUAL(out, 0);

    // Now init
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);

    // Bad len
    result = AlpacaComms_send(client_commsCTX, rand_stream, 0, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_EQUAL(out, 0);
    free(rand_stream);
    rand_stream = NULL;

    // Bad buff
    result = AlpacaComms_send(client_commsCTX, rand_stream, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_EQUAL(out, 0);

    // Bad buff but with out var pre-set
    out = 123;
    result = AlpacaComms_send(client_commsCTX, rand_stream, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_EQUAL(out, 0);

    // Now connect
    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT(client_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);
    pthread_mutex_lock(&write_lock);
    rand_stream = gen_rdm_bytestream(FOUR_KB);
    
    // Final send
    result = AlpacaComms_send(client_commsCTX, rand_stream, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(out, FOUR_KB);
    // Grab read lock to ensure buffer is in a readable state
    pthread_mutex_lock(&read_lock);
    CU_ASSERT_NSTRING_EQUAL(thrd_serv_buffer, rand_stream, FOUR_KB);
    
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

static
void* client_thread(void* args){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    Alpaca_commsCtx_t *client_commsCtx = NULL;
    ssize_t numsent = 0;
    
    AlpacaUtilities_mSleep(250);

    result = AlpacaComms_initCtx(&client_commsCtx, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    if(result){
        LOGERROR("Error in client_thread during init...\n");
        goto exit;
    }

    /*************************************************************************************
     * Set up client 
     *************************************************************************************/
    result = AlpacaComms_connect(client_commsCtx, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    if(result){
        LOGERROR("Error in client_thread during listen...\n");
        goto exit;
    }

    while(!thrd_shutdown){
        ready = 0;
        thrd_cli_buffer = gen_rdm_bytestream(FOUR_KB);
        result = AlpacaComms_send(client_commsCtx, thrd_cli_buffer, FOUR_KB, &numsent);
 
        while(ready != 1){
            AlpacaUtilities_mSleep(10);
        }
        free(thrd_cli_buffer);
        thrd_cli_buffer = NULL;
    }

exit:
    if(thrd_cli_buffer){
        free(thrd_cli_buffer);
        thrd_cli_buffer = NULL;
    }
    result = AlpacaComms_destroyCtx(&client_commsCtx);
    return NULL;
}

void AlpacaUnit_comms_recv(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    Alpaca_commsCtx_t *server_commsCTX = NULL;
    pthread_t client;
    int ret = 0;
    ssize_t out = 0;

    char local_buffer[FOUR_KB] = {0};
    memset(local_buffer, 0, sizeof(local_buffer));

    result = AlpacaComms_init(ALPACACOMMS_PROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);

    /*************************************************************************************
     * Baseline Listen and recv. 
     * Verify contents and recv values
     *************************************************************************************/
    result = AlpacaComms_initCtx(&server_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    setsockopt(server_commsCTX->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    ret = pthread_create(&client, NULL, client_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
   
    result = AlpacaComms_listen(server_commsCTX, UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
   
    memset(local_buffer, 0, sizeof(local_buffer));
    result = AlpacaComms_recv(server_commsCTX, local_buffer, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(out, FOUR_KB);
    CU_ASSERT_NSTRING_EQUAL(thrd_cli_buffer, local_buffer, FOUR_KB);

    thrd_shutdown = 1;
    out = 0;
    // UNlocking this allows thread to move forward
    
    ready = 1;
    pthread_join(client,NULL);
    thrd_shutdown = 0;
    ready = 0;

    // Clean
    result = AlpacaComms_destroyCtx(&server_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(server_commsCTX);



/*
    result = AlpacaComms_initCtx(&server_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    setsockopt(server_commsCTX->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    ret = pthread_create(&client, NULL, client_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    
    result = AlpacaComms_listen(server_commsCTX, UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_EQUAL(server_commsCTX->status, ALPACACOMMS_STATUS_TLSCONN);

    for(int i = 0; i < ONE_KB; i++){
        LOGDEBUG("***** RECV [%d] ***** \n", i+1);
        result = AlpacaComms_recv(server_commsCTX, local_buffer, FOUR_KB, &out);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(out, FOUR_KB);
        CU_ASSERT_NSTRING_EQUAL(thrd_cli_buffer, local_buffer, FOUR_KB);
        memset(&local_buffer, 0, sizeof(local_buffer));
        ready = 1;
        if(i%64 == 0){printf(".");}
    }
    thrd_shutdown = 1;
    ready = 1;


    // Clean
    result = AlpacaComms_destroyCtx(&server_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(server_commsCTX);

    
    pthread_join(client,NULL);
    thrd_shutdown = 0;
*/

    /*************************************************************************************
     * Setup valid context then procede to pass bad args finally passing proper values
     * Verify contents and recv values
     *************************************************************************************/

    result = AlpacaComms_initCtx(&server_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    setsockopt(server_commsCTX->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    ret = pthread_create(&client, NULL, client_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
   
    LOGDEBUG("HERE\n");
    result = AlpacaComms_listen(server_commsCTX, UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);

    
    LOGDEBUG("HERE\n");
    result = AlpacaComms_recv(server_commsCTX, local_buffer, 0, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);

    LOGDEBUG("HERE\n");
    result = AlpacaComms_recv(server_commsCTX, NULL, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);

    LOGDEBUG("HERE\n");
    result = AlpacaComms_recv(server_commsCTX, local_buffer, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    CU_ASSERT_NSTRING_EQUAL(thrd_cli_buffer, local_buffer, FOUR_KB);
    memset(&local_buffer, 0, sizeof(local_buffer));
    thrd_shutdown = 1;
    ready = 1;

    LOGDEBUG("HERE\n");
    // Clean
    result = AlpacaComms_destroyCtx(&server_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(server_commsCTX);

    pthread_join(client,NULL);
    thrd_shutdown = 0;

    LOGDEBUG("HERE\n");
    // Last failure check, pass null CTX
    result = AlpacaComms_recv(server_commsCTX, local_buffer, FOUR_KB, &out);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(server_commsCTX);

    
    

    result = AlpacaComms_cleanUp();
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);

    return;
}


void AlpacaUnit_comms_listen(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    Alpaca_commsCtx_t *server_commsCTX = NULL;
    pthread_t client;
    int ret = 0;
    

    result = AlpacaComms_init(ALPACACOMMS_PROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);

    /*************************************************************************************
     * Baseline Listen. 
     *************************************************************************************/
    result = AlpacaComms_initCtx(&server_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    setsockopt(server_commsCTX->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    ret = pthread_create(&client, NULL, client_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    
    result = AlpacaComms_listen(server_commsCTX, UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    CU_ASSERT(server_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);

    // Clean
    result = AlpacaComms_destroyCtx(&server_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(server_commsCTX);

    thrd_shutdown = 1;
    ready = 1;

    pthread_join(client,NULL);
    thrd_shutdown = 0;


    /*
     * Test failure cases
     */
    result = AlpacaComms_listen(server_commsCTX, UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(server_commsCTX);

    result = AlpacaComms_initCtx(&server_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    setsockopt(server_commsCTX->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    // Bad port but valid CTX
    result = AlpacaComms_listen(server_commsCTX, 1023);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);

    // Clean
    result = AlpacaComms_destroyCtx(&server_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(server_commsCTX);

    result = AlpacaComms_initCtx(&server_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_SERVER);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    setsockopt(server_commsCTX->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    ret = pthread_create(&client, NULL, client_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
       
    result = AlpacaComms_listen(server_commsCTX, UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(server_commsCTX);
    CU_ASSERT(server_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);

    // Clean
    result = AlpacaComms_destroyCtx(&server_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(server_commsCTX);

    thrd_shutdown = 1;
    ready = 1;

    pthread_join(client,NULL);
    thrd_shutdown = 0;

    // Clean
    result = AlpacaComms_destroyCtx(&server_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(server_commsCTX);

    result = AlpacaComms_cleanUp();
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);

    return;
}

void AlpacaUnit_comms_close(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;

    Alpaca_commsCtx_t *client_commsCTX = NULL;
    struct sockaddr_in temp_addrin;

    pthread_t server;
    int ret = 0;

    memset(&temp_addrin, 0, sizeof(struct sockaddr_in));

    result = AlpacaComms_init(ALPACACOMMS_PROTO_TLS12);
    CU_ASSERT_EQUAL_FATAL(result, ALPACA_SUCCESS);

    ret = pthread_create(&server, NULL, server_thread, NULL);
    CU_ASSERT_FALSE_FATAL(ret);
    AlpacaUtilities_mSleep(1000);
    
    /*************************************************************************************
     * Init TLS 1.2 CLIENT
     * Initiate a VALID connection
     * Verify results, then close
     *************************************************************************************/
    result = AlpacaComms_initCtx(&client_commsCTX, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);


    result = AlpacaComms_connect(client_commsCTX, "127.0.0.1", UNITTEST_DEFAULT_PORT);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_FATAL(client_commsCTX->status & ALPACACOMMS_STATUS_TLSCONN);

    AlpacaUtilities_mSleep(1000);

    result = AlpacaComms_close(client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(client_commsCTX);
    CU_ASSERT_EQUAL(ALPACACOMMS_STATUS_NOTCONN, client_commsCTX->status);
    CU_ASSERT_PTR_NULL(client_commsCTX->protoCtx);

    thrd_shutdown = 1;
    pthread_join(server,NULL);
    thrd_shutdown = 0;

    // Clean
    result = AlpacaComms_destroyCtx(&client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(client_commsCTX);

    result = AlpacaComms_close(client_commsCTX);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(client_commsCTX);



    result = AlpacaComms_cleanUp();
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);

    return;
}