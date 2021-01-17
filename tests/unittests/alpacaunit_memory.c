#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <unittests/alpaca_unit.h>

#include <core/logging.h>
#include <core/codes.h>
#include <interfaces/memory_interface.h>



ALLU_Buffer_t *alpaca_buffer;
static char HELLOWORLD[] = "HELLO WORLD!";
static const uint32_t HWSTRSZ = 12;


int AlpacaUnit_memory_initSuite(void){
    return 0;
}
int AlpacaUnit_memory_cleanSuite(void){

    if(NULL != alpaca_buffer){
        AlpacaBuffer_free(&alpaca_buffer);
        alpaca_buffer = NULL;
    }
    return 0;
}


void AlpacaUnit_buffer_base(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;

    /*
     * Verify base allocation and member assignment
     */
    CU_ASSERT_PTR_NULL(alpaca_buffer);
    result = AlpacaBuffer_init(&alpaca_buffer, FOUR_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, FOUR_KB);

    /*
     * Verify base de-allocation and pointer cleanup
     */
    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*
     * Verify base allocation and member assignment
     * to max allowed buffer size
     */
    result = AlpacaBuffer_init(&alpaca_buffer, MAXMEM);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, MAXMEM);

    /*
     * Verify base de-allocation and pointer cleanup
     */
    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*
     * Verify that attempts to allocate past max allowed
     * gracefully fails
     */
    result = AlpacaBuffer_init(&alpaca_buffer, MAXMEM+1);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*
     * Verify de-allocation gracefully fails with
     * invalid pointer
     */
    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_MEMNOBUFFER);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    return;
}

void AlpacaUnit_buffer_append(void){
    
    ALPACA_STATUS result = ALPACA_SUCCESS;

    /*
     * Baseline append test, verify data can be
     * written accurately, repeticiously, and verified
     */
    CU_ASSERT_PTR_NULL(alpaca_buffer);
    AlpacaBuffer_init(&alpaca_buffer,TEN_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, TEN_KB);

    for(int i = 0; i<300; i++){
        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(alpaca_buffer->index, (HWSTRSZ*(i+1)));
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(HWSTRSZ*i)),HELLOWORLD,HWSTRSZ);
    }

    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*
     * Test appends ability to properly expand memory
     * on-demand 
     */
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    AlpacaBuffer_init(&alpaca_buffer,ONE_KB);

    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, ONE_KB);

    for(int i = 0; i<86; i++){

        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);

        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(alpaca_buffer->index, (HWSTRSZ*(i+1)));
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(HWSTRSZ*i)),HELLOWORLD,HWSTRSZ);
    }

    // Save old values for ASSERTs
    size_t prev_index = alpaca_buffer->index;
    size_t prev_size  = alpaca_buffer->size;

    // Should see proper resizing
    for(int i = 0; i<85; i++){

        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);

        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(alpaca_buffer->index, prev_index+(HWSTRSZ*(i+1)));
        CU_ASSERT_EQUAL(alpaca_buffer->size, prev_size+(HWSTRSZ*(i+1)));
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(prev_index+(HWSTRSZ*i))),HELLOWORLD,HWSTRSZ);
    }

    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    
    
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    AlpacaBuffer_init(&alpaca_buffer,FOUR_KB);

    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, FOUR_KB);

    
    return;
}