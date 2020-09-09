
#define DRIVER_PRESSURE_API_VERSION DRIVER_VERSION_MAIJOR_MINOR( 1, 00 )

#include "Driver_Pressure.h"

static void SetChannel( DRV_PRESSURE *self, uint8_t pressureChannel, uint8_t adcChannel, uint8_t wavePosNeg, uint16_t smtUp, uint16_t smtLo ) {
    self->channels[ pressureChannel ].enabled    = true;
    self->channels[ pressureChannel ].adcChannel = adcChannel;
    self->channels[ pressureChannel ].wavePosNeg = wavePosNeg;
    self->channels[ pressureChannel ].schmittTriggerUpper = smtUp;
    self->channels[ pressureChannel].schmittTriggerLower = smtLo;
}

static void Update( DRV_PRESSURE *self ) {
    uint8_t i;
    for ( i = 0; i < _DRV_PRESSURE_CHANNELS_NUM; i++ ) {
        if ( self->channels[ i ].enabled == true ) {

            uint16_t adcValue = _DRV_PRESSURE_ADC->Read( _DRV_PRESSURE_ADC, self->channels[ i ].adcChannel );
            
            // 正負波類型
            if(self->channels[i].wavePosNeg == 1){
                // 是否正在捕捉
                if(self->channels[i].isCapturing == 1){
                    // 是否達到結束捕捉條件
                    if(adcValue < self->channels[i].schmittTriggerLower){
                        self->channels[i].isCapturing = 0;
                    }
                    else{
                        // 極值更新
                        if( adcValue > self->channels[i].Value.extremum){
                            self->channels[i].Value.extremum = adcValue;
                        }
                        // 樣本數計數
                        self->channels[i].Value.sampleSize++;
                    }
                }
                else{
                    // 是否達到開始捕捉條件
                    if(adcValue > self->channels[i].schmittTriggerUpper){
                        self->channels[i].isCapturing = 1;
                        self->channels[i].Value.extremum = adcValue;
                        self->channels[i].Value.sampleSize = 1;
                    }
                }
            }
            else{
                 if(self->channels[i].isCapturing == 1){
                    // 是否達到結束捕捉條件
                    if(adcValue > self->channels[i].schmittTriggerUpper){
                        self->channels[i].isCapturing = 0;
                    }
                    else{
                        // 極值更新
                        if( adcValue < self->channels[i].Value.extremum){
                            self->channels[i].Value.extremum = adcValue;
                        }
                        // 樣本數計數
                        self->channels[i].Value.sampleSize++;
                    }
                }
                else{
                    // 是否達到開始捕捉條件
                    if(adcValue < self->channels[i].schmittTriggerLower){
                        self->channels[i].isCapturing = 1;
                        self->channels[i].Value.extremum = adcValue;
                        self->channels[i].Value.sampleSize = 1;
                    }
                }
            }
        }
    }
}

static void Free( DRV_PRESSURE *self ) {
    _DRV_FREE( self );
}

DRV_PRESSURE *DRV_PressureNew( DRV_PRESSURE_PARAMS Params ) {
    DRV_PRESSURE *self = _DRV_MACCLOC( sizeof( DRV_PRESSURE ) );

    self->Params.adc = Params.adc;

    _FUNCTION_POINTER_LINK( Update );
    _FUNCTION_POINTER_LINK( SetChannel );

    _FUNCTION_POINTER_LINK( Free );

    return self;
}

