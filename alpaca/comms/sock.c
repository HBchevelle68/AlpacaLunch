/**/


// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
#include <interfaces/memory_interface.h>
#include <core/logging.h>
#include <core/codes.h>


#define DEFAULTPORT 12345

static
int32_t __attribute__ ((unused))setNonblocking(int fd)
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




ALPACA_STATUS AlpacaSock_createSocket(void* a){

	ALPACA_STATUS result = ALPACA_ERROR_UNKNOWN;
	/*
	int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        
        exit(0); 
    } 
    else{

    }
       
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
    }
	*/
    return result;
}
