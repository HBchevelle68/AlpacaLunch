#include <stdlib.h>
// Internal
#include <core/allu.h>
#include <core/logging.h>
#include <core/sighandler.h>


ALPACA_STATUS AlpacaCore_init(){
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

    LEAVING;
    return result;
}


/*
 * Small for now. Called by sighandler
 * will get expandaed on
 */
void AlpacaCore_exit(){
    
    exit(0);
}