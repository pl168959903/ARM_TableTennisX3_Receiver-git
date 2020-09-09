#include "TableTennisX3_Receiver.h"
uint8_t g_rfAddress[ 5 ] = { 0x00, 0x00, 0x01, 0x00, 0x00 };
uint8_t dt[ 32 ]         = { 0 };
uint8_t packet[ 58 ]     = { 0 };
int     main( void ) {
    uint8_t i;

    MCU_Init();
    NRF_Init();

    nrf->AddRxChannel( nrf, ( DRV_NRF_RX_PARAMS ){ 1, g_rfAddress, 32, 1, 1 } );
    DRV_NRF_RX_MODE( nrf );

    while ( 1 ) {
        if ( nrf->Flags.RX_DR == 1 ) {
            uint8_t size;
			nrf->Flags.RX_DR = 0;
            size = nrf->ReadPacket( nrf, dt );
            if ( dt[ 0 ] == 0 && size == 26 ) {
                for ( i = 0; i < 24; i++ ) packet[ i ] = dt[ i + 1 ];
            }
            if ( dt[ 0 ] == 1 && size == 26 ) {
                for ( i = 0; i < 24; i++ ) packet[ i + 24 ] = dt[ i + 1 ];
            }
            if ( dt[ 0 ] == 2 && size == 26 ) {
                for ( i = 0; i < 10; i++ ) packet[ i + 48 ] = dt[ i + 1 ];
                _LED0_PIN = 0;
                UART_Write( UART1, packet, 58 );
                _LED0_PIN = 1;
            }
            DRV_NRF_FLUSH_RX( nrf );

            
        }
    }
}
