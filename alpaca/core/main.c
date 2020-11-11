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



int main(){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;


    AlpacaUtilities_daemonize();

    

    result = AlpacaCore_init();
    if(result){
        LOGERROR("Initialization error code: %u\n", result);
        goto done;
    }

    

    //DevTests();
   
done:
    AlpacaCore_exit();
    LOGINFO("Exiting with exit code:%u\n", result);
    return 0; 
}
