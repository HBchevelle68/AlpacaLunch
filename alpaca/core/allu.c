#include <stdlib.h>
// Internal
#include <interfaces/comms_interface.h>
#include <core/allu.h>
#include <core/logging.h>
#include <core/sighandler.h>


ALPACA_STATUS AlpacaCore_init(void){
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;

    DEBUGWARNING();

    // Check for root if in release
#ifndef DEBUGENABLE
    LOGDEBUG("DEBUGENABLE: %d\n", DEBUGENABLE);
    
    // NEED ERROR HANDLING!
    getuid();
#endif 



    // Install signal handlers
    // NEED ERROR HANDLING!
    AlpacaCore_installSigHandlers();

    AlpacaComms_init(tls_1_2);

    LEAVING;
    return result;
}


/*
 * Small for now. Called by sighandler
 * will get expandaed on
 */
void AlpacaCore_exit(ALPACA_STATUS* code){
    
    *code = AlpacaComms_cleanUp();
    if(*code){
        LOGERROR("Error cleaning process comms\n");
    }

     
}