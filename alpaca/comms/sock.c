/**/


// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
//#include <interfaces/memory_interface.h>
#include <comms/sock.h>
#include <comms/wolf.h>
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




ALPACA_STATUS AlpacaSock_create(Alpaca_sock_t* ctx) {

	ALPACA_STATUS result = ALPACA_ERROR_SOCKCREATE;
	ENTRY;

    /*
     * Convert the opaque oparam to an Alpaca Socket
     * then verify its validity 
     */
    if(ctx == NULL){
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
    ctx->fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0); 
    if(ctx->fd == -1) { 
        LOGERROR("Failure to create socket\n");
        result = ALPACA_ERROR_SOCKCREATE;
        goto done;
    } 
       
    /*
     * Set type accordingly
     * Set ssl ptr and peer struct to NULL/0
     */
    ctx->type = SOCK_STREAM;
    ctx->ssl  = NULL;
    memset(&ctx->peer, 0, sizeof(struct sockaddr_in));

    // Update return code
    result = ALPACA_SUCCESS;

done:
    LEAVING;
    return result;
}


ALPACA_STATUS AlpacaSock_setPeer(Alpaca_sock_t* alpacasock, char* ipstr, uint16_t port){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    /* 
     * Initialize the server address struct with zeros 
     */
    memset(&(alpacasock->peer), 0, sizeof(struct sockaddr_in));

    /*
     * Fill in the server address
     */
    alpacasock->peer.sin_family = AF_INET;
    alpacasock->peer.sin_port   = htons(port);

    /* Get the server IPv4 address from the command line call */
    if (inet_pton(AF_INET, ipstr, &alpacasock->peer.sin_addr) != 1) {
        LOGERROR("Error invalid address\n");
        result = ALPACA_ERROR_SOCKSETPEER;
    }

    LEAVING;
    return result;
}


ALPACA_STATUS AlpacaSock_close(Alpaca_sock_t* alpacasock){

    ALPACA_STATUS result = ALPACA_SUCCESS;
    ENTRY;

    /*
     * Convert the opaque oparam to an Alpaca Socket
     * then verify its validity 
     */
    if(!alpacasock) {
        LOGERROR("Invalid param\n");
        result = ALPACA_ERROR_BADPARAM;
        goto exit;
    }
    
    if(alpacasock->ssl){
        LOGERROR("Underlying comms layer still valid. Close security layer first");
        result = ALPACA_ERROR_BADSTATE;
        goto exit;            
    }

    if(alpacasock->fd > 0){
        close(alpacasock->fd);
        alpacasock->fd = -1;
    }
    
    alpacasock->type = 0;
    memset(&alpacasock->peer, 0, sizeof(struct sockaddr_in));

exit:
    LEAVING;
    return result;

}