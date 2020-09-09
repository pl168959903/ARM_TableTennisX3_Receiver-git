#ifndef _DRIVER_PRESSURE_
#define _DRIVER_PRESSURE_

#define DRIVER_PRESSURE_API_VERSION DRIVER_VERSION_MAIJOR_MINOR( 1, 00 )

#define _DRV_PRESSURE_CHANNELS_NUM 8
#define _DRV_PRESSURE_ADC          self->Params.adc

#include "Driver_ADC.h"

typedef struct _DRV_PRESSURE_PARAMS {
    DRV_ADC *adc;
} DRV_PRESSURE_PARAMS;

typedef struct _DRV_PRESSURE_CH {
    uint8_t  enabled : 1;
    uint8_t  wavePosNeg : 1;
    uint8_t  isCapturing : 1;
    uint16_t schmittTriggerUpper;
    uint16_t schmittTriggerLower;
    uint8_t  adcChannel;

    struct {
        uint32_t sampleSize;
        uint16_t extremum;
    } Value;
} DRV_PRESSURE_CH;

typedef struct _DRV_PRESSURE {

    struct {
        DRV_ADC *adc;
    } Params;

    DRV_PRESSURE_CH channels[ _DRV_PRESSURE_CHANNELS_NUM ];

    void ( *Update )( struct _DRV_PRESSURE *self );
    void ( *SetChannel )( struct _DRV_PRESSURE *self, uint8_t pressureChannel, uint8_t adcChannel, uint8_t wavePosNeg, uint16_t smtUp, uint16_t smtLo );

    void ( *Free )( struct _DRV_PRESSURE *self );

} DRV_PRESSURE;

DRV_PRESSURE *DRV_PressureNew( DRV_PRESSURE_PARAMS Params );

#endif
