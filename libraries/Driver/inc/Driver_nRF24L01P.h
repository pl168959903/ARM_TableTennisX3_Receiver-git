
#ifndef _DRIVER_NRF24L01P_
#define _DRIVER_NRF24L01P_

#include "Driver_MCU.h"
#include "Driver_MCU_SPI.h"

typedef struct _DRV_NRF_TX_PARAMS {
    uint8_t *address;
    uint8_t  autoAck : 1;
    uint8_t  dypw : 1;
    uint8_t *data;
    uint8_t  length;
} DRV_NRF_TX_PARAMS;

typedef struct _DRV_NRF_RX_PARAMS {
    uint8_t  channel : 3;
    uint8_t *address;
    uint8_t  dataWidth : 6;
    uint8_t  autoAck : 1;
    uint8_t  dypw : 1;
} DRV_NRF_RX_PARAMS;

typedef struct _DRV_NRF_PARAMS {
    DRV_SPI *spi;
    void ( *SetCE )( uint8_t en );

    uint8_t rfChannel : 7;

    uint8_t addrWidth : 2;
    uint8_t rfDataRate : 2;
    uint8_t rfPower : 2;

    uint8_t autoResendDelay : 4;
    uint8_t autoResendCount : 4;

} DRV_NRF_PARAMS;

typedef struct _DRV_NRF {
    DRV_SPI *spi;

    union {
        volatile uint8_t irqFlags;
        struct {
            volatile uint8_t RX_DR : 1;
            volatile uint8_t TX_DR : 1;
            volatile uint8_t MAX_RT : 1;
        } Flags;
    };

    void ( *Initialize )( struct _DRV_NRF *self, DRV_NRF_PARAMS params );
    uint8_t ( *CmdRead )( struct _DRV_NRF *self, uint8_t cmd, uint8_t *addr, uint32_t len );
    uint8_t ( *CmdWrite )( struct _DRV_NRF *self, uint8_t cmd, uint8_t *addr, uint32_t len );
    uint8_t ( *SetRegisters )( struct _DRV_NRF *self, uint8_t regAddr, uint8_t regMsk );
    uint8_t ( *ClrRegisters )( struct _DRV_NRF *self, uint8_t regAddr, uint8_t regMsk );

    bool ( *SendPacket )( struct _DRV_NRF *self, DRV_NRF_TX_PARAMS txParams );
    void ( *AddRxChannel )( struct _DRV_NRF *self, DRV_NRF_RX_PARAMS rxParams );
    uint8_t ( *ReadPacket )( struct _DRV_NRF *self, uint8_t *array );

    void ( *Event_RX_DR )( struct _DRV_NRF *self );
    void ( *Event_TX_DS )( struct _DRV_NRF *self );
    void ( *Event_MAX_RT )( struct _DRV_NRF *self );

    void ( *SetCE )( uint8_t en );

    void ( *Free )( struct _DRV_NRF *self );
} DRV_NRF;

#define DRV_NRF_READ( self, regAddr, addr, len )       self->CmdRead( self, ( regAddr | _DRV_NRF_CMD_R_REGISTER ), addr, len )
#define DRV_NRF_READ_REGISTER( self, regAddr )         DRV_NRF_READ( self, regAddr, NULL, 1 );
#define DRV_NRF_WRITE( self, regAddr, addr, len )      self->CmdWrite( self, ( regAddr | _DRV_NRF_CMD_W_REGISTER ), addr, len )
#define DRV_NRF_WRITE_REGISTER( self, regAddr, data )  DRV_NRF_WRITE( self, regAddr, ( uint8_t[] ){ data }, 1 );
#define DRV_NRF_SET_REGISTER( self, regAddr, msk )     self->SetRegisters( self, regAddr, msk )
#define DRV_NRF_CLR_REGISTER( self, regAddr, msk )     self->ClrRegisters( self, regAddr, msk )
#define DRV_NRF_READ_RX_PAYLOAD( self, array, len )    self->CmdRead( self, _DRV_NRF_CMD_R_RX_PAYLOAD, array, len )
#define DRV_NRF_WRITE_TX_PAYLOAD( self, array, len )   self->CmdWrite( self, _DRV_NRF_CMD_W_TX_PAYLOAD, array, len )
#define DRV_NRF_FLUSH_TX( self )                       self->CmdWrite( self, _DRV_NRF_CMD_FLUSH_TX, NULL, 0 )
#define DRV_NRF_FLUSH_RX( self )                       self->CmdWrite( self, _DRV_NRF_CMD_FLUSH_RX, NULL, 0 )
#define DRV_NRF_REUSE_TX( self )                       self->CmdWrite( self, _DRV_NRF_CMD_REUSE_TX_PL, NULL, 0 )
#define DRV_NRF_READ_RX_PAYLOAD_WIDE( self )           self->CmdRead( self, _DRV_NRF_CMD_R_RX_PL_WID, NULL, 1 )
#define DRV_NRF_ACK_PAYLOAD( self, addr, len, ch )     self->CmdWrite( self, _DRV_NRF_CMD_W_ACK_PAYLOAD | ( ch & 0x7 ), addr, len )
#define DRV_NRF_TX_WITHOUT_AUTO_ACK( self, addr, len ) self->CmdWrite( self, _DRV_NRF_CMD_W_TX_PAYLOAD_NOACK, addr, len )
#define DRV_NRF_NOP( self )                            self->CmdWrite( self, _DRV_NRF_CMD_NOP, NULL, 0 )

//************************************************************************
#define DRV_NRF_POWER_DOWN( self )            DRV_NRF_CLR_REGISTER( self, _DRV_NRF_REG_CFG, ~_DRV_NRF_REG_CFG_PWR_UP_MSK )
#define DRV_NRF_POWER_ON( self )              DRV_NRF_SET_REGISTER( self, _DRV_NRF_REG_CFG, _DRV_NRF_REG_CFG_PWR_UP_MSK )
#define DRV_NRF_RX_MODE( self )               DRV_NRF_SET_REGISTER( self, _DRV_NRF_REG_CFG, _DRV_NRF_REG_CFG_PRIM_RX_MSK )
#define DRV_NRF_TX_MODE( self )               DRV_NRF_CLR_REGISTER( self, _DRV_NRF_REG_CFG, ~_DRV_NRF_REG_CFG_PRIM_RX_MSK )
#define DRV_NRF_RST_IRQ( self, msk )          DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_STATUS, msk )
#define DRV_NRF_SET_RX_PW( self, ch, pw )     DRV_NRF_WRITE_REGISTER( self, _DRV_NRF_REG_RX_PW_P0 + ch, pw )
#define DRV_NRF_AUTOACK_ENABLE( self, chMsk ) DRV_NRF_SET_REGISTER( self, _DRV_NRF_REG_EN_AA, chMsk )
#define DRV_NRF_RX_CH_ENABLE( self, chMsk )   DRV_NRF_SET_REGISTER( self, _DRV_NRF_REG_EN_RXADDR, chMsk )
#define DRV_NRF_DPL_CH_ENABLE( self, chMsk )  DRV_NRF_SET_REGISTER( self, _DRV_NRF_REG_DYNPD, chMsk )
#define DRV_NRF_DPL_ENABLE( self )            DRV_NRF_SET_REGISTER( self, _DRV_NRF_REG_FEATURE, _DRV_NRF_REG_FEATURE_EN_DPL_MSK )
#define DRV_NRF_ACK_PAY_ENABLE( self )        DRV_NRF_SET_REGISTER( self, _DRV_NRF_REG_FEATURE, _DRV_NRF_REG_FEATURE_EN_ACK_MSK )
#define DRV_NRF_GET_RX_P_NO( self )           ( ( DRV_NRF_NOP( self ) & 0x0E ) >> 1 )

DRV_NRF *DRV_nrfNew( void );

//-------------------------------------------------
// NRF CMD
#define _DRV_NRF_CMD_R_REGISTER         0x00
#define _DRV_NRF_CMD_W_REGISTER         0x20
#define _DRV_NRF_CMD_R_RX_PAYLOAD       0x61
#define _DRV_NRF_CMD_W_TX_PAYLOAD       0xA0
#define _DRV_NRF_CMD_FLUSH_TX           0xE1
#define _DRV_NRF_CMD_FLUSH_RX           0xE2
#define _DRV_NRF_CMD_REUSE_TX_PL        0xE3
#define _DRV_NRF_CMD_R_RX_PL_WID        0x60
#define _DRV_NRF_CMD_W_ACK_PAYLOAD      0xA8
#define _DRV_NRF_CMD_W_TX_PAYLOAD_NOACK 0xB0
#define _DRV_NRF_CMD_NOP                0xFF
//-------------------------------------------------

//-------------------------------------------------
// NRF REG
#define _DRV_NRF_REG_CFG                 0x00
#define _DRV_NRF_REG_CFG_MAKE_RX_RD_POS  ( 6 )
#define _DRV_NRF_REG_CFG_MAKE_RX_RD_MSK  ( 0x1 << _DRV_NRF_REG_CFG_MAKE_RX_RD_POS )
#define _DRV_NRF_REG_CFG_MAKE_TX_RS_POS  ( 5 )
#define _DRV_NRF_REG_CFG_MAKE_TX_RS_MSK  ( 0x1 << _DRV_NRF_REG_CFG_MAKE_TX_RS_POS )
#define _DRV_NRF_REG_CFG_MAKE_MAX_RT_POS ( 4 )
#define _DRV_NRF_REG_CFG_MAKE_MAX_RT_MSK ( 0x1 << _DRV_NRF_REG_CFG_MAKE_MAX_RT_POS )
#define _DRV_NRF_REG_CFG_EN_CRC_POS      ( 3 )
#define _DRV_NRF_REG_CFG_EN_CRC_MSK      ( 0x1 << _DRV_NRF_REG_CFG_EN_CRC_POS )
#define _DRV_NRF_REG_CFG_CRCO_POS        ( 2 )
#define _DRV_NRF_REG_CFG_CRCO_MSK        ( 0x1 << _DRV_NRF_REG_CFG_CRCO_POS )
#define _DRV_NRF_REG_CFG_PWR_UP_POS      ( 1 )
#define _DRV_NRF_REG_CFG_PWR_UP_MSK      ( 0x1 << _DRV_NRF_REG_CFG_PWR_UP_POS )
#define _DRV_NRF_REG_CFG_PRIM_RX_POS     ( 0 )
#define _DRV_NRF_REG_CFG_PRIM_RX_MSK     ( 0x1 << _DRV_NRF_REG_CFG_PRIM_RX_POS )

#define _DRV_NRF_REG_EN_AA        0x01
#define _DRV_NRF_REG_EN_AA_P5_POS ( 5 )
#define _DRV_NRF_REG_EN_AA_P5_MSK ( 0x1 << _DRV_NRF_REG_EN_AA_P5_POS )
#define _DRV_NRF_REG_EN_AA_P4_POS ( 4 )
#define _DRV_NRF_REG_EN_AA_P4_MSK ( 0x1 << _DRV_NRF_REG_EN_AA_P4_POS )
#define _DRV_NRF_REG_EN_AA_P3_POS ( 3 )
#define _DRV_NRF_REG_EN_AA_P3_MSK ( 0x1 << _DRV_NRF_REG_EN_AA_P3_POS )
#define _DRV_NRF_REG_EN_AA_P2_POS ( 2 )
#define _DRV_NRF_REG_EN_AA_P2_MSK ( 0x1 << _DRV_NRF_REG_EN_AA_P2_POS )
#define _DRV_NRF_REG_EN_AA_P1_POS ( 1 )
#define _DRV_NRF_REG_EN_AA_P1_MSK ( 0x1 << _DRV_NRF_REG_EN_AA_P1_POS )
#define _DRV_NRF_REG_EN_AA_P0_POS ( 0 )
#define _DRV_NRF_REG_EN_AA_P0_MSK ( 0x1 << _DRV_NRF_REG_EN_AA_P0_POS )

#define _DRV_NRF_REG_EN_RXADDR            0x02
#define _DRV_NRF_REG_EN_RXADDR_ERX_P5_POS ( 5 )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P5_MSK ( 0x1 << _DRV_NRF_REG_EN_RXADDR_ERX_P5_POS )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P4_POS ( 4 )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P4_MSK ( 0x1 << _DRV_NRF_REG_EN_RXADDR_ERX_P4_POS )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P3_POS ( 3 )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P3_MSK ( 0x1 << _DRV_NRF_REG_EN_RXADDR_ERX_P3_POS )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P2_POS ( 2 )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P2_MSK ( 0x1 << _DRV_NRF_REG_EN_RXADDR_ERX_P2_POS )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P1_POS ( 1 )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P1_MSK ( 0x1 << _DRV_NRF_REG_EN_RXADDR_ERX_P1_POS )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P0_POS ( 0 )
#define _DRV_NRF_REG_EN_RXADDR_ERX_P0_MSK ( 0x1 << _DRV_NRF_REG_EN_RXADDR_ERX_P0_POS )

#define _DRV_NRF_REG_SETUP_AW     0x03
#define _DRV_NRF_REG_SETUP_AW_POS ( 0 )
#define _DRV_NRF_REG_SETUP_AW_MSK ( 0x3 << _DRV_NRF_REG_SETUP_AW_POS )

#define _DRV_NRF_REG_SETUP_RETR         0x04
#define _DRV_NRF_REG_SETUP_RETR_ARD_POS ( 4 )
#define _DRV_NRF_REG_SETUP_RETR_ARD_MSK ( 0xF << _DRV_NRF_REG_SETUP_RETR_ARD_POS )
#define _DRV_NRF_REG_SETUP_RETR_ARC_POS ( 0 )
#define _DRV_NRF_REG_SETUP_RETR_ARC_MSK ( 0xF << _DRV_NRF_REG_SETUP_RETR_ARC_POS )

#define _DRV_NRF_REG_RF_CH     0x05
#define _DRV_NRF_REG_RF_CH_POS ( 0 )
#define _DRV_NRF_REG_RF_CH_MSK ( 0x7F << _DRV_NRF_REG_RF_CH_POS )

#define _DRV_NRF_REG_RF_SETUP                0x06
#define _DRV_NRF_REG_RF_SETUP_CONT_WAVE_POS  ( 7 )
#define _DRV_NRF_REG_RF_SETUP_CONT_WAVE_MSK  ( 0x1 << _DRV_NRF_REG_RF_SETUP_CONT_WAVE_POS )
#define _DRV_NRF_REG_RF_SETUP_RF_DR_LOW_POS  ( 5 )
#define _DRV_NRF_REG_RF_SETUP_RF_DR_LOW_MSK  ( 0x1 << _DRV_NRF_REG_RF_SETUP_RF_DR_LOW_POS )
#define _DRV_NRF_REG_RF_SETUP_PLL_LOCK_POS   ( 4 )
#define _DRV_NRF_REG_RF_SETUP_PLL_LOCK_MSK   ( 0x1 << _DRV_NRF_REG_RF_SETUP_PLL_LOCK_POS )
#define _DRV_NRF_REG_RF_SETUP_RF_DR_HIGH_POS ( 3 )
#define _DRV_NRF_REG_RF_SETUP_RF_DR_HIGH_MSK ( 0x1 << _DRV_NRF_REG_RF_SETUP_RF_DR_HIGH_POS )
#define _DRV_NRF_REG_RF_SETUP_RF_PWR_POS     ( 1 )
#define _DRV_NRF_REG_RF_SETUP_RF_PWR_MSK     ( 0x3 << _DRV_NRF_REG_RF_SETUP_RF_PWR_POS )
#define _DRV_NRF_REG_RF_SETUP_LNA_HCURR_POS  ( 0 )
#define _DRV_NRF_REG_RF_SETUP_LNA_HCURR_MSK  ( 0x1 << _DRV_NRF_REG_RF_SETUP_LNA_HCURR_POS )

#define _DRV_NRF_REG_STATUS             0x07
#define _DRV_NRF_REG_STATUS_RX_DR_POS   ( 6 )
#define _DRV_NRF_REG_STATUS_RX_DR_MSK   ( 0x1 << _DRV_NRF_REG_STATUS_RX_DR_POS )
#define _DRV_NRF_REG_STATUS_TX_DS_POS   ( 5 )
#define _DRV_NRF_REG_STATUS_TX_DS_MSK   ( 0x1 << _DRV_NRF_REG_STATUS_TX_DS_POS )
#define _DRV_NRF_REG_STATUS_MAX_RT_POS  ( 4 )
#define _DRV_NRF_REG_STATUS_MAX_RT_MSK  ( 0x1 << _DRV_NRF_REG_STATUS_MAX_RT_POS )
#define _DRV_NRF_REG_STATUS_RX_P_NO_POS ( 1 )
#define _DRV_NRF_REG_STATUS_RX_P_NO_MSK ( 0x7 << _DRV_NRF_REG_STATUS_RX_P_NO_POS )
#define _DRV_NRF_REG_STATUS_TX_FULL_POS ( 0 )
#define _DRV_NRF_REG_STATUS_TX_FULL_MSK ( 0x1 << _DRV_NRF_REG_STATUS_TX_FULL_POS )

#define _DRV_NRF_REG_OBSERVE_TX              0x08
#define _DRV_NRF_REG_OBSERVE_TX_PLOS_CNT_POS ( 4 )
#define _DRV_NRF_REG_OBSERVE_TX_PLOS_CNT_MSK ( 0xF << _DRV_NRF_REG_OBSERVE_TX_PLOS_CNT_POS )
#define _DRV_NRF_REG_OBSERVE_TX_ARC_CNT_POS  ( 0 )
#define _DRV_NRF_REG_OBSERVE_TX_ARC_CNT_MSK  ( 0xF << _DRV_NRF_REG_OBSERVE_TX_ARC_CNT_POS )

#define _DRV_NRF_REG_CD     0x09
#define _DRV_NRF_REG_CD_POS ( 0 )
#define _DRV_NRF_REG_CD_MSK ( 0x1 << _DRV_NRF_REG_CD_POS )

#define _DRV_NRF_REG_RX_ADDR_P0 0x0A
#define _DRV_NRF_REG_RX_ADDR_P1 0x0B
#define _DRV_NRF_REG_RX_ADDR_P2 0x0C
#define _DRV_NRF_REG_RX_ADDR_P3 0x0D
#define _DRV_NRF_REG_RX_ADDR_P4 0x0E
#define _DRV_NRF_REG_RX_ADDR_P5 0x0F

#define _DRV_NRF_REG_TX_ADDR 0x10

#define _DRV_NRF_REG_RX_PW_P0     0x11
#define _DRV_NRF_REG_RX_PW_P0_POS ( 0 )
#define _DRV_NRF_REG_RX_PW_P0_MSK ( 0x3F << _DRV_NRF_REG_RX_PW_P0_POS )

#define _DRV_NRF_REG_RX_PW_P1     0x12
#define _DRV_NRF_REG_RX_PW_P1_POS ( 0 )
#define _DRV_NRF_REG_RX_PW_P1_MSK ( 0x3F << _DRV_NRF_REG_RX_PW_P1_POS )

#define _DRV_NRF_REG_RX_PW_P2     0x13
#define _DRV_NRF_REG_RX_PW_P2_POS ( 0 )
#define _DRV_NRF_REG_RX_PW_P2_MSK ( 0x3F << _DRV_NRF_REG_RX_PW_P2_POS )

#define _DRV_NRF_REG_RX_PW_P3     0x14
#define _DRV_NRF_REG_RX_PW_P3_POS ( 0 )
#define _DRV_NRF_REG_RX_PW_P3_MSK ( 0x3F << _DRV_NRF_REG_RX_PW_P3_POS )

#define _DRV_NRF_REG_RX_PW_P4     0x15
#define _DRV_NRF_REG_RX_PW_P4_POS ( 0 )
#define _DRV_NRF_REG_RX_PW_P4_MSK ( 0x3F << _DRV_NRF_REG_RX_PW_P4_POS )

#define _DRV_NRF_REG_RX_PW_P5     0x16
#define _DRV_NRF_REG_RX_PW_P5_POS ( 0 )
#define _DRV_NRF_REG_RX_PW_P5_MSK ( 0x3F << _DRV_NRF_REG_RX_PW_P5_POS )

#define _DRV_NRF_REG_FIFO_STATUS              0x17
#define _DRV_NRF_REG_FIFO_STATUS_TX_REUSE_POS ( 6 )
#define _DRV_NRF_REG_FIFO_STATUS_TX_REUSE_MSK ( 0x1 << _DRV_NRF_REG_FIFO_STATUS_TX_REUSE_POS )
#define _DRV_NRF_REG_FIFO_STATUS_TX_FULL_POS  ( 5 )
#define _DRV_NRF_REG_FIFO_STATUS_TX_FULL_MSK  ( 0x1 << _DRV_NRF_REG_FIFO_STATUS_TX_FULL_POS )
#define _DRV_NRF_REG_FIFO_STATUS_TX_EMPTY_POS ( 4 )
#define _DRV_NRF_REG_FIFO_STATUS_TX_EMPTY_MSK ( 0x1 << _DRV_NRF_REG_FIFO_STATUS_TX_EMPTY_POS )
#define _DRV_NRF_REG_FIFO_STATUS_RX_FULL_POS  ( 1 )
#define _DRV_NRF_REG_FIFO_STATUS_RX_FULL_MSK  ( 0x1 << _DRV_NRF_REG_FIFO_STATUS_RX_FULL_POS )
#define _DRV_NRF_REG_FIFO_STATUS_RX_EMPTY_POS ( 0 )
#define _DRV_NRF_REG_FIFO_STATUS_RX_EMPTY_MSK ( 0x1 << _DRV_NRF_REG_FIFO_STATUS_RX_EMPTY_POS )

#define _DRV_NRF_REG_DYNPD            0x1C
#define _DRV_NRF_REG_DYNPD_DPL_P5_POS ( 5 )
#define _DRV_NRF_REG_DYNPD_DPL_P5_MSK ( 0x1 << _DRV_NRF_REG_DYNPD_DPL_P5_POS )
#define _DRV_NRF_REG_DYNPD_DPL_P4_POS ( 4 )
#define _DRV_NRF_REG_DYNPD_DPL_P4_MSK ( 0x1 << _DRV_NRF_REG_DYNPD_DPL_P4_POS )
#define _DRV_NRF_REG_DYNPD_DPL_P3_POS ( 3 )
#define _DRV_NRF_REG_DYNPD_DPL_P3_MSK ( 0x1 << _DRV_NRF_REG_DYNPD_DPL_P3_POS )
#define _DRV_NRF_REG_DYNPD_DPL_P2_POS ( 2 )
#define _DRV_NRF_REG_DYNPD_DPL_P2_MSK ( 0x1 << _DRV_NRF_REG_DYNPD_DPL_P2_POS )
#define _DRV_NRF_REG_DYNPD_DPL_P1_POS ( 1 )
#define _DRV_NRF_REG_DYNPD_DPL_P1_MSK ( 0x1 << _DRV_NRF_REG_DYNPD_DPL_P1_POS )
#define _DRV_NRF_REG_DYNPD_DPL_P0_POS ( 0 )
#define _DRV_NRF_REG_DYNPD_DPL_P0_MSK ( 0x1 << _DRV_NRF_REG_DYNPD_DPL_P0_POS )

#define _DRV_NRF_REG_FEATURE                0x1D
#define _DRV_NRF_REG_FEATURE_EN_DPL_POS     ( 2 )
#define _DRV_NRF_REG_FEATURE_EN_DPL_MSK     ( 0x1 << _DRV_NRF_REG_FEATURE_EN_DPL_POS )
#define _DRV_NRF_REG_FEATURE_EN_ACK_POS     ( 1 )
#define _DRV_NRF_REG_FEATURE_EN_ACK_MSK     ( 0x1 << _DRV_NRF_REG_FEATURE_EN_ACK_POS )
#define _DRV_NRF_REG_FEATURE_EN_DYN_ACK_POS ( 0 )
#define _DRV_NRF_REG_FEATURE_EN_DYN_ACK_MSK ( 0x1 << _DRV_NRF_REG_FEATURE_EN_DYN_ACK_POS )

//-------------------------------------------------

#endif /* _DRIVER_NRF24L01P_ */
