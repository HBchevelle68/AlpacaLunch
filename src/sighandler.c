#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <sighandler.h>
#include <logging.h>
#include <ns.h>


void NS_signal_handler(int signum){
    LOGDEBUG("RECV'd SIGNAL: %d\n", signum);

    switch (signum) {
    
    case SIGHUP:
    case SIGINT:
    case SIGQUIT:
    case SIGALRM:
        break;
    
    case SIGTERM:
        NS_exit();
        break;
    default:
        
        break;
    }

}

NS_STATUS NS_install_sighandlers(){
    NS_STATUS result = NS_SUCCESS;

    struct sigaction act;
    
    memset(&act, 0, sizeof(struct sigaction));
    
    act.sa_handler = NS_signal_handler;
    sigaction(SIGHUP, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGALRM, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    return result;
}