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

#define ADD_SUITE(name, init, clean)\
    do{\
        AlpacaUnit_CU_pSuite = CU_add_suite(name, init, clean);\
        if (NULL == AlpacaUnit_CU_pSuite) {\
            CU_cleanup_registry();\
            exit(CU_get_error());\
        }\
        }while(0)

#define ADD_TEST(suite, name, test)\
    do{\
        if ((NULL == CU_add_test(suite,name,test))){\
            CU_cleanup_registry();\
            exit(CU_get_error());\
        }\
        }while(0)

int main(){

    CU_pSuite AlpacaUnit_CU_pSuite = NULL;

    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry()){
        exit(CU_get_error());
    }
    
    ADD_SUITE("AlpacaLunch Memory Unit Tests", 
              AlpacaUnit_memory_initSuite,
              AlpacaUnit_memory_cleanSuite);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaBuffer_init/AlpacaBuffer_free", AlpacaUnit_buffer_base);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaBuffer_append", AlpacaUnit_buffer_append);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaBuffer_resize", AlpacaUnit_buffer_resize);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaBuffer_ensureRoom",AlpacaUnit_buffer_ensureRoom);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaBuffer_zero", AlpacaUnit_buffer_zero);


    ADD_SUITE("AlpacaLunch Comms Unit Tests",
              AlpacaUnit_comms_initSuite,
              AlpacaUnit_comms_cleanSuite);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaComms_initCtx/destroyCtx", AlpacaUnit_comms_base);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaComms_connect", AlpacaUnit_comms_connect);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaComms_send", AlpacaUnit_comms_send);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaComms_recv", AlpacaUnit_comms_recv);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaComms_listen", AlpacaUnit_comms_listen);
    ADD_TEST(AlpacaUnit_CU_pSuite,"AlpacaComms_close", AlpacaUnit_comms_close);


    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    
    return CU_get_error();

}

        
        //
        //
        //(NULL == CU_add_test(pSuite1,"AlpacaComms_listen", AlpacaUnit_comms_listen)) ||

        //||

        //