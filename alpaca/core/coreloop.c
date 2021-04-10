#include <sys/poll.h>

#include <interfaces/comms_interface.h>
#include <core/codes.h>
#include <core/logging.h>

#define MILLI_FIVE_SEC (1000*5) // Five seconds in milliseconds

//static int debug_counter;  

ALPACA_STATUS handle_payload(void){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ssize_t numRecvd = 0;
    //size_t numWant = 0;
    Alpaca_protoHdr_t header =  {0};
    uint8_t body[COMMS_MAX_BODY];

    memset(body, 0, sizeof(body));
    ENTRY;

    result = AlpacaComms_recv(coreComms, &header, COMMS_HDR_SIZE, &numRecvd);
    if(0 < numRecvd){
        /* TODO Process header
         * progress state machine
         */

        /* Convert all members (that require it)
         * from network to host byte order.
         * This macro is in release 
         */
        NTOH_COMMS_HDR(header);

        // DEBUG ONLY
        DEBUG_COMMS_HDR(header);

        if(header.bodySize > COMMS_MAX_BODY){
            LOGERROR("header.bodySize exceeds max size! [%u]", header.bodySize);
            result = ALPACA_ERROR_HDRBODYSIZE;
            goto exit;
        }

        /*
         * Header has been read, now we 
         * know the size we are expecting and
         * can read of remaining from network stack
         */
        result = AlpacaComms_recv(coreComms, &body, header.bodySize, &numRecvd);
        if(0 < numRecvd){
            LOGDEBUG("Received: {%s}\n", body);
            //LOGDEBUG("Counter: %d\n", ++debug_counter);


            // TODO 
            // Process whole payload

        }
        else if(0 > numRecvd) {
            /*
            * Error occured
            */
           LOGERROR("Failure occured during body recv\n");
        }
        else {
            /*
            * Socket closed
            */
           LOGERROR("Peer closed during body recv\n");
        }

    }
    else if(0 > numRecvd) {
        /*
         * Error occured
         */
    }
    else {
        /*
         * Socket closed
         */
    }


exit:
    LEAVING;
    return result;
}









ALPACA_STATUS AlpacaCore_coreLoop(void){

	ALPACA_STATUS result = ALPACA_SUCCESS;
	int32_t ret = 0;
    
    struct pollfd pfd;
    
    ENTRY;

    /* 
     * We only care about inbound data from the 
     * controller. we also are concerned about
     * loss of connection  
     */
    DEBUG_COMMS_CTX(coreComms);

    pfd.fd = coreComms->fd;
    pfd.events = POLLIN;


    while(1) 
    {
        pfd.revents = 0;
        /* TODO
         * Amount to memset will likely need change after payload
         * processing is decided upon
         */
        
        LOGDEBUG("Polling on controller fd...\n");
        ret = poll(&pfd, 1, MILLI_FIVE_SEC*5);
        if(ret > 0) 
        {
            /* Data available for read */
            if(POLLIN == pfd.revents) 
            {
                /* ** TO DO ********************************************************
                 * Since the header size is deterministic should read off header
                 * from there introspection into the header will provide 
                 * remaining length to readoff
                 */
                result = handle_payload();
                if(ALPACA_ERROR_COMMSCLOSED == result) 
                {
                    // TODO

                    // HANDLE RECONNECTION
                    break;
                }
                else if (ALPACA_SUCCESS != result) 
                {
                    // TODO 
                    // HANDLE ERROR
                    break;
                }
                else 
                {
                    continue;
                }
            }
            else if(POLLHUP == pfd.revents ||
                    POLLERR == pfd.revents || 
                    POLLNVAL == pfd.revents) 
            {
                /*
                 * FIN or RST recieved
                 * This should never be expected 
                 * attempt to re-establish connection to controller
                 */
                
                // This strangely never seems to be hit...
                LOGERROR("Lost connection to controller...\n");
                goto exit;
            }
        }
        else if(0 == ret)
        {
            LOGDEBUG("Coreloop poll timeout\n");
        }
        else 
        {
            /* 
             * 0 > ret
             * An error occured 
             */ 
            LOGERROR("Error in coreloop pool errno[%d]\n", errno);
            break;
        }

        /*
         * Either a timeout occured (ret == 0) or data was 
         * read off the socket. Either way, time to 
         * check for any outbound data
         */

        // TO DO - CHECK FOR OUTBOUND DATA AND SEND
        // Need outbound data queue 
        // For now just send something
    }

exit:
    return result;
}