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

    switch (signum) {
    
    case SIGHUP:
    case SIGQUIT:
    case SIGALRM:
    case SIGINT:
        break;

    case SIGTERM:
        AlpacaCore_exit();
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
    
    //ALPACA_STATUS result = ALPACA_SUCCESS;
    //struct sigaction act;
    //memset(&act, 0, sizeof(struct sigaction));
    
    //act.sa_handler = alpacacore_signal_handler;
    /*
    sigaction(SIGHUP, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGALRM, &act, NULL);
    #ifndef DEBUGENABLE
    sigaction(SIGINT, &act, NULL);
    #endif
    */

    signal(SIGHUP, AlpacaCore_signalHandler);
    signal(SIGTERM,AlpacaCore_signalHandler);
    signal(SIGQUIT,AlpacaCore_signalHandler);
    signal(SIGALRM,AlpacaCore_signalHandler);
    #ifndef DEBUGENABLE
    signal(SIGINT, AlpacaCore_signalHandler;
    #endif
        
    return ALPACA_SUCCESS;
}