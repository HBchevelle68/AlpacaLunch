

// Internal
#include <ns.h>
#include <macros.h>
#include <server.h>
#include <logging.h>
#include <sighandler.h>


NS_STATUS NS_init(){
    NS_STATUS result = NS_SUCCESS;

    
    FAIL_IF(getuid());
    
    DEBUGWARNING(); 
    FAIL_IF(NS_install_sighandlers());

    return result;
}


/*
 * Small for now. Called by sighandler
 */
void NS_exit(){
    NS_server_clean();
    exit(0);
}