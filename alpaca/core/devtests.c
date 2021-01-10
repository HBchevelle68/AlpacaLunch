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
#include <interfaces/threadpool_interface.h>


#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>


#if DEBUGENABLE
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



#else
static inline void mem_api_test(void){}
static inline void printTypeSizes(void){}
static inline void BasicCrypto(void){}
//static inline void threadpool_api_test(void){}
#endif

static int diff(int a, int b){
    return a-b;
}

void test_diff(void)//our test function 2
{
    CU_ASSERT(1==diff(3,2));
    CU_ASSERT(-3==diff(3,6));
    CU_ASSERT(-9==diff(-3,6));
    CU_ASSERT(-6==diff(0,6));
}
/*
static
void threadpool_api_test(void) {

    ALPACA_STATUS result = ALPACA_SUCCESS;

    //result = AlpacaThreadpool_initPool((uint8_t)10);
    
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    
    
}
*/

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

void DevTests(void){

    CU_pSuite pSuite1 = NULL;

    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry()){
        exit(CU_get_error());
    }

    // Add suite1 to registry
    pSuite1 = CU_add_suite("TEST SUITE", init_suite, clean_suite);
    if (NULL == pSuite1) {
        CU_cleanup_registry();
        exit(CU_get_error());
    }

    // add test1 to suite1
    if ((NULL == CU_add_test(pSuite1,"Testing Diff Func", test_diff)))
    {
        CU_cleanup_registry();
        exit(CU_get_error());
    }


    CU_basic_run_tests();// OUTPUT to the screen

    CU_cleanup_registry();//Cleaning the Registry
    //return CU_get_error();

/*
    printTypeSizes();
    BasicCrypto();
    mem_api_test();
    threadpool_api_test();
*/
    exit(CU_get_error());
}

