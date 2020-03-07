#define _GNU_SOURCE // Will freak out if this isn't here

// Implements interface 
#include <interfaces/comms_interface.h>
#include <core/logging.h>


#include <unistd.h>
#include <sys/utsname.h>
#include <sys/socket.h> 
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/types.h>

/*
 * @brief Initialize a AlpacaNetInfo struct (ALLU_net_info)
 * @return ALLU_net_info pointer to initialized struct.
 * 		   struct will be filled with respective information 
 */
extern ALLU_net_info* AlpacaNetInfo_init(void){
	// Alloc 
	ALLU_net_info* new_netInfo = NULL;
	new_netInfo = (ALLU_net_info *)calloc(sizeof(ALLU_net_info), sizeof(uint8_t));
	if(!new_netInfo){
		LOGERROR("Failed to calloc\n");
		return NULL;
	}

	if (AlpacaNetInfo_getUname(&new_netInfo)){
		AlpacaNetInfo_clean(new_netInfo);
		return NULL;		
	}

	if (AlpacaNetInfo_getHostEntry(new_netInfo) || AlpacaNetInfo_getIfAddrs(&new_netInfo)){
		AlpacaNetInfo_clean(new_netInfo);
		return NULL;
	}


	return new_netInfo;
}

extern void AlpacaNetInfo_clean(ALLU_net_info* allu_ni){

	if(allu_ni){
		if(allu_ni->interfaces){
			freeifaddrs(allu_ni->interfaces);
			allu_ni->interfaces = NULL;
		}

		free(allu_ni);
		allu_ni = NULL;
	}
}
/*
 * @brief grabs uname output for current host system
 * @param allu_ni ALLU_net_info pointer to struct to fill
 * @return ALPACA_STATUS of success or failure  
 */
ALPACA_STATUS AlpacaNetInfo_getUname(ALLU_net_info** allu_ni){
 
   	if (uname(&((*allu_ni)->host_uname)) != 0) {
   		LOGERROR("Error getting uname\n");
   		return ALPACA_FAILURE;
   	}
   return ALPACA_SUCCESS;
}

/*
 * @brief grabs host entry output for current host system
 * @param allu_ni ALLU_net_info pointer to struct to fill
 * @return ALPACA_STATUS of success or failure  
 */
ALPACA_STATUS AlpacaNetInfo_getHostEntry(ALLU_net_info* allu_ni){
 
    // To retrieve host information 
    if(!(allu_ni->host_entry = gethostbyname(allu_ni->host_uname.nodename))){
    	LOGERROR("Error getting Host Entry\n");
    	return ALPACA_FAILURE; 
    }
  	return ALPACA_SUCCESS; 
}

/*
 * @brief grabs all interfaces and respective ip addrs 
 *		  for current host system
 * @param allu_ni ALLU_net_info pointer to struct to fill
 * @return ALPACA_STATUS of success or failure  
 */
ALPACA_STATUS AlpacaNetInfo_getIfAddrs(ALLU_net_info** allu_ni){

    if (getifaddrs(&((*allu_ni)->interfaces)) == -1) {
        LOGERROR("Error getting interfaces");
        return ALPACA_FAILURE;
    }


	return ALPACA_SUCCESS;
}