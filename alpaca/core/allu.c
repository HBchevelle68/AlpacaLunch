

// Internal
#include <core/allu.h>
#include <core/macros.h>
#include <core/server.h>
#include <core/logging.h>
#include <core/sighandler.h>


ALPACA_STATUS alpacacore_init(){
    EPILOG;
    ALPACA_STATUS result = ALPACA_SUCCESS;

    DEBUGWARNING();

    // Check for root if in release
    #ifndef DEBUGENABLE
    LOGDEBUG("DEBUGENABLE: %d\n", DEBUGENABLE);
    FAIL_IF_TRUE(getuid());
    #endif 

    // Install signal handlers
    FAIL_IF_TRUE(alpacacore_install_sighandlers());

    PROLOG;
    return result;
}


/*
 * Small for now. Called by sighandler
 * will get expandaed on
 */
void alpacacore_exit(){
    alpacacore_server_clean();
    exit(0);
}