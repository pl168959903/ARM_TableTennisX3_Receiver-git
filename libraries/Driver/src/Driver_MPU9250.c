
#include "Driver_MPU9250.h"

// DRV版本號
#define DRIVER_MPU9250_DRV_VERSION DRIVER_VERSION_MAIJOR_MINOR( 1, 00 )

#define __MPU9250_SPI self->Params.spi

/**
 * @brief  取得版本號
 * @note
 * @retval 版本號
 */
DRIVER_VERSION DRV_MPU9250_GetVersion( void ) {
    uint32_t version = ( ( DRIVER_MPU9250_DRV_VERSION << 16 ) | DRIVER_MPU9250_API_VERSION );
    return *( DRIVER_VERSION * )( &version );
}

//************************************************************
// Static function

/**
 * @brief  讀取暫存器
 * @note   基礎動作
 * @param  self: this指標
 * @param  regAddr: 暫存器地址
 * @param  data: 存放資料地址，若為NULL則讀取一次
 * @param  len: 讀取長度
 * @retval 第一個讀取的資料
 */
static uint8_t ReadRegister( DRV_MPU9250 *self, uint8_t regAddr, uint8_t *data, uint32_t len ) {
    uint8_t reg = regAddr | _DRV_MPU9250_READ_REG;
    uint8_t returnValue;
    __MPU9250_SPI->CsEnable( __MPU9250_SPI );
    __MPU9250_SPI->ReadAndWrite( __MPU9250_SPI, &reg, NULL, 1 );
    if ( data != NULL ) {
        __MPU9250_SPI->ReadAndWrite( __MPU9250_SPI, NULL, data, len );
        returnValue = *data;
    }
    else {
        __MPU9250_SPI->ReadAndWrite( __MPU9250_SPI, NULL, &returnValue, 1 );
    }
    __MPU9250_SPI->CsDisable( __MPU9250_SPI );
    return returnValue;
}

/**
 * @brief  寫暫存器
 * @note   基礎動作
 * @param  self: this指標
 * @param  regAddr: 暫存器地址
 * @param  data: 寫入資料地址
 * @param  len: 寫入長度
 * @retval None
 */
static void WriteRegister( DRV_MPU9250 *self, uint8_t regAddr, uint8_t *data, uint32_t len ) {
    uint8_t reg = regAddr | _DRV_MPU9250_WRITE_REG;
    __MPU9250_SPI->CsEnable( __MPU9250_SPI );
    __MPU9250_SPI->ReadAndWrite( __MPU9250_SPI, &reg, NULL, 1 );
    __MPU9250_SPI->ReadAndWrite( __MPU9250_SPI, data, NULL, len );
    __MPU9250_SPI->CsDisable( __MPU9250_SPI );
}

/**
 * @brief  初始化物件
 * @note
 * @param  *self: this指標
 * @retval None
 */
static void Initialize( DRV_MPU9250 *self, DRV_MPU9250_PARAMS initParams ) {
    DRV_MPU9250_USE_PLL( self );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_PWR_MGMT_1, 0x01 );  // PLL
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_SMPLRT_DIV, initParams.div );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_MPU_CONFIG, initParams.gyroLpf );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_GYRO_CONFIG, initParams.gyroFs << _DRV_MPU9250_GYRO_CONFIG_GYRO_FS_SEL_Pos );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_ACCEL_CONFIG, initParams.accelFs << _DRV_MPU9250_ACCEL_CONFIG_ACCEL_FS_SEL_Pos );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_ACCEL_CONFIG2, initParams.accelLpf );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_I2C_MST_CTRL, _DRV_MPU9250_I2C_MST_CTRL_WAIT_FOR_ES_Msk | _DRV_MPU9250_I2C_MST_CLK_400K );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_USER_CTRL, _DRV_MPU9250_USER_CTRL_I2C_MST_EN_Msk );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_INT_PIN_CFG, _DRV_MPU9250_INT_PIN_CFG_ACTL_Msk | _DRV_MPU9250_INT_PIN_CFG_OPEN_Msk );
    DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_INT_ENABLE, _DRV_MPU9250_INT_ENABLE_RAW_RDY_EN_Msk );

    self->I2cMstWriteByte( self, _DRV_MPU9250_I2C_SLV0, _DRV_AK8963_ADDRESS, _DRV_AK8963_CNTL, 0x16 );
    self->I2cMstRead( self, _DRV_MPU9250_I2C_SLV0, _DRV_AK8963_ADDRESS, _DRV_AK8963_XOUT_L, NULL, 7 );
}

/**
 * @brief  取消初始化
 * @note
 * @param  *self: this指標
 * @retval None
 */
static void Deinitialize( DRV_MPU9250 *self ) {}

/**
 * @brief  設定暫存器
 * @note   依照遮罩資料，做OR運算操作
 * @param  *self: this 指標
 * @param  regAddr: 暫存器地址
 * @param  regMsk: 遮罩資料
 * @retval 改變後的資料(非讀取)
 */
static uint8_t SetRegisters( DRV_MPU9250 *self, uint8_t regAddr, uint8_t regMsk ) {
    uint8_t regData = DRV_MPU9250_READ_BYTE( self, regAddr );
    regData |= regMsk;
    DRV_MPU9250_WRITE_BYTE( self, regAddr, regData );
    return regData;
}

/**
 * @brief  設定暫存器
 * @note   依照遮罩資料，做AND運算操作
 * @param  *self: this 指標
 * @param  regAddr: 暫存器地址
 * @param  regMsk: 遮罩資料
 * @retval 改變後的資料(非讀取)
 */
static uint8_t ClrRegisters( DRV_MPU9250 *self, uint8_t regAddr, uint8_t regMsk ) {
    uint8_t regData = DRV_MPU9250_READ_BYTE( self, regAddr );
    regData &= regMsk;
    DRV_MPU9250_WRITE_BYTE( self, regAddr, regData );
    return regData;
}
/**
 * @brief  Master I2C 寫byte
 * @note
 * @param  *self: this 指標
 * @param  slv: 選擇主端I2C
 * @param  i2cAddr: 從端地址
 * @param  regAddr: 暫存器地址
 * @param  data: 寫入資料
 * @retval None
 */
static void I2cMstWriteByte( DRV_MPU9250 *self, uint8_t slv, uint8_t i2cAddr, uint8_t regAddr, uint8_t data ) {
    if ( slv == _DRV_MPU9250_I2C_SLV4 ) {
        self->WriteRegister(
            self, _DRV_MPU9250_I2C_SLV4_ADDR, ( uint8_t[] ){ i2cAddr | _DRV_MPU9250_I2C_MST_WRITE, regAddr, data, _DRV_MPU9250_I2C_SLV_ENABLE | _DRV_MPU9250_I2C_SLV4_CTRL_SLV4_DONE_INT_EN_Msk }, 4 );
        self->Params.RawDataReady_Flag = 0;
        while ( self->Params.RawDataReady_Flag == 0 ) {};
    }
    else {
        DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_I2C_SLV0_DO + slv, data );
        self->WriteRegister( self, _DRV_MPU9250_I2C_SLV_BASE( slv ), ( uint8_t[] ){ i2cAddr | _DRV_MPU9250_I2C_MST_WRITE, regAddr, _DRV_MPU9250_I2C_SLV_ENABLE | 1 }, 3 );
        self->Params.RawDataReady_Flag = 0;
        while ( self->Params.RawDataReady_Flag == 0 ) {};
    }
}

/**
 * @brief  讀取已設定的 Master I2C 資料
 * @note
 * @param  *self: this 指標
 * @param  slv: 選擇主端I2C
 * @param  *data: 儲存地址
 * @param  len: 讀取長度
 * @retval None
 */
static void I2cSlvDataRead( DRV_MPU9250 *self, uint8_t slv, uint8_t *data, uint8_t len ) {
    uint8_t i;
    uint8_t addrShift  = 0;
    uint8_t readLength = ( len > self->Params.SlvReadLength[ slv ] ) ? self->Params.SlvReadLength[ slv ] : len;
    readLength         = ( ( addrShift + readLength ) > 24 ) ? ( 24 - addrShift ) : readLength;
    for ( i = 0; i < slv; i++ ) addrShift += self->Params.SlvReadLength[ i ];
    self->ReadRegister( self, _DRV_MPU9250_EXT_SENS_DATA_00 + addrShift, data, readLength );
}

/**
 * @brief  設定 Master I2C並讀取資料
 * @note
 * @param  *self:  this 指標
 * @param  slv: 選擇主端I2C
 * @param  i2cAddr: 從端地址
 * @param  regAddr: 暫存器地址
 * @param  *data: 儲存地址
 * @param  len: 寫入長度
 * @retval None
 */
static void I2cMstRead( DRV_MPU9250 *self, uint8_t slv, uint8_t i2cAddr, uint8_t regAddr, uint8_t *data, uint8_t len ) {
    if ( slv == _DRV_MPU9250_I2C_SLV4 ) {
        self->WriteRegister(
            self, _DRV_MPU9250_I2C_SLV4_ADDR, ( uint8_t[] ){ i2cAddr | _DRV_MPU9250_I2C_MST_READ, regAddr, 0x00, _DRV_MPU9250_I2C_SLV_ENABLE | _DRV_MPU9250_I2C_SLV4_CTRL_SLV4_DONE_INT_EN_Msk }, 4 );
        self->Params.RawDataReady_Flag = 0;
        while ( self->Params.RawDataReady_Flag == 0 ) {};
        if ( data != NULL ) *data = DRV_MPU9250_READ_BYTE( self, _DRV_MPU9250_I2C_SLV4_DI );
    }
    else {
        self->WriteRegister( self, _DRV_MPU9250_I2C_SLV_BASE( slv ), ( uint8_t[] ){ i2cAddr | _DRV_MPU9250_I2C_MST_READ, regAddr, _DRV_MPU9250_I2C_SLV_ENABLE | len }, 3 );
        self->Params.RawDataReady_Flag = 0;
        while ( self->Params.RawDataReady_Flag == 0 ) {};
        self->Params.SlvReadLength[ slv ] = len;
        if ( data != NULL ) self->I2cSlvDataRead( self, slv, data, len );
    }
}

/**
 * @brief  更新原始資料
 * @note   加速度儀及陀螺儀能直接讀取，磁力計須讓SLV0掛載讀取才能使用
 * @param  *self:  this 指標
 * @retval None
 */
static void Updata( struct _DRV_MPU9250 *self ) {
    uint8_t data[ 20 ];
    self->ReadRegister( self, _DRV_MPU9250_ACCEL_XOUT_H, data, 20 );
    self->Params.RawData.Accel.X = ( int16_t )( ( uint16_t )data[ 0 ] << 8 | data[ 1 ] );
    self->Params.RawData.Accel.Y = ( int16_t )( ( uint16_t )data[ 2 ] << 8 | data[ 3 ] );
    self->Params.RawData.Accel.Z = ( int16_t )( ( uint16_t )data[ 4 ] << 8 | data[ 5 ] );
    self->Params.RawData.Temp    = ( int16_t )( ( uint16_t )data[ 6 ] << 8 | data[ 7 ] );
    self->Params.RawData.Gyro.X  = ( int16_t )( ( uint16_t )data[ 8 ] << 8 | data[ 9 ] );
    self->Params.RawData.Gyro.Y  = ( int16_t )( ( uint16_t )data[ 10 ] << 8 | data[ 11 ] );
    self->Params.RawData.Gyro.Z  = ( int16_t )( ( uint16_t )data[ 12 ] << 8 | data[ 13 ] );
    self->Params.RawData.Mag.X   = ( int16_t )( ( uint16_t )data[ 15 ] << 8 | data[ 14 ] );
    self->Params.RawData.Mag.Y   = ( int16_t )( ( uint16_t )data[ 17 ] << 8 | data[ 16 ] );
    self->Params.RawData.Mag.Z   = ( int16_t )( ( uint16_t )data[ 19 ] << 8 | data[ 18 ] );
}

//************************************************************
//事件

static void Event_WakeOnMotion( DRV_MPU9250 *self ) {
    self->Params.WakeOnMotion_Flag = 1;
}
static void Event_FifoOverflow( DRV_MPU9250 *self ) {
    self->Params.FifoOverflow_Flag = 1;
}
static void Event_Fsync( DRV_MPU9250 *self ) {
    self->Params.Fsync_Flag = 1;
}
static void Event_RawDataReady( DRV_MPU9250 *self ) {
    self->Params.RawDataReady_Flag = 1;
    self->Updata( self );

    // printf( "Accel X:%4d Y:%4d Z:%4d  ", self->Params.RawData.Accel.X, self->Params.RawData.Accel.Y, self->Params.RawData.Accel.Z );
    // printf( "Gyro X:%4d Y:%4d Z:%4d  ", self->Params.RawData.Gyro.X, self->Params.RawData.Gyro.Y, self->Params.RawData.Gyro.Z );
    // printf( "Mag X:%4d Y:%4d Z:%4d  ", self->Params.RawData.Mag.X, self->Params.RawData.Mag.Y, self->Params.RawData.Mag.Z );
    // printf( "Temp %4d\n", self->Params.RawData.Temp );
}

/**
 * @brief  釋放物件記憶體
 * @note
 * @param  *self: this 指標
 * @retval None
 */
static void DRV_MPU9250Free( DRV_MPU9250 *self ) {
    _DRV_FREE( self );
}

/**
 * @brief  建立物件
 * @note
 * @param  *self: this 指標
 * @param  param : 物件參數結構
 * @retval 物件指標
 */
DRV_MPU9250 *DRV_MPU9250New( DRV_SPI *spi ) {

    //初始化
    DRV_MPU9250 *self = _DRV_MACCLOC( sizeof( DRV_MPU9250 ) );  //建立物件&分配記憶體
    self->Params.spi  = spi;

    //函示
    self->GetVersion      = DRV_MPU9250_GetVersion;
    self->Initialize      = Initialize;
    self->Deinitialize    = Deinitialize;
    self->ReadRegister    = ReadRegister;
    self->WriteRegister   = WriteRegister;
    self->SetRegisters    = SetRegisters;
    self->ClrRegisters    = ClrRegisters;
    self->I2cMstWriteByte = I2cMstWriteByte;
    self->I2cSlvDataRead  = I2cSlvDataRead;
    self->I2cMstRead      = I2cMstRead;
    self->Updata          = Updata;

    // 事件
    self->Event_WakeOnMotion = Event_WakeOnMotion;
    self->Event_FifoOverflow = Event_FifoOverflow;
    self->Event_Fsync        = Event_Fsync;
    self->Event_RawDataReady = Event_RawDataReady;

    //物件操作
    self->Free = DRV_MPU9250Free;

    return self;
}

/*
volatile uint8_t MPU_int_status = 0;

void MPU9250_init(void)
{
    MPU9250_reg_write_byte(USER_CTRL, 0x74);
    MPU9250_reg_write_byte(USER_CTRL, 0x72);
    MPU9250_reg_write_byte(USER_CTRL, 0x71);
    MPU9250_reg_write_byte(PWR_MGMT_2, 0x00);
    MPU9250_reg_write_byte(MPU_CONFIG, 0x03);
    MPU9250_reg_write_byte(SMPLRT_DIV, 0x18);
    MPU9250_reg_write_byte(GYRO_CONFIG, 0x18);
    MPU9250_reg_write_byte(ACCEL_CONFIG, 0x18);
    MPU9250_reg_write_byte(ACCEL_CONFIG2, 0x03);
    MPU9250_reg_write_byte(INT_PIN_CFG, 0xE0);
    MPU9250_reg_write_byte(INT_ENABLE, 0x01);
}

void MPU9250_set_offset(int16_t data[])
{
    int16_t	offset[6] = {0};
    uint8_t temp[2];
    uint8_t i;
    MPU9250_reg_read_arg(XA_OFFSET_H, 2, temp);
    offset[0] = (int16_t)((temp[0]<<8) | temp[1]);
    MPU9250_reg_read_arg(YA_OFFSET_H, 2, temp);
    offset[1] = (int16_t)((temp[0]<<8) | temp[1]);
    MPU9250_reg_read_arg(ZA_OFFSET_H, 2, temp);
    offset[2] = (int16_t)((temp[0]<<8) | temp[1]);
    MPU9250_reg_read_arg(XG_OFFSET_H, 2, temp);
    offset[3] = (int16_t)((temp[0]<<8) | temp[1]);
    MPU9250_reg_read_arg(YG_OFFSET_H, 2, temp);
    offset[4] = (int16_t)((temp[0]<<8) | temp[1]);
    MPU9250_reg_read_arg(ZG_OFFSET_H, 2, temp);
    offset[5] = (int16_t)((temp[0]<<8) | temp[1]);

    for(i=0;i<6;i++)
    {
        offset[i] -= data[i];
        offset[i] &= 0xFFFE;
    }
    MPU9250_reg_write_byte(XA_OFFSET_H, (uint8_t)((offset[0]>>8) & 0x00FF));
    MPU9250_reg_write_byte(XA_OFFSET_L, (uint8_t)((offset[0]>>0) & 0x00FF));
    MPU9250_reg_write_byte(YA_OFFSET_H, (uint8_t)((offset[1]>>8) & 0x00FF));
    MPU9250_reg_write_byte(YA_OFFSET_L, (uint8_t)((offset[1]>>0) & 0x00FF));
    MPU9250_reg_write_byte(ZA_OFFSET_H, (uint8_t)((offset[2]>>8) & 0x00FF));
    MPU9250_reg_write_byte(ZA_OFFSET_L, (uint8_t)((offset[2]>>0) & 0x00FF));
}

*/
