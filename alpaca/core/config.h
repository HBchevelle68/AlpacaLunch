#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>

#include <core/codes.h>

#define HDRFTRSIZE 16
//__attribute__((packed))
typedef struct __attribute__((packed)) AlpacaLunch_Configuration {

    uint8_t  MAGIC_HEADER[HDRFTRSIZE];


    uint8_t  initial_behavior;
    
    uint16_t port;
    uint8_t  addr[INET_ADDRSTRLEN];



    uint8_t  MAGIC_FOOTER[HDRFTRSIZE];
} Alpaca_config_t;
#define ALPACA_CONFIG_SIZE (sizeof(struct AlpacaLunch_Configuration))

extern Alpaca_config_t alpaca_config;


ALPACA_STATUS AlpacaConfig_init(void);





#endif