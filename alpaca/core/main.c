// Standard
#include <stdio.h> 
#include <time.h>

// Internal
#include <core/logging.h> 
#include <core/crypto.h>
#include <core/server.h>
#include <core/macros.h>
#include <core/sighandler.h>
#include <core/allu.h>
//#include <multithreadserver/multithreadserver.h>

#include <interfaces/threadpool_interface.h>

/*
 * Test harness
 */
#ifndef SNOW_ENABLED



int main(){

    
    FAIL_IF(alpacacore_init());
    int temp = 10;

    LOGERROR("This is a test: %d\n", temp);
    LOGDEBUG("This is a test: %d\n", temp);
    LOGINFO("This is a test: %d\n", temp);

    ALtpool_t* tpool = NULL;
    tpool = AlpacaThreadpool_init(10);
    if(tpool != NULL){
        LOGDEBUG("Created thread pool of size: %d\n", tpool->t_size);
    }
    else{
        LOGDEBUG("Threadpool is NULL!!!\n");
    }
    
    temp = AlpacaThreadpool_exit(tpool);
    LOGDEBUG("Return from AlpacaThreadpool_exit: %d\n", temp);

    LOGDEBUG("End tests....\n");



    alpacacore_server_run(12345);


    alpacacore_exit();
   
    return 0; 
}

#else
#if __STDC_VERSION__ >= 199901L
   /*
    * C99 or higher
    * 
    * Included for pid_t usage in snow.h 
    */
   #include <unistd.h>
   #include <sys/types.h>
#endif

#include <snow.h>
    snow_main();
    
#endif 

#ifdef SNOW_ENABLED
#include <core/server.h>
#include <core/crypto.h>

#define TEST_SUCCESS 1
#define TEST_FAILURE 0
/*
int checkport(uint16_t testport){
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = BEU16(testport);
    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        if(errno == EADDRINUSE) {
            return TEST_SUCCESS;
        } 
    }
    return TEST_FAILURE;
}

describe(networking) {

    
    
    after_each() {
        alpacacore_server_clean();
	}


}

describe(chacha20) {

    it("Testing that crypto worked?") {

	}


}
*/
#endif