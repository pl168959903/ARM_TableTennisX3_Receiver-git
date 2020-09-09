

#ifndef _DRIVER_MCU_SPI_
#define _DRIVER_MCU_SPI_

#include "Driver_MCU.h"

// API版本號
#define DRIVER_SPI_API_VERSION DRIVER_VERSION_MAIJOR_MINOR( 1, 00 )

#define __DRV_SPI_PARAMS()        \
    struct {                      \
        void*    baseAddress;     \
        uint32_t masterSlave : 1; \
        uint32_t mode : 2;        \
        uint32_t dataWidth : 8;   \
        uint32_t busClock;        \
    }

//****************************************************************
//參數結構
typedef struct _DRV_SPI_PARAMS {
    __DRV_SPI_PARAMS();
} DRV_SPI_PARAMS;

//****************************************************************
//物件結構
typedef struct _DRV_SPI {
    //參數
    union {
        DRV_SPI_PARAMS Params;
        __DRV_SPI_PARAMS();
    };

    // 函式
    DRIVER_VERSION ( *GetVersion )( void );

    void ( *Initialize )( struct _DRV_SPI* self );
    void ( *Deinitialize )( struct _DRV_SPI* self );
    void ( *CsEnable )( struct _DRV_SPI* self );
    void ( *CsDisable )( struct _DRV_SPI* self );
    void ( *Write )( struct _DRV_SPI* self, uint32_t data );
    void ( *Read )( struct _DRV_SPI* self, uint32_t* data );
    void ( *Trigger )( struct _DRV_SPI* self );
    void ( *ReadAndWrite )( struct _DRV_SPI* self, uint8_t* writeData, uint8_t* readData, uint32_t count );

    // 事件
    void ( *Event_UnitsTxComplete )( void );
    void ( *Event_Slave3wireStart )( void );
    void ( *Event_FifoTxThreshold )( void );
    void ( *Event_FifoRxThreshold )( void );
    void ( *Event_FifoRxOverflow )( void );
    void ( *Event_FifoRxTimeout )( void );

    //物件操作
    void ( *Free )( struct _DRV_SPI* self );
} DRV_SPI;

//****************************************************************
//對外函式
DRIVER_VERSION DRV_SPI_GetVersion( void );
DRV_SPI*       DRV_SpiNew( DRV_SPI_PARAMS spiParams );

//****************************************************************
// Extern

#endif /* _DRIVER_MCU_SPI_ */
