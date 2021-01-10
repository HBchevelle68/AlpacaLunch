#define _GNU_SOURCE // Temp for test
// Standard
#include <stdio.h> 
#include <time.h>

// Internal
#include <core/logging.h> 
#include <core/crypto.h>
#include <core/sighandler.h>
#include <core/allu.h>
#include <core/devtests.h>

// Interfaces 
#include <interfaces/threadpool_interface.h>
#include <interfaces/memory_interface.h>
#include <interfaces/comms_interface.h>
#include <interfaces/utility_interface.h>

extern Alpaca_commsCtx_t *coreComms;

int main(){
    
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;

    DevTests();

    /** 
     * Immediately daemonize ourself according to *nix spec
     * See more in daemon(7)
     * 
     * When debug is enabled this is an empty function
     */
    AlpacaUtilities_daemonize();

    /** 
     * When debug is enabled this is an empty function
     */
    result = AlpacaCore_init();
    if(result != ALPACA_SUCCESS){
        LOGERROR("Global initialization error! [%u]\n", result);
        goto done;
    }
    

    // FOR TEST ONLY! Doesn't belong here
    AlpacaComms_connect(&coreComms, "127.0.0.1", 44444);

    //DevTests();
   
done:
    AlpacaCore_exit();
    LOGINFO("Exiting with exit code:%u\n", result);
    return 0; 
}
