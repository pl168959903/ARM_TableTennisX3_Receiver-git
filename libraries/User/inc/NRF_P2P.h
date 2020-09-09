
#ifndef _NRF_P2P_
#define _NRF_P2P_

#ifdef __cplusplus
extern "C" {
#endif

#include "NRF24L01.h"
#include <string.h>

#define nrfP2P_USE_VMEMALLOC    1

#if nrfP2P_USE_VMEMALLOC
    #include "vMemAlloc.h"
    #define nrfP2P_MALLOC( size ) vMemAlloc( size )
    #define nrfP2P_FREE( ptr ) vMemFree( ptr )
#else
    #define nrfP2P_MALLOC( size ) malloc( size )
    #define nrfP2P_FREE( ptr ) free( ptr )
#endif  // nrfP2P_USE_VMEMALLOC


typedef struct {
    NRF_T* nrf;
    uint8_t ch;
} NRFP2P_CHANNEL_T;

void              nrfP2P_InitNrf( NRF_T* nrf, uint8_t rfCh );
NRFP2P_CHANNEL_T* nrfP2P_NewChannel( NRF_T* nrf, uint8_t ch, uint8_t* address, bool autoAck, bool dypw );
void              nrfP2P_SetPayloadWide( NRFP2P_CHANNEL_T* nrfCh, uint8_t wide );
void              nrfP2P_EnableTxAutoAck( NRF_T* nrf, uint8_t *destAddress );
void              nrfP2P_EnableTxDypw( NRF_T* nrf );
bool              nrfP2P_SendPacket( NRF_T* nrf, uint8_t* destAddress, uint8_t* array, uint8_t pw );
bool              nrfP2P_ReceivePacket( NRFP2P_CHANNEL_T* ptr, uint8_t* array );
bool              nrfP2P_TxReuse( NRF_T* nrf, uint32_t times );

#ifdef __cplusplus
}
#endif
#endif
