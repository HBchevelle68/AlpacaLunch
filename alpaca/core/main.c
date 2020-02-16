// Standard
#include <stdio.h> 
#include <time.h>

// Internal
#include <core/logging.h> 
#include <core/crypto.h>
#include <core/server.h>
#include <core/macros.h>
#include <core/sighandler.h>
#include <core/allu.h>

// Interfaces 
#include <interfaces/threadpool_interface.h>
#include <interfaces/memory_interface.h>

/*
 * Test harness
 */
#ifndef SNOW_ENABLED

char HELLOWORLD[] = "HELLO WORLD!";

void memtest(void){

    ALLU_Buffer_t *testBuff  = NULL;

    // Test init
    testBuff = AlpacaBuffer_init(4000);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", testBuff, testBuff->size, testBuff->index);
    
    // Test ensureRoom and resize (GROW)
    AlpacaBuffer_ensureRoom(&testBuff, 1024*10);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", testBuff, testBuff->size, testBuff->index);
    AlpacaMemory_dumpHex(testBuff->buffer, testBuff->index);

    // Test append
    for(int i = 0; i<10; i++){
        AlpacaBuffer_append(&testBuff, (uint8_t*)HELLOWORLD, 13);
    }
    AlpacaMemory_dumpHex(testBuff->buffer, testBuff->index);


    // Test zero
    AlpacaBuffer_zero(&testBuff);
    AlpacaMemory_dumpHex(testBuff->buffer, testBuff->index);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", testBuff, testBuff->size, testBuff->index);

    // Test resize (SHRINK)
    testBuff = AlpacaBuffer_resize(&testBuff, 1024);
    AlpacaMemory_dumpHex(testBuff->buffer, testBuff->index);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", testBuff, testBuff->size, testBuff->index);

    // Test free
    AlpacaBuffer_free(&testBuff);
    LOGDEBUG("Buffer [%p]\n", testBuff);
   
    while(1){}
}



int main(){

    
    FAIL_IF_TRUE(alpacacore_init());
    int temp = 10;

    LOGERROR("This is a test: %d\n", temp);
    LOGDEBUG("This is a test: %d\n", temp);
    LOGINFO("This is a test: %d\n", temp);

    // START Threadpool Test
/*    
    ALtpool_t* tpool = NULL;
    tpool = AlpacaThreadpool_init(10);
    if(tpool != NULL){
        LOGDEBUG("Created thread pool of size: %d\n", tpool->t_size);
    }
    else{
        LOGDEBUG("Threadpool is NULL!!!\n");
    }
    
    temp = AlpacaThreadpool_exit(tpool);
    LOGDEBUG("Return from AlpacaThreadpool_exit: %d\n", temp);

    LOGDEBUG("End tests....\n");
*/
    memtest();

    /* TO DO
     * 
     * Needs some slight rework, core loop, should never really
     * return. If it does should examine the cause.
     * currently the way the loop is built there isn't a good reason to
     */
    //alpacacore_server_run(12345, 20);


    


    alpacacore_exit();
   
    return 0; 
}


/*
 * Ignoring for now, will likely return to later
 */ 

#else
#if __STDC_VERSION__ >= 199901L
   /*
    * C99 or higher
    * 
    * Included for pid_t usage in snow.h 
    */
   #include <unistd.h>
   #include <sys/types.h>
#endif

#include <snow.h>
    snow_main();
    
#endif 

#ifdef SNOW_ENABLED
#include <core/server.h>
#include <core/crypto.h>

#define TEST_SUCCESS 1
#define TEST_FAILURE 0
/*
int checkport(uint16_t testport){
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = BEU16(testport);
    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        if(errno == EADDRINUSE) {
            return TEST_SUCCESS;
        } 
    }
    return TEST_FAILURE;
}

describe(networking) {

    
    
    after_each() {
        alpacacore_server_clean();
	}


}

describe(chacha20) {

    it("Testing that crypto worked?") {

	}


}
*/
#endif