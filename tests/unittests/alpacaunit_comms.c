
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


#define DEFAULTPORT 54321
#define CERTFILE ".testcerts/cert.pem"
#define KEYFILE  ".testcerts/private.pem"


Alpaca_commsCtx_t *testServComms;
Alpaca_commsCtx_t *testClientComms;


static pthread_mutex_t g_lock;
static ALPACA_STATUS flag;
static uint8_t *g_buffer;




void unit_server(void* args){
    int                sockfd;
    int                connd;
    struct sockaddr_in servAddr;
    struct sockaddr_in clientAddr;
    socklen_t          size = sizeof(clientAddr);
    char               g_buff[256];
    size_t             len;
    int                shutdown = 0;
    int                ret;
    const char*        reply = "I hear ya fa shizzle!\n";
    
    /* declare wolfSSL objects */
    WOLFSSL_CTX* ctx;
    WOLFSSL*     ssl;

    pthread_mutex_lock(&g_lock);


    /* Create a socket that uses an internet IPv4 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. 
     */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        LOGERROR(stderr, "ERROR: failed to create the socket\n");
        flag = ALPACA_FAILURE;
    }



    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL) {
        LOGERROR(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        flag = ALPACA_FAILURE;
    }

    /* Load server certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_certificate_file(ctx, CERTFILE, SSL_FILETYPE_PEM)
        != SSL_SUCCESS) {
        LOGERROR(stderr, "ERROR: failed to load %s, please check the file.\n",
                CERTFILE);
        flag = ALPACA_FAILURE;
    }

    /* Load server key into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_PrivateKey_file(ctx, KEYFILE, SSL_FILETYPE_PEM)
        != SSL_SUCCESS) {
        LOGERROR(stderr, "ERROR: failed to load %s, please check the file.\n",
                KEYFILE);
        flag = ALPACA_FAILURE;
    }



    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));

    /* Fill in the server address */
    servAddr.sin_family      = AF_INET;             /* using IPv4      */
    servAddr.sin_port        = htons(DEFAULTPORT); /* on DEFAULT_PORT */
    servAddr.sin_addr.s_addr = INADDR_ANY;          /* from anywhere   */



    /* Bind the server socket to our port */
    if (bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        LOGERROR(stderr, "ERROR: failed to bind\n");
        flag = ALPACA_FAILURE;
    }

    /* Listen for a new connection, allow 5 pending connections */
    if (listen(sockfd, 5) == -1) {
        LOGERROR(stderr, "ERROR: failed to listen\n");
        flag = ALPACA_FAILURE;
    }

    while (!flag) {
        LOGDEBUG("Waiting for a connection...\n");

        /* Accept client connections */
        if ((connd = accept(sockfd, (struct sockaddr*)&clientAddr, &size)) == -1) {
            LOGERROR(stderr, "ERROR: failed to accept the connection\n\n");
            flag = ALPACA_FAILURE;
        }

        /* Create a WOLFSSL object */
        if ((ssl = wolfSSL_new(ctx)) == NULL) {
            LOGERROR(stderr, "ERROR: failed to create WOLFSSL object\n");
            flag = ALPACA_FAILURE;
        }

        /* Attach wolfSSL to the socket */
        wolfSSL_set_fd(ssl, connd);

        /* Establish TLS connection */
        ret = wolfSSL_accept(ssl);
        if (ret != SSL_SUCCESS) {
            LOGERROR(stderr, "wolfSSL_accept error = %d\n",
                wolfSSL_get_error(ssl, ret));
            flag = ALPACA_FAILURE;
        }


        LOGDEBUG("Client connected successfully\n");



        /* Read the client data into our g_buff array */
        memset(g_buff, 0, sizeof(g_buff));
        if (wolfSSL_read(ssl, g_buff, sizeof(g_buff)-1) == -1) {
            LOGERROR(stderr, "ERROR: failed to read\n");
            flag = ALPACA_FAILURE;
        }
        flag = ALPACA_SUCCESS;
        pthread_mutex_unlock(&g_lock);
        LOGDEBUG("Client: %s\n", g_buff);


        /* Cleanup after this connection */
        wolfSSL_free(ssl);      /* Free the wolfSSL object              */
        close(connd);           /* Close the connection to the client   */
    }


    /* Cleanup and return */
    wolfSSL_CTX_free(ctx);  /* Free the wolfSSL context object          */
    close(sockfd);          /* Close the socket listening for clients   */
    return 0;               /* Return reporting a success               */



}

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