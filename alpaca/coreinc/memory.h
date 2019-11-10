#ifndef MEMORY_H
#define MEMORY_H




/*
 * Memory 
 * 
 */
#define BUFALLOC(buf, size) \
     if(size > 0) {         \
          buf = (void*)malloc(size * sizeof(unsigned char)); \
          memset(buf, 0, size);                              \
     }

#define BUFFREE(buf)                                        \
     if(buf != NULL) {                                      \
          memset(buf, (unsigned char)0x00, sizeof(*buf));   \
          free(buf);                                        \
          buf = NULL;                                       \
     }             

#define ZEROBUFF(buf,size) memset(buf, 0, (size_t)size)


#endif