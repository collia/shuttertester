/* This file contains declaration and implementation ring buffer */

#define RING_BUFFER_DECL(name, size) \
    uint8_t name##Buffer[size]; \
    uint32_t name##BufPtrIn = 0;\
    uint32_t name##BufPtrOut = 0;

#define RING_BUFFER(name) name##Buffer
#define RING_BUFFER_IN_PTR(name) name##BufPtrIn
#define RING_BUFFER_OUT_PTR(name) name##BufPtrOut

#define RING_BUFFER_RESET(name) \
    name##BufPtrIn = 0;          \
    name##BufPtrOut = 0;


#define RING_BUFFER_INSERT(name, data, len) \
    {\
    uint8_t *__buffptr;\
    uint32_t __buffsize;\
    __buffptr = name##Buffer + name##BufPtrIn;             \
    if (name##BufPtrOut > name##BufPtrIn) {              \
        __buffsize = name##BufPtrOut - name##BufPtrIn ;     \
        if(len > __buffsize) {                               \
            memcpy(__buffptr, data, __buffsize);               \
            name##BufPtrIn +=__buffsize;                      \
        } else {                                           \
            memcpy(__buffptr, data, len);                    \
            name##BufPtrIn += len;                         \
        }                                                 \
    } else {                                                       \
        if (name##BufPtrIn + len >= sizeof(name##Buffer)) {           \
            __buffsize =  sizeof(name##Buffer) - name##BufPtrIn ;         \
            memcpy(__buffptr, data, __buffsize);                        \
            name##BufPtrIn = 0;                                          \
            if(len -  __buffsize > name##BufPtrOut) {                    \
                memcpy(name##Buffer, data + __buffsize, name##BufPtrOut); \
                name##BufPtrIn =__buffsize;                              \
            } else if ( name##BufPtrIn !=  name##BufPtrOut) {              \
                memcpy(name##Buffer,                                     \
                       data + __buffsize,                               \
                       len - __buffsize);                               \
                name##BufPtrIn = len - __buffsize;                       \
            }                                                           \
        } else {                                                        \
            memcpy(__buffptr, data, len);                             \
            name##BufPtrIn += len;                                   \
        }                                                           \
    }                                                               \
    }


#define RING_BUFFER_GET(name, dataptr, len) {                       \
        uint32_t __aval_len;                                        \
        if (name##BufPtrOut != name##BufPtrIn) {                       \
            if (name##BufPtrOut > name##BufPtrIn){                    \
                __aval_len = sizeof(name##Buffer) - name##BufPtrOut;  \
                dataptr = &(name##Buffer[name##BufPtrOut]);           \
                if(__aval_len <= len) {                              \
                    len = __aval_len;                               \
                    name##BufPtrOut = 0;                             \
                } else {                                            \
                    name##BufPtrOut += len;                          \
                }                                                   \
            } else {                                                \
                __aval_len = name##BufPtrIn - name##BufPtrOut;        \
                dataptr = &(name##Buffer[name##BufPtrOut]);           \
                if(__aval_len <= len) {                              \
                    len = __aval_len;                               \
                    name##BufPtrOut = name##BufPtrIn;                 \
                } else {                                            \
                    name##BufPtrOut += len;                         \
                }                                                  \
            }                                                 \
        } else {                                              \
            len = 0;                                          \
        }                                                     \
    }
