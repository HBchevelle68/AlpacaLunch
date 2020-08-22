/**/


// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
//#include <interfaces/memory_interface.h>
#include <comms/sock.h>
#include <core/logging.h>
#include <core/codes.h>


#define DEFAULTPORT 12345


int32_t AlpacaSock_setNonBlocking(int fd) {
    int flags;

    /* If system have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
} 




ALPACA_STATUS AlpacaSock_createSocket(void** ctx){

	ALPACA_STATUS result = ALPACA_ERROR_SOCKCREATE;
    
	ENTRY;

    /*
     * Convert the opaque oparam to an Alpaca Socket
     * then verify its validity 
     */
	Alpaca_sock_t* sockPtr = (Alpaca_sock_t*)(*ctx);
    if((*ctx) == NULL){
        LOGERROR("Invalid param\n");
        result = ALPACA_ERROR_BADPARAM;
        goto done;
    }
  
    /*
     * Create socket and verify
     * 
     * Socket created is set to non-blocking as well as setting the
     * Close on Exec (CLOEXEC) flag to prevent file descriptor
     * leaking into child processes 
     */
    sockPtr->fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0); 
    if(sockPtr->fd == -1) { 
        LOGERROR("Failure to create socket\n");
        result = ALPACA_ERROR_SOCKCREATE;
        goto done;
    } 
       
    /*
     * Set type accordingly
     * Set ssl ptr and peer struct to NULL/0
     */
    sockPtr->type = SOCK_STREAM;
    sockPtr->ssl  = NULL;
    memset(&sockPtr->peer, 0, sizeof(struct sockaddr_in));

    // Update return code
    result = ALPACA_SUCCESS;

done:
    LEAVING;
    return result;
}

ALPACA_STATUS AlpacaSock_close(void** ctx){

    ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
    Alpaca_sock_t* sockPtr = (Alpaca_sock_t*)(*ctx);
    ENTRY;

    /*
     * Convert the opaque oparam to an Alpaca Socket
     * then verify its validity 
     */
    
    if((*ctx)) {

        if(sockPtr->ssl){
            wolfSSL_free(sockPtr->ssl);
        }

        if(sockPtr->fd > 0){
            close(sockPtr->fd);
            sockPtr->fd = -1;
        }
        
        sockPtr->type = 0;
        memset(&sockPtr->peer, 0, sizeof(struct sockaddr_in));
        result = ALPACA_SUCCESS;
    }
    else {
        LOGERROR("Invalid param\n");
        result = ALPACA_ERROR_BADPARAM;
    }
    
    LEAVING;
    return result;

}