// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
#include <interfaces/memory_interface.h>
#include <core/macros.h>
#include <core/logging.h>

#define DEFAULTPORT 12345


int setNonblocking(int fd)
{
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


ALPACA_STATUS AlpacaComms_createServSock(ALLU_server_ctx** serverCtx){
    
    /*
     * Create underlying socket
     */
    if (((*serverCtx)->serverSock = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        LOGERROR("Socket creation failure\n");
        return ALPACA_FAILURE;
    } 

    /*
     * Fill server structure
     */
    (*serverCtx)->servAddr->sin_family = AF_INET;          
    (*serverCtx)->servAddr->sin_addr.s_addr = INADDR_ANY;  
    (*serverCtx)->servAddr->sin_port = htons(DEFAULTPORT);


    /*
     * Force kernel to let use re-use addr:port tuple
     */
    if(setsockopt((*serverCtx)->serverSock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &(int){1}, sizeof(uint32_t)) != ALPACA_SUCCESS){
        LOGERROR("setsockopt failure\n");
        return ALPACA_COMMS_SOCKERR;
    }

    /*
     * Bind to address space 
     */
    if(bind((*serverCtx)->serverSock, (struct sockaddr*)(*serverCtx)->servAddr, sizeof(struct sockaddr_in)) != ALPACA_SUCCESS){
        LOGERROR("bind failure\n");
        return ALPACA_COMMS_SOCKERR;
    }

    /*
     * Begin to listen for connections
     */
    if(listen((*serverCtx)->serverSock, 10) != ALPACA_SUCCESS){
        LOGERROR("listen failure\n");
        return ALPACA_COMMS_SOCKERR;            
    }

    return ALPACA_SUCCESS;    
}