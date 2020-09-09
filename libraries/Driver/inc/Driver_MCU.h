
#ifndef _DRIVER_MCU_
#define _DRIVER_MCU_

#define DRIVER_USE_VMEMALLOC true

#define DRIVER_VERSION_MAIJOR_MINOR( major, minor ) ( ( major << 8 ) | minor )
#define _FUNCTION_POINTER_LINK( func ) self->func = func

#include <stdbool.h>
#include <stdint.h>

#if DRIVER_USE_VMEMALLOC
#include "vMemAlloc.h"
#define _DRV_MACCLOC vMemAlloc
#define _DRV_FREE    vMemFree
#define _DRV_MEMCPY  vMemCopy
#else
#include <stdlib.h>
#define _DRV_MACCLOC malloc
#define _DRV_FREE    free
#define _DRV_MEMCPY  memcpy
#endif

typedef struct _DRIVER_VERSION {
    uint16_t api_version;
    uint16_t drv_version;
} DRIVER_VERSION;

// return codes
#define MCU_DRIVER_OK              0
#define MCU_DRIVER_ERROR           -1
#define MCU_DRIVER_ERROR_BUSY      -2
#define MCU_DRIVER_ERROR_TIMEOUT   -3
#define MCU_DRIVER_ERROR_PARAMETER -4

#endif  // _DRIVER_MCU_
