/*
 * AlpacaLunch Memory Interface 
 *
 */
#ifndef MEMORY_INTERFACE_H
#define MEMORY_INTERFACE_H
#include <stddef.h>
#include <stdint.h>

extern void* AlpacaMemory_zero(void* buf, size_t bufsize, size_t len);

#endif