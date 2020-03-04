#define _GNU_SOURCE // Temp for test
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

// Interfaces 
#include <interfaces/threadpool_interface.h>
#include <interfaces/memory_interface.h>
#include <interfaces/comms_interface.h>

/*
 * Test harness
 */
#ifndef SNOW_ENABLED

char HELLOWORLD[] = "HELLO WORLD!";
ALLU_hinfo* my_hinfo = NULL;

void unameTest(void){
    struct ifaddrs *ifa;
    int s;
    char host[NI_MAXHOST];
    char *IPbuffer;
    my_hinfo = malloc(sizeof(ALLU_hinfo) * sizeof(uint8_t));
    AlpacaNetUtils_getUname(my_hinfo);

    printf("system name = %s\n", my_hinfo->host_uname.sysname);
    printf("node name   = %s\n", my_hinfo->host_uname.nodename);
    printf("release     = %s\n", my_hinfo->host_uname.release);
    printf("version     = %s\n", my_hinfo->host_uname.version);
    printf("machine     = %s\n", my_hinfo->host_uname.machine);

    #ifdef _GNU_SOURCE
       printf("domain name = %s\n", my_hinfo->host_uname.domainname);
    #endif

    AlpacaNetUtils_getHostEntry(my_hinfo);
       // To convert an Internet network 
    // address into ASCII string 

    IPbuffer = inet_ntoa(*(struct in_addr*)(my_hinfo->host_entry->h_addr_list[0])); 
   
    printf("Host IP: %s\n", IPbuffer); 

    AlpacaNetUtils_getIfAddrs(&my_hinfo);
    
    for (ifa = my_hinfo->interfaces; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;  

        s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if((ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            printf("\tInterface : <%s>\n",ifa->ifa_name );
            printf("\t  Address : <%s> %lu\n", host, strlen(host)); 
        }
    }

    freeifaddrs(my_hinfo->interfaces);

    if(my_hinfo){
        free(my_hinfo);
    }
}


void memtest(void){

    ALLU_Buffer_t *testBuff  = NULL;

    // Test init
    testBuff = AlpacaBuffer_init(4000);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", testBuff, testBuff->size, testBuff->index);
    
    // Test ensureRoom and resize (GROW)
    AlpacaBuffer_ensureRoom(&testBuff, 1024*10);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", testBuff, testBuff->size, testBuff->index);
    AlpacaMemory_dumpHex(testBuff->buffer, testBuff->index);

    // Test append
    for(int i = 0; i<10; i++){
        AlpacaBuffer_append(&testBuff, (uint8_t*)HELLOWORLD, 13);
    }
    AlpacaMemory_dumpHex(testBuff->buffer, testBuff->index);


    // Test zero
    AlpacaBuffer_zero(&testBuff);
    AlpacaMemory_dumpHex(testBuff->buffer, testBuff->index);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", testBuff, testBuff->size, testBuff->index);

    // Test resize (SHRINK)
    testBuff = AlpacaBuffer_resize(&testBuff, 1024);
    AlpacaMemory_dumpHex(testBuff->buffer, testBuff->index);
    LOGDEBUG("Buffer [%p] Size [%lu] Used [%lu]\n", testBuff, testBuff->size, testBuff->index);

    // Test free
    AlpacaBuffer_free(&testBuff);
    LOGDEBUG("Buffer [%p]\n", testBuff);
   
    while(1){}
}


void printTypeSizes(void){
        // sizeof evaluates the size of a variable
    printf("**** BUILT-IN TYPES ****\n");
    printf("Size of int: %ld bytes\n", sizeof(int));
    printf("Size of int*: %ld bytes\n", sizeof(int*));
    printf("Size of int**: %ld bytes\n", sizeof(int**));
    printf("Size of long int: %ld bytes\n", sizeof(long int));

    printf("Size of float: %ld bytes\n", sizeof(float));
    printf("Size of double: %ld bytes\n", sizeof(double));
    printf("Size of char: %ld byte\n", sizeof(char));
    printf("Size of char*: %ld byte\n", sizeof(char*));

    printf("**** STRUCT TYPES ****\n");
    printf("Size of struct ifaddrs: %ld bytes\n", sizeof(struct ifaddrs));
    printf("Size of struct hostent: %ld bytes\n", sizeof(struct hostent));
    printf("Size of struct utsname: %ld bytes\n", sizeof(struct utsname));
    

    printf("**** ALPACA TYPES ****\n");
    printf("Size of ALLU_Buffer_t: %ld bytes\n", sizeof(ALLU_Buffer_t));
    printf("Size of ALLU_Buffer_t*: %ld bytes\n", sizeof(ALLU_Buffer_t*));
    printf("Size of ALLU_hinfo: %ld bytes\n", sizeof(ALLU_hinfo));
    printf("Size of ALLU_hinfo*: %ld bytes\n", sizeof(ALLU_hinfo*));



}


int testTemp = 0;

int main(){
    EPILOG;
    #ifndef DEBUGENABLE
    AlpacaUtilities_daemonize();
    #endif
    unameTest();
    printTypeSizes();
    FAIL_IF_TRUE(alpacacore_init());
    

    LOGERROR("This is a test: %d\n", testTemp);
    LOGDEBUG("This is a test: %d\n", testTemp);
    LOGINFO("This is a test: %d\n", testTemp);

    // START Threadpool Test

    ALtpool_t* tpool = NULL;
    tpool = AlpacaThreadpool_init(10);
    if(tpool != NULL){
        LOGDEBUG("Created thread pool of size: %d\n", tpool->t_size);
    }
    else{
        LOGDEBUG("Threadpool is NULL!!!\n");
    }
    
    testTemp = AlpacaThreadpool_exit(tpool);
    LOGDEBUG("Return from AlpacaThreadpool_exit: %d\n", testTemp);

    LOGDEBUG("End tests....\n");

    memtest();

    /* TO DO
     * 
     * Needs some slight rework, core loop, should never really
     * return. If it does should examine the cause.
     * currently the way the loop is built there isn't a good reason to
     */
    //alpacacore_server_run(12345, 20);


    


    alpacacore_exit();
   
    return 0; 
}


/*
 * Ignoring for now, will likely return to later
 */ 

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