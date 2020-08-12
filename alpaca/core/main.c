#define _GNU_SOURCE // Temp for test
// Standard
#include <stdio.h> 
#include <time.h>

// Internal
#include <core/logging.h> 
#include <core/crypto.h>
#include <core/sighandler.h>
#include <core/allu.h>

// Interfaces 
#include <interfaces/threadpool_interface.h>
#include <interfaces/memory_interface.h>
#include <interfaces/comms_interface.h>

/*
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


void printTypeSizes(void){
        // sizeof evaluates the size of a variable
    printf("**** BUILT-IN TYPES ****\n");
    printf("Size of int: %ld bytes\n", sizeof(int));
    printf("Size of int*: %ld bytes\n", sizeof(int*));
    printf("Size of int**: %ld bytes\n", sizeof(int**));
    printf("Size of long int: %ld bytes\n", sizeof(long int));

    printf("Size of float: %ld bytes\n", sizeof(float));
    printf("Size of double: %ld bytes\n", sizeof(double));
    printf("Size of char: %ld byte\n", sizeof(char));
    printf("Size of char*: %ld byte\n", sizeof(char*));

    printf("**** STRUCT TYPES ****\n");
    printf("Size of struct ifaddrs: %ld bytes\n", sizeof(struct ifaddrs));
    printf("Size of struct hostent: %ld bytes\n", sizeof(struct hostent));
    printf("Size of struct utsname: %ld bytes\n", sizeof(struct utsname));
    

    printf("**** ALPACA TYPES ****\n");
    printf("Size of ALLU_Buffer_t: %ld bytes\n", sizeof(ALLU_Buffer_t));
    printf("Size of ALLU_Buffer_t*: %ld bytes\n", sizeof(ALLU_Buffer_t*));
    printf("Size of ALLU_net_info: %ld bytes\n", sizeof(ALLU_net_info));
    printf("Size of ALLU_server_ctx: %ld bytes\n", sizeof(ALLU_server_ctx));

}
*/

int testTemp = 0;

int main(){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

#ifndef DEBUGENABLE
    AlpacaUtilities_daemonize();
#endif

    // NEED ERROR CHECKING!

    result = AlpacaCore_init();
    if(result){
        LOGERROR("Initialization error code: %u\n", result);
        goto done;
    }

    LOGERROR("This is a test: %d\n", testTemp);
    LOGDEBUG("This is a test: %d\n", testTemp);
    LOGINFO("This is a test: %d\n", testTemp);
    // START Threadpool Test
   
done:
    AlpacaCore_exit(&result);
    LOGINFO("Exiting with exit code:%u\n", result);
    return 0; 
}
