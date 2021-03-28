#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdint.h>


//__attribute__((packed))
typedef struct __attribute__((packed)) AlpacaLunch_Configuration {

    uint8_t  initial_behavior;
    uint16_t port;
    uint32_t addr;

    //uint8_t max_threads;

} Alpaca_config_t;

extern Alpaca_config_t alpaca_config;

#define ALPACA_CONFIG_SIZE (sizeof(struct AlpacaLunch_Configuration))

#endif