/**/


// Implements interface 
#include <interfaces/comms_interface.h>


// Internal
#include <interfaces/memory_interface.h>
#include <core/logging.h>
#include <core/codes.h>
#include <comms/wolf.h>

#define DEFAULTPORT 12345

static
int32_t __attribute__ ((unused))setNonblocking(int fd)
{
    int flags = -1;

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

/**
 *	@brief
 *  
 *  @param
 *	@return
 */
ALPACA_STATUS AlpacaComms_init(Alpaca_tlsVersion_t v){
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;

	result = AlpacaWolf_init(v);
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error during TLS layer init Version:%d\n", v);
	}

	LEAVING;
	return result;
}

ALPACA_STATUS AlpacaComms_cleanUp (void){
	ALPACA_STATUS result = ALPACA_SUCCESS;
	ENTRY;

	result = AlpacaWolf_cleanUp();
	if(result != ALPACA_SUCCESS){
		LOGERROR("Error during comms cleanup\n");
	}

	LEAVING;
	return result;
}
