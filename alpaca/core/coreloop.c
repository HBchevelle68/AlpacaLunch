#include <sys/poll.h>

#include <interfaces/comms_interface.h>
#include <core/codes.h>
#include <core/logging.h>

#define MILLI_ONE_SEC (1000) // one second in milliseconds


/* 
 * Process level comms connected to controller 
 */
extern Alpaca_commsCtx_t *coreComms;


ALPACA_STATUS handle_payload(uint8_t* data){
    ALPACA_STATUS result = ALPACA_SUCCESS;
    ssize_t numRecvd = 0;
    ENTRY;

    result = AlpacaComms_recv(coreComms, &data, COMMS_HDR_SIZE, &numRecvd);
    if(0 < numRecvd){
        /* TODO Process header
         * progress state machine
         */
    }

//exit:
    LEAVING;
    return result;
}









ALPACA_STATUS AlpacaCore_coreLoop(void){

	ALPACA_STATUS result = ALPACA_SUCCESS;
	int32_t ret = 0;
    
    struct pollfd pfd;
    uint8_t data[ALPACACOMMS_MAX_BUFF] = {0};
    ENTRY;

    /* 
     * We only care about inbound data from the 
     * controller. we also are concerned about
     * loss of connection  
     */
    //DEBUG_COMMS_CTX(coreComms);

    pfd.fd = coreComms->fd;
    pfd.events = POLLIN | POLLPRI;
    LOGDEBUG("Polling on controller fd...\n");
    while(1) {
        pfd.revents = 0;
        /* TODO
         * Amount to memset will likely need change after payload
         * processing is decided upon
         */
        memset(data, 0, sizeof(data));

        ret = poll(&pfd, 1, MILLI_ONE_SEC*5);
        if(ret > 0) {
            /* Data available for read */
            if(POLLIN == pfd.revents) {
                /* ** TO DO ********************************************************
                 * Since the header size is deterministic should read off header
                 * from there introspection into the header will provide 
                 * remaining length to readoff
                 */
                result = handle_payload(data);
                if(ALPACA_ERROR_COMMSCLOSED == result) {
                    // TODO

                    // HANDLE RECONNECTION
                    break;
                }
                else if (ALPACA_SUCCESS != result) {
                    // TODO 
                    // HANDLE ERROR
                    break;
                }
                else {
                    continue;
                }
            }
            else if (POLLHUP == pfd.revents) {
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
        else if(0 > ret) {
            /* An error occured */ 
            LOGERROR("Error in coreloop pool errno[%d]\n", errno);
        }
        else if(0 == ret)

        LOGDEBUG("Coreloop poll timeout\n");
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