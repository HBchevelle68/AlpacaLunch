// Standard
#include <stdio.h> 
#include <time.h>

// CUnit
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

// Internal
#include <core/logging.h> 
#include <core/crypto.h>

// Internal Tests
#include <unittests/alpaca_unit.h>


int main(){

    CU_pSuite pSuite1 = NULL;

    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry()){
        exit(CU_get_error());
    }
/*
    // Add mem tests to registry
    pSuite1 = CU_add_suite("AlpacaLunch Memory Unit Tests", AlpacaUnit_memory_initSuite, AlpacaUnit_memory_cleanSuite);
    if (NULL == pSuite1) {
        CU_cleanup_registry();
        exit(CU_get_error());
    }

    if ((NULL == CU_add_test(pSuite1,"AlpacaBuffer_init/AlpacaBuffer_free", AlpacaUnit_buffer_base)) || 
        (NULL == CU_add_test(pSuite1,"AlpacaBuffer_append", AlpacaUnit_buffer_append)) ||
        (NULL == CU_add_test(pSuite1,"AlpacaBuffer_resize", AlpacaUnit_buffer_resize)) ||
        (NULL == CU_add_test(pSuite1,"AlpacaBuffer_ensureRoom", AlpacaUnit_buffer_ensureRoom)) ||
        (NULL == CU_add_test(pSuite1,"AlpacaBuffer_zero", AlpacaUnit_buffer_zero))){
        CU_cleanup_registry();
        exit(CU_get_error());
    }
*/
    // Add comms tests to registry
    pSuite1 = CU_add_suite("AlpacaLunch Comms Unit Tests", AlpacaUnit_comms_initSuite, AlpacaUnit_comms_cleanSuite);
    if (NULL == pSuite1) {
        CU_cleanup_registry();
        exit(CU_get_error());
    }

    if ((NULL == CU_add_test(pSuite1,"AlpacaComms_initCtx/destroyCtx", AlpacaUnit_comms_base)) ||
        (NULL == CU_add_test(pSuite1,"AlpacaComms_connect", AlpacaUnit_comms_connect))){
        CU_cleanup_registry();
        exit(CU_get_error());
    }


    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    
    return CU_get_error();

}