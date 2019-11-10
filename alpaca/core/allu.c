

// Internal
#include <allu.h>
#include <macros.h>
#include <server.h>
#include <logging.h>
#include <sighandler.h>


ALPACA_STATUS alpacacore_init(){
    ALPACA_STATUS result = ALPACA_SUCCESS;

    
    FAIL_IF(getuid());
    
    DEBUGWARNING(); 
    FAIL_IF(alpacacore_install_sighandlers());

    return result;
}


/*
 * Small for now. Called by sighandler
 */
void alpacacore_exit(){
    alpacacore_server_clean();
    exit(0);
}