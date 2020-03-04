

// Internal
#include <core/allu.h>
#include <core/macros.h>
#include <core/server.h>
#include <core/logging.h>
#include <core/sighandler.h>


ALPACA_STATUS AlpacaCore_init(){
    EPILOG;
    ALPACA_STATUS result = ALPACA_SUCCESS;

    DEBUGWARNING();

    // Check for root if in release
    #ifndef DEBUGENABLE
    LOGDEBUG("DEBUGENABLE: %d\n", DEBUGENABLE);
    FAIL_IF_TRUE(getuid());
    #endif 

    // Install signal handlers
    FAIL_IF_TRUE(AlpacaCore_installSigHandlers());

    PROLOG;
    return result;
}


/*
 * Small for now. Called by sighandler
 * will get expandaed on
 */
void AlpacaCore_exit(){
    AlpacaCore_serverCleanST();
    exit(0);
}