
/* This header contains simple round buffer definition and realization */

#define RING_BUFFER_DECL(name,size)\
    uint8_t name##Buffer[size]; \
    uint32_t name##BufPtrIn = 0; \
    uint32_t name##BufPtrOut = 0;


#define RING_BUFFER(name) name##Buffer
