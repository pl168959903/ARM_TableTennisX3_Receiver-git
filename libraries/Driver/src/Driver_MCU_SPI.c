
#include "Driver_MCU_SPI.h"
#include "TableTennisX3_Receiver.h"

// DRV版本號
#define DRIVER_SPI_DRV_VERSION DRIVER_VERSION_MAIJOR_MINOR( 1, 00 )

#define __SELF_BASE ( SPI_T* )( self->baseAddress )

//********************************************************************
/**
 * @brief  取得版本號
 * @note
 * @retval 版本號
 */
DRIVER_VERSION DRV_SPI_GetVersion( void ) {
    uint32_t version = ( ( DRIVER_SPI_DRV_VERSION << 16 ) | DRIVER_SPI_API_VERSION );
    return *( DRIVER_VERSION* )( &version );
}

/**
 * @brief  初始化物件
 * @note
 * @param  self: this指標
 * @retval None
 */
static void Initialize( DRV_SPI* self ) {
    uint32_t masterSlave, mode;
    if ( self->masterSlave )
        masterSlave = SPI_SLAVE;
    else
        masterSlave = SPI_MASTER;

    if ( self->mode == 0 )
        mode = SPI_MODE_0;
    else if ( self->mode == 1 )
        mode = SPI_MODE_1;
    else if ( self->mode == 2 )
        mode = SPI_MODE_2;
    else if ( self->mode == 3 )
        mode = SPI_MODE_3;

    SPI_Open( __SELF_BASE, masterSlave, mode, self->dataWidth, self->busClock );
}

/**
 * @brief  取消初始化
 * @note
 * @param  self: this指標
 * @retval None
 */
static void Deinitialize( DRV_SPI* self ) {
    if ( __SELF_BASE == SPI0 ) SYS_ResetModule( SPI0_RST );
    if ( __SELF_BASE == SPI1 ) SYS_ResetModule( SPI1_RST );
    if ( __SELF_BASE == SPI2 ) SYS_ResetModule( SPI2_RST );
    if ( __SELF_BASE == SPI3 ) SYS_ResetModule( SPI3_RST );
}

/**
 * @brief  啟動CS腳位
 * @note
 * @param  self: this指標
 * @retval None
 */
static void CsEnable( DRV_SPI* self ) {
    SPI_SET_SS0_LOW( __SELF_BASE );
}

/**
 * @brief  關閉CS腳位
 * @note
 * @param  self: this指標
 * @retval None
 */
static void CsDisable( DRV_SPI* self ) {
    SPI_SET_SS0_HIGH( __SELF_BASE );
}

/**
 * @brief  寫入Byte傳送暫存區
 * @note
 * @param  self: this指標
 * @param  data: 資料
 * @retval None
 */
static void Write( DRV_SPI* self, uint32_t data ) {
    SPI_WRITE_TX0( __SELF_BASE, data );
}

/**
 * @brief  讀取Byte接收暫存區
 * @note
 * @param  self: this指標
 * @param  data: 資料地址
 * @retval None
 */
static void Read( DRV_SPI* self, uint32_t* data ) {
    *data = SPI_READ_RX0( __SELF_BASE );
}

/**
 * @brief  觸發SPI
 * @note
 * @param  self: this指標
 * @retval None
 */
static void Trigger( DRV_SPI* self ) {
    SPI_TRIGGER( __SELF_BASE );
}

/**
 * @brief  讀取&傳送byte組
 * @note
 * @param  self: this指標
 * @param  writeData: 傳送資料地址
 * @param  readData: 儲存資料地址
 * @param  count: 長度
 * @retval None
 */
static void ReadAndWrite( DRV_SPI* self, uint8_t* writeData, uint8_t* readData, uint32_t len ) {
    uint8_t* wd = writeData;
    uint8_t* rd = readData;
    while ( len-- ) {
        if ( self->masterSlave == 0 ) {
            if ( writeData != NULL ) self->Write( self, *( wd++ ) );
            self->Trigger( self );
            while ( SPI_IS_BUSY( __SELF_BASE ) ) {};
            if ( readData != NULL ) *( rd++ ) = SPI_READ_RX0( __SELF_BASE );
        }
    }
}

//********************************************************************
// 結構操作
/**
 * @brief  釋放物件記憶體
 * @note
 * @param  *self: this 指標
 * @retval None
 */
static void DRV_SpiFree( DRV_SPI* spi ) {
    _DRV_FREE( spi );
}

/**
 * @brief  建立物件
 * @note
 * @param  *self: this 指標
 * @param  param : 物件參數結構
 * @retval 物件指標
 */
DRV_SPI* DRV_SpiNew( DRV_SPI_PARAMS spiParams ) {

    //初始化
    DRV_SPI* self = _DRV_MACCLOC( sizeof( DRV_SPI ) );                       //建立物件&分配記憶體
    _DRV_MEMCPY( &( self->Params ), &spiParams, sizeof( DRV_SPI_PARAMS ) );  //參數Copy

    //函示
    self->GetVersion   = DRV_SPI_GetVersion;
    self->Initialize   = Initialize;
    self->Deinitialize = Deinitialize;
    self->CsEnable     = CsEnable;
    self->CsDisable    = CsDisable;
    self->Write        = Write;
    self->Read         = Read;
    self->Trigger      = Trigger;
    self->ReadAndWrite = ReadAndWrite;

    // 事件
    // self->Event_UnitsTxComplete =
    // self->Event_Slave3wireStart =
    // self->Event_FifoTxThreshold =
    // self->Event_FifoRxThreshold =
    // self->Event_FifoRxOverflow =
    // self->Event_FifoRxTimeout =

    //物件操作
    self->Free = DRV_SpiFree;

    return self;
}
