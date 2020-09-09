
#include "Driver_nRF24L01P.h"

#define _NRF_SPI self->spi

static void Initialize( DRV_NRF *self, DRV_NRF_PARAMS params ) {
    self->spi   = params.spi;
    self->SetCE = params.SetCE;

    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_CFG, _DRV_NRF_REG_CFG_EN_CRC_MSK | _DRV_NRF_REG_CFG_PWR_UP_MSK );
    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_EN_AA, 0x00 );
    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_EN_RXADDR, 0x00 );
    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_SETUP_AW, params.addrWidth );
    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_SETUP_RETR, ( params.autoResendDelay << 4 ) | params.autoResendCount );
    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_RF_CH, params.rfChannel );
    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_RF_SETUP, ( ( params.rfDataRate & 0x2 ) << 5 ) | ( ( params.rfDataRate & 0x1 ) << 3 ) | ( params.rfPower << 1 ) | _DRV_NRF_REG_RF_SETUP_LNA_HCURR_MSK );
    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_DYNPD, 0x00 );
    DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_FEATURE, 0x00 );
    DRV_NRF_FLUSH_TX( self );
    DRV_NRF_FLUSH_RX( self );
    DRV_NRF_RST_IRQ( self, _DRV_NRF_REG_STATUS_RX_DR_MSK | _DRV_NRF_REG_STATUS_TX_DS_MSK | _DRV_NRF_REG_STATUS_MAX_RT_MSK );
    self->irqFlags = 0;
}

static bool SendPacket( DRV_NRF *self, DRV_NRF_TX_PARAMS txParams ) {
    self->SetCE( 1 );
    DRV_NRF_TX_MODE( self );
    DRV_NRF_WRITE( self, _DRV_NRF_REG_TX_ADDR, txParams.address, 5 );
    if ( txParams.autoAck == 1 ) {
        DRV_NRF_RX_CH_ENABLE( self, 0x1 );
        DRV_NRF_AUTOACK_ENABLE( self, 0x1 );
        DRV_NRF_WRITE( self, _DRV_NRF_REG_RX_ADDR_P0, txParams.address, 5 );
    }
    if ( txParams.dypw == 1 ) {
        DRV_NRF_DPL_ENABLE( self );
        DRV_NRF_DPL_CH_ENABLE( self, 0x1 );
    }

    self->irqFlags = 0;
    DRV_NRF_WRITE_TX_PAYLOAD( self, txParams.data, txParams.length );
    while ( ( ( self->Flags.TX_DR ) | ( self->Flags.MAX_RT ) ) == 0 ) {};
    if ( self->Flags.TX_DR ) { return 1; }
    if ( self->Flags.MAX_RT ) { return 0; }
    return 0;
}

static void AddRxChannel( DRV_NRF *self, DRV_NRF_RX_PARAMS rxParams ) {
    DRV_NRF_RX_CH_ENABLE( self, 0x1 << rxParams.channel );
    if ( rxParams.channel > 1 )
        DRV_NRF_WRITE( self, _DRV_NRF_REG_RX_ADDR_P0 + rxParams.channel, rxParams.address, 1 );
    else
        DRV_NRF_WRITE( self, _DRV_NRF_REG_RX_ADDR_P0 + rxParams.channel, rxParams.address, 5 );
    DRV_NRF_SET_RX_PW( self, rxParams.channel, rxParams.dataWidth );
    if ( rxParams.autoAck == 1 ) DRV_NRF_AUTOACK_ENABLE( self, 0x1 << rxParams.channel );
    if ( rxParams.dypw == 1 ) {
        DRV_NRF_DPL_ENABLE( self );
        DRV_NRF_DPL_CH_ENABLE( self, 0x1 << rxParams.channel );
    }
}

static uint8_t ReadPacket(DRV_NRF *self, uint8_t * array){
    uint8_t size = DRV_NRF_READ_RX_PAYLOAD_WIDE( self);
    DRV_NRF_READ_RX_PAYLOAD( self, array, size );
    return size;
}

static uint8_t CmdRead( DRV_NRF *self, uint8_t cmd, uint8_t *addr, uint32_t length ) {
    uint8_t returnValue;
    _NRF_SPI->CsEnable( _NRF_SPI );
    _NRF_SPI->ReadAndWrite( _NRF_SPI, &cmd, NULL, 1 );
    if ( addr != NULL ) {
        _NRF_SPI->ReadAndWrite( _NRF_SPI, NULL, addr, length );
        returnValue = *addr;
    }
    else {
        _NRF_SPI->ReadAndWrite( _NRF_SPI, NULL, &returnValue, 1 );
    }
    _NRF_SPI->CsDisable( _NRF_SPI );
    return returnValue;
}

static uint8_t CmdWrite( DRV_NRF *self, uint8_t cmd, uint8_t *addr, uint32_t len ) {
    uint8_t returnValue;
    _NRF_SPI->CsEnable( _NRF_SPI );
    _NRF_SPI->ReadAndWrite( _NRF_SPI, &cmd, &returnValue, 1 );
    _NRF_SPI->ReadAndWrite( _NRF_SPI, addr, NULL, len );
    _NRF_SPI->CsDisable( _NRF_SPI );
    return returnValue;
}

static uint8_t SetRegisters( DRV_NRF *self, uint8_t regAddr, uint8_t regMsk ) {
    uint8_t regData = DRV_NRF_READ_REGISTER( self, regAddr );
    regData |= regMsk;
    DRV_NRF_WRITE_REGISTER( self, regAddr, regData );
    return regData;
}

static uint8_t ClrRegisters( DRV_NRF *self, uint8_t regAddr, uint8_t regMsk ) {
    uint8_t regData = DRV_NRF_READ_REGISTER( self, regAddr );
    regData &= regMsk;
    DRV_NRF_WRITE_REGISTER( self, regAddr, regData );
    return regData;
}

static void Event_RX_DR( DRV_NRF *self ) {
    self->Flags.RX_DR = 1;
    DRV_NRF_RST_IRQ( self, _DRV_NRF_REG_STATUS_RX_DR_MSK );
}
static void Event_TX_DS( DRV_NRF *self ) {
    self->Flags.TX_DR = 1;
    DRV_NRF_RST_IRQ( self, _DRV_NRF_REG_STATUS_TX_DS_MSK );
}
static void Event_MAX_RT( DRV_NRF *self ) {
    self->Flags.MAX_RT = 1;
    DRV_NRF_RST_IRQ( self, _DRV_NRF_REG_STATUS_MAX_RT_MSK );
}

static void Free( DRV_NRF *self ) {
    _DRV_FREE( self );
}

DRV_NRF *DRV_nrfNew( void ) {
    DRV_NRF *self = _DRV_MACCLOC( sizeof( DRV_NRF ) );

    _FUNCTION_POINTER_LINK( Initialize );
    _FUNCTION_POINTER_LINK( CmdRead );
    _FUNCTION_POINTER_LINK( CmdWrite );
    _FUNCTION_POINTER_LINK( SetRegisters );
    _FUNCTION_POINTER_LINK( ClrRegisters );

    _FUNCTION_POINTER_LINK( SendPacket );
    _FUNCTION_POINTER_LINK( AddRxChannel );
    _FUNCTION_POINTER_LINK( ReadPacket );

    _FUNCTION_POINTER_LINK( Event_RX_DR );
    _FUNCTION_POINTER_LINK( Event_TX_DS );
    _FUNCTION_POINTER_LINK( Event_MAX_RT );

    _FUNCTION_POINTER_LINK( Free );
    return self;
}
