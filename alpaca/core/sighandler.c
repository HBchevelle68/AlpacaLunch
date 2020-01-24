#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// Internal
#include <core/sighandler.h>
#include <core/logging.h>
#include <core/allu.h>


void alpacacore_signal_handler(int signum){
    LOGDEBUG("RECV'd SIGNAL: %d\n", signum);

    switch (signum) {
    
    case SIGHUP:
    case SIGQUIT:
    case SIGALRM:
    case SIGINT:
        break;

    case SIGTERM:
        alpacacore_exit();
        break;
    default:
        
        break;
    }

}

/*
 * ToDO
 * Explicitly catch all catchable signals, handle appropriately 
 */
ALPACA_STATUS alpacacore_install_sighandlers(){
    
    ALPACA_STATUS result = ALPACA_SUCCESS;
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    
    act.sa_handler = alpacacore_signal_handler;
    sigaction(SIGHUP, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGALRM, &act, NULL);
    #ifndef DEBUGENABLE
    sigaction(SIGTERM, &act, NULL);
    #endif

    return result;
}