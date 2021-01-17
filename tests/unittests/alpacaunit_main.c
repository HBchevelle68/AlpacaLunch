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

// CUnit
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

// Internal Tests
#include <unittests/alpaca_unit.h>


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

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }



int main(){

    CU_pSuite pSuite1 = NULL;

    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry()){
        exit(CU_get_error());
    }

    // Add suite1 to registry
    pSuite1 = CU_add_suite("AlpacaLunch Memory Unit Tests", AlpacaUnit_memory_initSuite, AlpacaUnit_memory_cleanSuite);
    if (NULL == pSuite1) {
        CU_cleanup_registry();
        exit(CU_get_error());
    }

    if ((NULL == CU_add_test(pSuite1,"AlpacaBuffer_init/AlpacaBuffer_free", AlpacaUnit_buffer_base)) || 
        (NULL == CU_add_test(pSuite1,"AlpacaBuffer_append", AlpacaUnit_buffer_append))){
        CU_cleanup_registry();
        exit(CU_get_error());
    }


    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    
    return CU_get_error();

}