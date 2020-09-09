#ifndef _DRIVER_ADC_
#define _DRIVER_ADC_

#define DRIVER_ADC_API_VERSION DRIVER_VERSION_MAIJOR_MINOR( 1, 00 )
#define DRV_ADC_CHANNEL_NUM 18

#include "Driver_MCU.h"

typedef struct _DRV_ADC_PARAMS {
    void *   adc;
    uint32_t resolution;
    uint32_t refSource;
    uint32_t dataMode;
    uint32_t catchMode;
} DRV_ADC_PARAMS;

typedef struct _DRV_ADC {
    struct {
        uint32_t *adc;
    } Params;

    void ( *Initialize )( struct _DRV_ADC *self, DRV_ADC_PARAMS initParams );
    void ( *ChannelEnable )( struct _DRV_ADC *self, uint16_t channel, uint8_t extCatch );
    void ( *ChannelDisable )( struct _DRV_ADC *self, uint16_t channel );
    void ( *Conversion )( struct _DRV_ADC *self );
    uint16_t ( *Read )( struct _DRV_ADC *self, uint8_t channel );

    void ( *Event_AdcConverted )( struct _DRV_ADC *self );

    void ( *Free )( struct _DRV_ADC *self );

} DRV_ADC;

DRV_ADC *DRV_AdcNew( void );
#include "Driver_MCU.h"

#endif

