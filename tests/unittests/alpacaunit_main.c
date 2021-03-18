// Standard
#include <stdio.h> 
#include <time.h>
#include <stdlib.h>

// CUnit
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

// Internal
#include <core/logging.h> 

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
        (NULL == CU_add_test(pSuite1,"Alpac
        aBuffer_ensureRoom", AlpacaUnit_buffer_ensureRoom)) ||
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
        (NULL == CU_add_test(pSuite1,"AlpacaComms_connect", AlpacaUnit_comms_connect)) ||
        (NULL == CU_add_test(pSuite1,"AlpacaComms_close", AlpacaUnit_comms_close))){
        CU_cleanup_registry();
        exit(CU_get_error());
    }
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    
    return CU_get_error();

}

        
        //(NULL == CU_add_test(pSuite1,"AlpacaComms_send", AlpacaUnit_comms_send))   || 
        //(NULL == CU_add_test(pSuite1,"AlpacaComms_recv", AlpacaUnit_comms_recv))   ||
        //(NULL == CU_add_test(pSuite1,"AlpacaComms_listen", AlpacaUnit_comms_listen)) ||

        //||

        //