#define _GNU_SOURCE // Temp for test
// Standard
#include <stdio.h> 
#include <time.h>

// Internal
#include <core/logging.h> 
#include <core/crypto.h>
#include <core/sighandler.h>
#include <core/allu.h>

// Interfaces 
#include <interfaces/threadpool_interface.h>
#include <interfaces/memory_interface.h>
#include <interfaces/comms_interface.h>
#include <interfaces/utility_interface.h>

/*
 * Comms context leading back to home
 * See comms/comms.c
 */
extern Alpaca_commsCtx_t *coreComms;

int main(int argc, char** argv){
    
    ENTRY;
    ALPACA_STATUS result = ALPACA_SUCCESS;
    char buffer[1024] = {0};
    ssize_t out = 0;


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
    LOGERROR("here\n");
    result = AlpacaCore_init( ALPACA_COMMSTYPE_TLS12 | ALPACA_COMMSTYPE_CLIENT);
    if(result != ALPACA_SUCCESS){
        LOGERROR("Global initialization error! [%u]\n", result);
        goto done;
    }
    

    // FOR TEST ONLY! Doesn't belong here
    result = AlpacaComms_connect(&coreComms, "127.0.0.1", 44444);
    
    memset(buffer, 0, 1024);
    strcpy(buffer,"WAZZZZZZUP!");
    result = AlpacaComms_write(&coreComms, buffer, strlen(buffer), &out);
    
    result = AlpacaComms_read(&coreComms, buffer, 1024, &out);
    LOGDEBUG("Buffer: %s\n", buffer);



    //DevTests();
   
done:
    AlpacaCore_exit();
    LOGINFO("Exiting with exit code:%u\n", result);
    return 0; 
}
