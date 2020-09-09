

#ifndef _V_MEM_ALLOC
#define _V_MEM_ALLOC

#include <stdint.h>
#include <stdio.h>

#define VM_MEM_SIZE 5000
#define VM_ALIGNMENT_SIZE 8

#define __PRINTF printf



#if      (VM_ALIGNMENT_SIZE == 32)
    #define VM_ALIGNMENT_MASK   (0x001F)
#elif    (VM_ALIGNMENT_SIZE == 16)
    #define VM_ALIGNMENT_MASK   (0x000F)
#elif    (VM_ALIGNMENT_SIZE == 8)
    #define VM_ALIGNMENT_MASK   (0x0007)
#elif    (VM_ALIGNMENT_SIZE == 4)
    #define VM_ALIGNMENT_MASK   (0x0003)
#elif    (VM_ALIGNMENT_SIZE == 2)
    #define VM_ALIGNMENT_MASK   (0x0001)
#elif    (VM_ALIGNMENT_SIZE == 1
    #define VM_ALIGNMENT_MASK   (0x0000)
#endif

int vcomPrintf( const char* fmt, ... );

void* vMemAlloc( size_t allocSize );
void vMemFree( void* pv );
size_t vMemSizeOf(void * pr);
void vMemCopy( void* dest, const void* src, size_t len );
void vMemInfoPrint(void);



#endif // _V_MEM_ALLOC
