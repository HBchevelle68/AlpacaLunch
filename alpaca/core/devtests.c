#define _GNU_SOURCE // Temp for test
// Standard
#include <stdio.h> 
#include <time.h>

// Internal
#include <core/logging.h> 
#include <core/crypto.h>


// Interfaces 
#include <interfaces/memory_interface.h>
#include <interfaces/utility_interface.h>






static
void memtest(void){

    char HELLOWORLD[] = "HELLO WORLD!";
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
    
    return; 
}

static
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

    

    printf("**** ALPACA TYPES ****\n");
    printf("Size of ALLU_Buffer_t: %ld bytes\n", sizeof(ALLU_Buffer_t));
    printf("Size of ALLU_Buffer_t*: %ld bytes\n", sizeof(ALLU_Buffer_t*));

}

static
void BasicCrypto(void){
    uint8_t plain[64]  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\0";
    uint8_t cipher[64];
    memset(cipher, 0, 64);

    MyEncrypt(plain, 64, cipher);
    memset(plain, 0, 64);
    MyDecrypt(plain, 64, cipher);
}



void DevTests(void){
    printTypeSizes();
    BasicCrypto();
    memtest();
    return;
}