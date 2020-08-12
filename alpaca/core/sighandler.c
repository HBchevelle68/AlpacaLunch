#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// Internal
#include <core/sighandler.h>
#include <core/logging.h>
#include <core/allu.h>


void AlpacaCore_signalHandler(int signum){
    LOGDEBUG("RECV'd SIGNAL: %d\n", signum);

    ALPACA_STATUS result = ALPACA_SUCCESS; 

    switch (signum) {
    
        case SIGHUP:
        case SIGQUIT:
        case SIGALRM:
        case SIGINT:
            break;

        case SIGTERM:
            AlpacaCore_exit(&result);
            exit(result);
            break;
        default:
            
            break;
    }

}

/*
 * ToDO
 * Explicitly catch all catchable signals, handle appropriately 
 */
ALPACA_STATUS AlpacaCore_installSigHandlers(){
    
    ALPACA_STATUS result = ALPACA_SUCCESS;


    signal(SIGHUP, AlpacaCore_signalHandler);
    signal(SIGTERM,AlpacaCore_signalHandler);
    signal(SIGQUIT,AlpacaCore_signalHandler);
    signal(SIGALRM,AlpacaCore_signalHandler);
    #ifndef DEBUGENABLE
    signal(SIGINT, AlpacaCore_signalHandler;
    #endif
        
    return result;
}