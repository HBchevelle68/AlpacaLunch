#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <core/logging.h>
#include <interfaces/memory_interface.h>

ALLU_Buffer_t *alpaca_buffer;
static char HELLOWORLD[] = "HELLO WORLD!";


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

static __attribute__((unused))
void memtest (void){

    // Test init
    alpaca_buffer = AlpacaBuffer_init(4000);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", alpaca_buffer, alpaca_buffer->size, alpaca_buffer->index);
    
    // Test ensureRoom and resize (GROW)
    AlpacaBuffer_ensureRoom(&alpaca_buffer, 1024*10);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", alpaca_buffer, alpaca_buffer->size, alpaca_buffer->index);
    AlpacaMemory_dumpHex(alpaca_buffer->buffer, alpaca_buffer->index);

    // Test append
    for(int i = 0; i<10; i++){
        AlpacaBuffer_append(&alpaca_buffer, (uint8_t*)HELLOWORLD, 13);
    }
    AlpacaMemory_dumpHex(alpaca_buffer->buffer, alpaca_buffer->index);


    // Test zero
    AlpacaBuffer_zero(&alpaca_buffer);
    AlpacaMemory_dumpHex(alpaca_buffer->buffer, alpaca_buffer->index);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", alpaca_buffer, alpaca_buffer->size, alpaca_buffer->index);

    // Test resize (SHRINK)
    alpaca_buffer = AlpacaBuffer_resize(&alpaca_buffer, 1024);
    AlpacaMemory_dumpHex(alpaca_buffer->buffer, alpaca_buffer->index);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", alpaca_buffer, alpaca_buffer->size, alpaca_buffer->index);

    // Test free
    AlpacaBuffer_free(&alpaca_buffer);
    LOGDEBUG("Buffer [%p]\n", alpaca_buffer);
    
    return; 
}

void AlpacaUnit_memory_base(void){

    CU_ASSERT_PTR_NULL(alpaca_buffer);
    
    alpaca_buffer = AlpacaBuffer_init(4000);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, 4000);

    AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    alpaca_buffer = AlpacaBuffer_init(MAXMEM);
    CU_ASSERT_PTR_NOT_NULL(alpaca_buffer);
    CU_ASSERT_EQUAL(alpaca_buffer->index, 0);
    CU_ASSERT_EQUAL(alpaca_buffer->size, MAXMEM);
    

    AlpacaBuffer_free(&alpaca_buffer);
    CU_ASSERT_PTR_NULL(alpaca_buffer);

    return;
}
