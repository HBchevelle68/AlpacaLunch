#include <stdlib.h>
#include <time.h>

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

static inline int random_int(int min, int max){
   return min + rand() % (max+1 - min);
}


unsigned char *gen_rdm_bytestream (size_t num_bytes){

  unsigned char *stream = malloc (num_bytes);
  size_t i;

  for (i = 0; i < num_bytes; i++) {
    stream[i] = rand ();
  }

  return stream;
}



void AlpacaUnit_buffer_base(void){

    ALPACA_STATUS result = ALPACA_SUCCESS;

    /*************************************************************************************
     * Verify base allocation and member assignment
     *************************************************************************************/
    CU_ASSERT_PTR_NULL(alpaca_buffer);
    result = AlpacaBuffer_init(&alpaca_buffer, FOUR_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, FOUR_KB);

    /*************************************************************************************
     * Verify base de-allocation and pointer cleanup
     *************************************************************************************/
    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*************************************************************************************
     * Verify base allocation and member assignment
     * to max allowed buffer size
     *************************************************************************************/
    result = AlpacaBuffer_init(&alpaca_buffer, MAXMEM);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, MAXMEM);

    /*************************************************************************************
     * Verify base de-allocation and pointer cleanup
     *************************************************************************************/
    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*************************************************************************************
     * Verify that attempts to allocate past max allowed
     * gracefully fails
     *************************************************************************************/
    result = AlpacaBuffer_init(&alpaca_buffer, MAXMEM+1);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*************************************************************************************
     * Verify de-allocation gracefully fails with
     * invalid pointer
     *************************************************************************************/
    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_MEMNOBUFFER);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    return;
}

void AlpacaUnit_buffer_append(void){
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    unsigned char* rand_stream = NULL;
    size_t rand_num = 0;
    size_t prev_index = 0;
    size_t prev_size = 0;

    /*************************************************************************************
     * Baseline append test, verify data can be
     * written accurately, repeticiously, and verified
     *************************************************************************************/
    CU_ASSERT_PTR_NULL(alpaca_buffer);
    result = AlpacaBuffer_init(&alpaca_buffer,TEN_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, TEN_KB);

    for(int i = 0; i<300; i++){
        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(alpaca_buffer->index, (HWSTRSZ*(i+1)));
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(HWSTRSZ*i)), HELLOWORLD, HWSTRSZ);
    }

    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*************************************************************************************
     * Test appends ability to properly expand memory
     * on-demand 
     *************************************************************************************/
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    result = AlpacaBuffer_init(&alpaca_buffer,ONE_KB);

    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, ONE_KB);

    for(int i = 0; i<86; i++){

        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);

        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(alpaca_buffer->index, (HWSTRSZ*(i+1)));
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(HWSTRSZ*i)), HELLOWORLD, HWSTRSZ);
    }

    // Save old values for ASSERTs
    prev_index = alpaca_buffer->index;
    prev_size  = alpaca_buffer->size;

    // Should see proper resizing
    for(int i = 0; i<85; i++){

        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);

        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(alpaca_buffer->index, prev_index+(HWSTRSZ*(i+1)));
        CU_ASSERT_EQUAL(alpaca_buffer->size, prev_size+(HWSTRSZ*(i+1)));
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(prev_index+(HWSTRSZ*i))), HELLOWORLD, HWSTRSZ);
    }

    prev_index = 0;
    prev_size  = 0;
    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    
    /*************************************************************************************
     * Test appends ability to properly append random sizes 
     *************************************************************************************/
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    result = AlpacaBuffer_init(&alpaca_buffer,TEN_KB);

    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, TEN_KB);

    srand(time(0));
    

    for(int i = 0; i<300; i++){

        // Set up
        rand_num = random_int(1,30);
        rand_stream = gen_rdm_bytestream(rand_num);
        prev_index = alpaca_buffer->index;

        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)rand_stream, rand_num);

        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
        CU_ASSERT_EQUAL(alpaca_buffer->index, prev_index+rand_num);
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(prev_index)), rand_stream, rand_num);
        
        free(rand_stream);
        rand_stream = NULL;
    }

    prev_index = 0;
    prev_size  = 0;
    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    // Double check
    if(rand_stream){
        free(rand_stream);
    }

    return;
}

void AlpacaUnit_buffer_resize(void){
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    size_t prev_index = 0;
    //size_t rand_num = 0;
    //size_t prev_size = 0;

    /*************************************************************************************
     * Baseline resize test. init at 1KB grow to 10KB
     *************************************************************************************/
    CU_ASSERT_PTR_NULL(alpaca_buffer);
    result = AlpacaBuffer_init(&alpaca_buffer,ONE_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, ONE_KB);

    result = AlpacaBuffer_resize(&alpaca_buffer, TEN_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, TEN_KB);

    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*************************************************************************************
     * Resize (GROW) a buffer that has been written to. Verify contents post resize
     *************************************************************************************/

    CU_ASSERT_PTR_NULL(alpaca_buffer);
    result = AlpacaBuffer_init(&alpaca_buffer,ONE_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, ONE_KB);
    
    for(int i = 0; i<300; i++){
        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    }

    // Save previous index for ASSERT
    prev_index = alpaca_buffer->index;

    result = AlpacaBuffer_resize(&alpaca_buffer, FOUR_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, prev_index);
    CU_ASSERT_EQUAL(alpaca_buffer->size, FOUR_KB);

    for(int i = 0; i<300; i++){
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(HWSTRSZ*i)), HELLOWORLD, HWSTRSZ);
    }

    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    /*************************************************************************************
     * Resize (SHRINK) a buffer that has been written to. Verify contents post resize
     *************************************************************************************/
    CU_ASSERT_PTR_NULL(alpaca_buffer);
    result = AlpacaBuffer_init(&alpaca_buffer, TEN_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, TEN_KB);
    
    for(int i = 0; i<300; i++){
        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    }

    // Save previous index for ASSERT
    prev_index = alpaca_buffer->index;

    result = AlpacaBuffer_resize(&alpaca_buffer, FOUR_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, prev_index);
    CU_ASSERT_EQUAL(alpaca_buffer->size, FOUR_KB);

    for(int i = 0; i<300; i++){
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(HWSTRSZ*i)), HELLOWORLD, HWSTRSZ);
    }

    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);    
    
    /*************************************************************************************
     * Attempt to Resize (GROW) a buffer that has been written to past 
     * the maximum size allowed. verify buffer intact and conents unchanged
     *************************************************************************************/
    CU_ASSERT_PTR_NULL(alpaca_buffer);
    result = AlpacaBuffer_init(&alpaca_buffer, TEN_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, TEN_KB);

    for(int i = 0; i<300; i++){
        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    }

    prev_index = alpaca_buffer->index;

    result = AlpacaBuffer_resize(&alpaca_buffer, MAXMEM+1);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, prev_index);
    CU_ASSERT_EQUAL(alpaca_buffer->size, TEN_KB);

    for(int i = 0; i<300; i++){
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(HWSTRSZ*i)), HELLOWORLD, HWSTRSZ);
    }

    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer);    

    /*************************************************************************************
     * Attempt to Resize (SHRINK) a buffer that has been written to below the min 
     * amount needed to preserve data. Should fail gracefully and data be intact
     *************************************************************************************/

    result = AlpacaBuffer_init(&alpaca_buffer, FOUR_KB);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, FOUR_KB);

    for(int i = 0; i<300; i++){
        result = AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, HWSTRSZ);
        CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    }

    prev_index = alpaca_buffer->index;

    result = AlpacaBuffer_resize(&alpaca_buffer, ONE_KB);
    CU_ASSERT_EQUAL(result, ALPACA_ERROR_BADPARAM);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, prev_index);
    CU_ASSERT_EQUAL(alpaca_buffer->size, FOUR_KB);

    for(int i = 0; i<300; i++){
        CU_ASSERT_NSTRING_EQUAL((alpaca_buffer->buffer+(HWSTRSZ*i)), HELLOWORLD, HWSTRSZ);
    }

    result = AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_EQUAL(result, ALPACA_SUCCESS);
    CU_ASSERT_PTR_NULL(alpaca_buffer); 

    return;
}