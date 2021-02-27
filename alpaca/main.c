//#define _GNU_SOURCE // Temp for test
// Standard
#include <stdio.h> 
#include <time.h>

// Internal
#include <core/logging.h> 

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
    result = AlpacaCore_init( ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    if(result != ALPACA_SUCCESS){
        LOGERROR("Global initialization error! [%u]\n", result);
        goto done;
    }

    result = AlpacaComms_initCtx(&coreComms, ALPACACOMMS_PROTO_TLS12 | ALPACACOMMS_TYPE_CLIENT);
    if(result != ALPACA_SUCCESS){
        LOGERROR("Global comms ctx error! [%u]\n", result);
        goto done;
    }
    

    // FOR TEST ONLY! Doesn't belong here
    result = AlpacaComms_connect(coreComms, "127.0.0.1" ,44444);
    
    memset(buffer, 0, 1024);
    strcpy(buffer,"WAZZZZZZUP!");
    result = AlpacaComms_send(coreComms, buffer, strlen(buffer), &out);
    memset(buffer, 0, out);
    result = AlpacaComms_recv(coreComms, buffer, 1024, &out);
    LOGDEBUG("Buffer: %s\n", buffer);



    //DevTests();
   
done:
    AlpacaCore_exit();
    LOGINFO("Exiting with exit code:%u\n", result);
    return 0; 
}
