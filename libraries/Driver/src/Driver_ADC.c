
#define DRIVER_ADC_DRV_VERSION DRIVER_VERSION_MAIJOR_MINOR( 1, 00 )

#include "Driver_ADC.h"
#include "TableTennisX3.h"

#define _ADC_BASE ( ( ADC_T * )( self->Params.adc ) )

static void ChannelEnable( DRV_ADC *self, uint16_t channel, uint8_t extCatch ) {
    ADC_SetExtraSampleTime( _ADC_BASE, channel, extCatch );
    _ADC_BASE->CHEN |= ( 0x1 << channel );
}
static void ChannelDisable( DRV_ADC *self, uint16_t channel ) {
    _ADC_BASE->CHEN &= ~( 0x1 << channel );
}

static uint16_t Read( DRV_ADC *self, uint8_t channel ) {
    uint16_t value = ADC_GET_CONVERSION_DATA( _ADC_BASE, channel );
    return value;
}

static void Initialize( DRV_ADC *self, DRV_ADC_PARAMS initParams ) {
    self->Params.adc = initParams.adc;
    ADC_SET_RESOLUTION( _ADC_BASE, initParams.resolution );
    ADC_SET_REF_VOLTAGE( _ADC_BASE, initParams.refSource );
    ADC_EnableInt( _ADC_BASE, ADC_ADF_INT );
    ADC_POWER_ON( _ADC_BASE );
    ADC_Open( _ADC_BASE, initParams.dataMode, initParams.catchMode, 0 );
}

void Conversion( DRV_ADC *self ) {
    ADC_START_CONV( _ADC_BASE );
}

static void Event_AdcConverted( DRV_ADC *self ) {}

static void Free( DRV_ADC *self ) {
    _DRV_FREE( self );
}

DRV_ADC *DRV_AdcNew( void ) {
    DRV_ADC *self = _DRV_MACCLOC( sizeof( DRV_ADC ) );

    _FUNCTION_POINTER_LINK( Initialize );
    _FUNCTION_POINTER_LINK( ChannelEnable );
    _FUNCTION_POINTER_LINK( ChannelDisable );
    _FUNCTION_POINTER_LINK( Conversion );
    _FUNCTION_POINTER_LINK( Read );

    _FUNCTION_POINTER_LINK( Event_AdcConverted );

    _FUNCTION_POINTER_LINK(Free);

    return self;
}


