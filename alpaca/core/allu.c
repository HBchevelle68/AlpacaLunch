#include <stdlib.h>
// Internal
#include <interfaces/comms_interface.h>
#include <core/allu.h>
#include <core/logging.h>


ALPACA_STATUS AlpacaCore_init(uint16_t commsFlags){
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;

    DEBUGWARNING();

    // Check for root if in release
#ifndef DEBUGENABLE
    // NEED ERROR HANDLING!
    getuid();
#endif 

    result = AlpacaComms_init(commsFlags);
    if(result){
        LOGERROR("Error initializing Alpaca Comms\n");
        goto exit;
    }

exit:
    LEAVING;
    return result;
}


/*
 * Small for now. Called by sighandler
 * will get expandaed on
 */
ALPACA_STATUS AlpacaCore_exit(void){
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;
    
    result = AlpacaComms_cleanUp();
    if(result){
        LOGERROR("Error cleaning process comms\n");
    }

    LEAVING;
    return result;
}