#ifndef __MPU9250_H__
#define __MPU9250_H__

#include "Driver_MCU_SPI.h"

// API版本號
#define DRIVER_MPU9250_API_VERSION DRIVER_VERSION_MAIJOR_MINOR( 1, 00 )

#define __DRV_MPU9250_PARAMS() \
    struct {                   \
        uint8_t  div;          \
        uint8_t  gyroLpf : 3;  \
        uint8_t  accelLpf : 3; \
        uint8_t  gyroFs : 2;   \
        uint8_t  accelFs : 2; \
}

//****************************************************************
//參數結構
typedef struct _DRV_MPU9250_PARAMS {
    __DRV_MPU9250_PARAMS();
} DRV_MPU9250_PARAMS;

//****************************************************************
//物件結構
typedef struct _DRV_MPU9250 {
    //參數
    struct {
        DRV_SPI *spi;

        volatile uint8_t WakeOnMotion_Flag : 1;
        volatile uint8_t FifoOverflow_Flag : 1;
        volatile uint8_t Fsync_Flag : 1;
        volatile uint8_t RawDataReady_Flag : 1;

        uint8_t SlvReadLength[ 4 ];

        struct {
            union {
                int16_t Array[ 3 ];
                struct {
                    int16_t X;
                    int16_t Y;
                    int16_t Z;
                };
            } Accel;

            union {
                int16_t Array[ 3 ];
                struct {
                    int16_t X;
                    int16_t Y;
                    int16_t Z;
                };
            } Gyro;

            union {
                int16_t Array[ 3 ];
                struct {
                    int16_t X;
                    int16_t Y;
                    int16_t Z;
                };
            } Mag;

            int16_t Temp;

        } RawData;  // RawData
    } Params;

    // 函式
    DRIVER_VERSION ( *GetVersion )( void );
    void ( *Initialize )( struct _DRV_MPU9250 *self , DRV_MPU9250_PARAMS initParams);
    void ( *Deinitialize )( struct _DRV_MPU9250 *self );
    uint8_t ( *ReadRegister )( struct _DRV_MPU9250 *self, uint8_t regAddr, uint8_t *data, uint32_t len );
    void ( *WriteRegister )( struct _DRV_MPU9250 *self, uint8_t regAddr, uint8_t *data, uint32_t len );
    uint8_t ( *SetRegisters )( struct _DRV_MPU9250 *self, uint8_t regAddr, uint8_t regMsk );
    uint8_t ( *ClrRegisters )( struct _DRV_MPU9250 *self, uint8_t regAddr, uint8_t regMsk );
    void ( *I2cMstWriteByte )( struct _DRV_MPU9250 *self, uint8_t i2cSlvAddr, uint8_t i2cAddr, uint8_t regAddr, uint8_t data );
    void ( *I2cSlvDataRead )( struct _DRV_MPU9250 *self, uint8_t slv, uint8_t *data, uint8_t len );
    void ( *I2cMstRead )( struct _DRV_MPU9250 *self, uint8_t slv, uint8_t i2cAddr, uint8_t regAddr, uint8_t *data, uint8_t len );
    void ( *Updata )( struct _DRV_MPU9250 *self );

    // 事件
    void ( *Event_WakeOnMotion )( struct _DRV_MPU9250 *self );
    void ( *Event_FifoOverflow )( struct _DRV_MPU9250 *self );
    void ( *Event_Fsync )( struct _DRV_MPU9250 *self );
    void ( *Event_RawDataReady )( struct _DRV_MPU9250 *self );

    //物件操作
    void ( *Free )( struct _DRV_MPU9250 *self );
} DRV_MPU9250;

//****************************************************************
//對外函式
DRV_MPU9250 *  DRV_MPU9250New( DRV_SPI * spi );
DRIVER_VERSION DRV_MPU9250_GetVersion( void );

//****************************************************************
//預處理函示
/**
 * @brief  讀取一個Byte
 * @note
 * @param  regAddr: 暫存器地址
 * @retval 讀取資料
 */
#define DRV_MPU9250_READ_BYTE( self, regAddr ) self->ReadRegister( self, regAddr, NULL, 1 )

/**
 * @brief  寫入一個Byte
 * @note
 * @param  regAddr: 暫存器地址
 * @param data : 寫入資料
 * @retval None
 */
#define DRV_MPU9250_WRITE_BYTE( self, regAddr, data ) self->WriteRegister( self, regAddr, ( uint8_t[] ){ data }, 1 )

/**
 * @brief  Reset
 * @note
 * @retval _DRV_MPU9250_PWR_MGMT_1
 */
#define DRV_MPU9250_RESET( self ) self->SetRegisters( mpu9250, _DRV_MPU9250_PWR_MGMT_1, _DRV_MPU9250_PWR_MGMT_1_H_RESET_Msk );

/**
 * @brief  取得中斷狀態
 * @note
 * @retval 中斷狀態
 */
#define DRV_MPU9250_GET_INT_STATUS( self ) DRV_MPU9250_READ_BYTE( self, _DRV_MPU9250_INT_STATUS )

/**
 * @brief  開啟中斷
 * @note
 * @param  regMsk: 資料遮罩
 * @retval _DRV_MPU9250_INT_ENABLE
 */
#define DRV_MPU9250_INT_ENABLE( self, regMsk ) self->SetRegisters( self, _DRV_MPU9250_INT_ENABLE, regMsk )

/**
 * @brief  關閉中斷
 * @note
 * @param  regMsk: 資料遮罩
 * @retval _DRV_MPU9250_INT_ENABLE
 */
#define DRV_MPU9250_INT_DISABLE( self, regMsk ) self->ClrRegisters( self, _DRV_MPU9250_INT_ENABLE, ~( regMsk ) )

/**
 * @brief  設定取樣頻率分頻
 * @note   SAMPLE_RATE = Internal_Sample_Rate / (1 + SMPLRT_DIV)
 * @retval None
 */
#define DRV_MPU9250_SAMPLE_DIV( self, DIV ) DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_SMPLRT_DIV, DIV )

/**
 * @brief  設定陀螺儀及溫度低通濾波器
 * @note
 * @retval None
 */
#define DRV_MPU9250_SET_GYRO_DLPF_CFG( self, cfg ) DRV_MPU9250_WRITE_BYTE( self, _DRV_MPU9250_MPU_CONFIG, ( cfg << _DRV_MPU9250_MPU_CONFIG_DLPF_CFG_Pos ) & _DRV_MPU9250_MPU_CONFIG_DLPF_CFG_Msk )

/**
 * @brief  關閉低通濾波器
 * @note   模式1
 * @retval _DRV_MPU9250_GYRO_CONFIG
 */
#define DRV_MPU9250_GYRO_DLPF_DISABLE_1( self ) self->SetRegisters( self, _DRV_MPU9250_GYRO_CONFIG, 0x01 )

/**
 * @brief  關閉低通濾波器
 * @note   模式2
 * @retval _DRV_MPU9250_GYRO_CONFIG
 */
#define DRV_MPU9250_GYRO_DLPF_DISABLE_2( self ) self->SetRegisters( self, _DRV_MPU9250_GYRO_CONFIG, 0x02 )

/**
 * @brief  開啟低通濾波器
 * @note
 * @retval _DRV_MPU9250_GYRO_CONFIG
 */
#define DRV_MPU9250_GYRO_DLPF_ENABLE( self ) self->ClrRegisters( self, _DRV_MPU9250_GYRO_CONFIG, ~0x03 )

/**
 * @brief   設定陀螺儀量程
 * @note
 * @param fs    量程設定
 *              DRV_MPU9250_GYRO_FS(value) 0:250dps ; 1:500dps ; 2:1000dps ; 3:2000dps
 * @retval _DRV_MPU9250_GYRO_CONFIG
 */
#define DRV_MPU9250_SET_GYRO_FS( self, fs ) self->SetRegisters( self, _DRV_MPU9250_GYRO_CONFIG, fs )
#define DRV_MPU9250_GYRO_FS( fs )           ( ( fs << _DRV_MPU9250_GYRO_CONFIG_GYRO_FS_SEL_Pos ) & _DRV_MPU9250_GYRO_CONFIG_GYRO_FS_SEL_Msk )

/**
 * @brief  設定加速度儀量程
 * @note
 * @param fs    量程設定
 *              DRV_MPU9250_ACCEL_FS(value) 0:2g ; 1:4g ; 2:8g ; 3:16g
 * @retval _DRV_MPU9250_ACCEL_CONFIG
 */
#define DRV_MPU9250_SET_ACCEL_FS( self, fs ) self->SetRegisters( self, _DRV_MPU9250_ACCEL_CONFIG, fs )
#define DRV_MPU9250_ACCEL_FS( fs )           ( ( fs << _DRV_MPU9250_ACCEL_CONFIG_ACCEL_FS_SEL_Pos ) & _DRV_MPU9250_ACCEL_CONFIG_ACCEL_FS_SEL_Msk )

/**
 * @brief  關閉加速度儀低通濾波器
 * @note
 * @retval
 */
#define DRV_MPU9250_ACCEL_DLPF_DISABLE( self ) self->SetRegisters( self, _DRV_MPU9250_ACCEL_CONFIG2, 0x1 << _DRV_MPU9250_ACCEL_CONFIG2_ACCEL_FCHOICE_B_Pos )

/**
 * @brief  開啟加速度儀低通濾波器
 * @note
 * @retval
 */
#define DRV_MPU9250_ACCEL_DLPF_ENABLE( self ) self->ClrRegisters( self, _DRV_MPU9250_ACCEL_CONFIG2, ~( 0x1 << _DRV_MPU9250_ACCEL_CONFIG2_ACCEL_FCHOICE_B_Pos ) )

/**
 * @brief  設定加速度儀低通濾波器
 * @note
 * @retval None
 */
#define DRV_MPU9250_ACCEL_SET_CFG( self, cfg ) self->SetRegisters( self, _DRV_MPU9250_ACCEL_CONFIG2, ( cfg << _DRV_MPU9250_MPU_CONFIG_DLPF_CFG_Pos ) & _DRV_MPU9250_MPU_CONFIG_DLPF_CFG_Msk )

/**
 * @brief  取得全部原始資料
 * @note   ACCEL_X_H | ACCEL_X_Ｌ | ACCEL_Y_H | ACCEL_Y_Ｌ | ACCEL_Z_H | ACCEL_Z_Ｌ |
 *         TEMP_H | TEMP_L |
 *         GYRO_X_H | GYRO_X_L | GYRO_Y_H | GYRO_Y_L | GYRO_Z_H | GYRO_Z_L
 * @param  data : 存放地址
 * @retval ACCEL_X_H
 */
#define DRV_MPU9250_GET_ALL_RAW_DATA( self, data ) self->ReadRegister( self, _DRV_MPU9250_ACCEL_XOUT_H, data, 14 )

/**
 * @brief  取得加速度儀原始資料
 * @note   ACCEL_X_H | ACCEL_X_Ｌ | ACCEL_Y_H | ACCEL_Y_Ｌ | ACCEL_Z_H | ACCEL_Z_Ｌ |
 * @param  data : 存放地址
 * @retval ACCEL_X_H
 */
#define DRV_MPU9250_GET_ACCEL_RAW_DATA( self, data ) self->ReadRegister( self, _DRV_MPU9250_ACCEL_XOUT_H, data, 6 )

/**
 * @brief  取得溫度原始資料
 * @note   TEMP_H | TEMP_L
 * @param  data : 存放地址
 * @retval TEMP_H
 */
#define DRV_MPU9250_GET_TEMP_RAW_DATA( self, data ) self->ReadRegister( self, _DRV_MPU9250_TEMP_OUT_H, data, 2 )

/**
 * @brief  取得陀螺儀原始資料
 * @note   GYRO_X_H | GYRO_X_L | GYRO_Y_H | GYRO_Y_L | GYRO_Z_H | GYRO_Z_L
 * @param  data : 存放地址
 * @retval GYRO_X_H
 */
#define DRV_MPU9250_GET_GYRO_RAW_DATA( self, data ) self->ReadRegister( self, _DRV_MPU9250_GYRO_XOUT_H, data, 6 )

/**
 * @brief  選擇PLL時鐘源
 * @note
 * @retval _DRV_MPU9250_PWR_MGMT_1
 */
#define DRV_MPU9250_USE_PLL( self ) self->SetRegisters( self, _DRV_MPU9250_PWR_MGMT_1, 0x01 << _DRV_MPU9250_PWR_MGMT_1_CLKSEL_Pos )

/**
 * @brief  開啟感測器FIFO
 * @note
 * @param dataMsk : 資料遮罩
 * @retval _DRV_MPU9250_FIFO_EN
 */
#define DRV_MPU9250_FIFO_SEN_ENABLE( self, dataMsk ) self->SetRegisters( self, _DRV_MPU9250_FIFO_EN, dataMsk )

/**
 * @brief  讀取FIFO內有效字節數量
 * @note   13bit
 * @param cnt : 存放地址
 * @retval _DRV_MPU9250_FIFO_COUNTH
 */
#define DRV_MPU9250_GET_FIFO_CNT( self, cnt ) self->ReadRegister( self, _DRV_MPU9250_FIFO_COUNTH, cnt, 2 )

/**
 * @brief  啟動FIFO
 * @note
 * @retval _DRV_MPU9250_USER_CTRL
 */
#define DRV_MPU9250_FIFO_ENABLE( self ) self->SetRegisters( self, _DRV_MPU9250_USER_CTRL, _DRV_MPU9250_USER_CTRL_FIFO_EN_Msk );

/**
 * @brief  Reset FIFO
 * @note
 * @retval _DRV_MPU9250_USER_CTRL
 */
#define DRV_MPU9250_FIFO_RST( self ) self->SetRegisters( self, _DRV_MPU9250_USER_CTRL, _DRV_MPU9250_USER_CTRL_FIFO_RST_Msk );

//**************************************************************************************************************
//暫存器位址

#define _DRV_MPU9250_READ_REG  0x80
#define _DRV_MPU9250_WRITE_REG 0x00

#define _DRV_AK8963_ADDRESS  0x0C
#define _DRV_AK8963_WHO_AM_I 0x00  // should return 0x48
#define _DRV_AK8963_INFO     0x01
#define _DRV_AK8963_ST1      0x02  // data ready status bit 0
#define _DRV_AK8963_XOUT_L   0x03  // data
#define _DRV_AK8963_XOUT_H   0x04
#define _DRV_AK8963_YOUT_L   0x05
#define _DRV_AK8963_YOUT_H   0x06
#define _DRV_AK8963_ZOUT_L   0x07
#define _DRV_AK8963_ZOUT_H   0x08
#define _DRV_AK8963_ST2      0x09  // Data overflow bit 3 and data read error status bit 2
#define _DRV_AK8963_CNTL     0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define _DRV_AK8963_ASTC     0x0C  // Self test control
#define _DRV_AK8963_I2CDIS   0x0F  // I2C disable
#define _DRV_AK8963_ASAX     0x10  // Fuse ROM x-axis sensitivity adjustment value
#define _DRV_AK8963_ASAY     0x11  // Fuse ROM y-axis sensitivity adjustment value
#define _DRV_AK8963_ASAZ     0x12  // Fuse ROM z-axis sensitivity adjustment value

#define _DRV_MPU9250_SELF_TEST_X_GYRO 0x00
#define _DRV_MPU9250_SELF_TEST_Y_GYRO 0x01
#define _DRV_MPU9250_SELF_TEST_Z_GYRO 0x02

// #define X_FINE_GAIN      0x03 // [7:0] fine gain
// #define Y_FINE_GAIN      0x04
// #define Z_FINE_GAIN      0x05
// #define XA_OFFSET_H      0x06 // User-defined trim values for accelerometer
// #define XA_OFFSET_L_TC   0x07
// #define YA_OFFSET_H      0x08
// #define YA_OFFSET_L_TC   0x09
// #define ZA_OFFSET_H      0x0A
// #define ZA_OFFSET_L_TC   0x0B

#define _DRV_MPU9250_SELF_TEST_X_ACCEL 0x0D
#define _DRV_MPU9250_SELF_TEST_Y_ACCEL 0x0E
#define _DRV_MPU9250_SELF_TEST_Z_ACCEL 0x0F

#define _DRV_MPU9250_SELF_TEST_A 0x10

#define _DRV_MPU9250_XG_OFFSET_H 0x13  // User-defined trim values for gyroscope
#define _DRV_MPU9250_XG_OFFSET_L 0x14
#define _DRV_MPU9250_YG_OFFSET_H 0x15
#define _DRV_MPU9250_YG_OFFSET_L 0x16
#define _DRV_MPU9250_ZG_OFFSET_H 0x17
#define _DRV_MPU9250_ZG_OFFSET_L 0x18

#define _DRV_MPU9250_SMPLRT_DIV 0x19

#define _DRV_MPU9250_MPU_CONFIG                  0x1A
#define _DRV_MPU9250_MPU_CONFIG_FIFO_MODE_Pos    ( 6 )
#define _DRV_MPU9250_MPU_CONFIG_FIFO_MODE_Msk    0x1ul << _DRV_MPU9250_MPU_CONFIG_FIFO_MODE_Pos
#define _DRV_MPU9250_MPU_CONFIG_EXT_SYNC_SET_Pos ( 3 )
#define _DRV_MPU9250_MPU_CONFIG_EXT_SYNC_SET_Msk 0x7ul << _DRV_MPU9250_MPU_CONFIG_EXT_SYNC_SET_Pos
#define _DRV_MPU9250_MPU_CONFIG_DLPF_CFG_Pos     ( 0 )
#define _DRV_MPU9250_MPU_CONFIG_DLPF_CFG_Msk     0x7ul << _DRV_MPU9250_MPU_CONFIG_DLPF_CFG_Pos

#define _DRV_MPU9250_GYRO_CONFIG                 0x1B
#define _DRV_MPU9250_GYRO_CONFIG_XGYRO_CTEN_Pos  ( 7 )
#define _DRV_MPU9250_GYRO_CONFIG_XGYRO_CTEN_Msk  0x1ul << _DRV_MPU9250_GYRO_CONFIG_XGYRO_CTEN_Pos
#define _DRV_MPU9250_GYRO_CONFIG_YGYRO_CTEN_Pos  ( 6 )
#define _DRV_MPU9250_GYRO_CONFIG_YGYRO_CTEN_Msk  0x1ul << _DRV_MPU9250_GYRO_CONFIG_YGYRO_CTEN_Pos
#define _DRV_MPU9250_GYRO_CONFIG_ZGYRO_CTEN_Pos  ( 5 )
#define _DRV_MPU9250_GYRO_CONFIG_ZGYRO_CTEN_Msk  0x1ul << _DRV_MPU9250_GYRO_CONFIG_ZGYRO_CTEN_Pos
#define _DRV_MPU9250_GYRO_CONFIG_GYRO_FS_SEL_Pos ( 3 )
#define _DRV_MPU9250_GYRO_CONFIG_GYRO_FS_SEL_Msk 0x3ul << _DRV_MPU9250_GYRO_CONFIG_GYRO_FS_SEL_Pos
#define _DRV_MPU9250_GYRO_CONFIG_FCHOICE_B_Pos   ( 0 )
#define _DRV_MPU9250_GYRO_CONFIG_FCHOICE_B_Msk   0x3ul << _DRV_MPU9250_GYRO_CONFIG_FCHOICE_B_Pos

#define _DRV_MPU9250_ACCEL_CONFIG                  0x1C
#define _DRV_MPU9250_ACCEL_CONFIG_AX_ST_EN_Pos     ( 7 )
#define _DRV_MPU9250_ACCEL_CONFIG_AX_ST_EN_Msk     0x1ul << _DRV_MPU9250_ACCEL_CONFIG_AX_ST_EN_Pos
#define _DRV_MPU9250_ACCEL_CONFIG_AY_ST_EN_Pos     ( 6 )
#define _DRV_MPU9250_ACCEL_CONFIG_AY_ST_EN_Msk     0x1ul << _DRV_MPU9250_ACCEL_CONFIG_AY_ST_EN_Pos
#define _DRV_MPU9250_ACCEL_CONFIG_AZ_ST_EN_Pos     ( 5 )
#define _DRV_MPU9250_ACCEL_CONFIG_AZ_ST_EN_Msk     0x1ul << _DRV_MPU9250_ACCEL_CONFIG_AZ_ST_EN_Pos
#define _DRV_MPU9250_ACCEL_CONFIG_ACCEL_FS_SEL_Pos ( 3 )
#define _DRV_MPU9250_ACCEL_CONFIG_ACCEL_FS_SEL_Msk 0x3ul << _DRV_MPU9250_ACCEL_CONFIG_ACCEL_FS_SEL_Pos

#define _DRV_MPU9250_ACCEL_CONFIG2                     0x1D
#define _DRV_MPU9250_ACCEL_CONFIG2_ACCEL_FCHOICE_B_Pos ( 3 )
#define _DRV_MPU9250_ACCEL_CONFIG2_ACCEL_FCHOICE_B_Msk 0x1ul << _DRV_MPU9250_ACCEL_CONFIG2_ACCEL_FCHOICE_B_Pos
#define _DRV_MPU9250_ACCEL_CONFIG2_A_DLPFCFG_Pos       ( 0 )
#define _DRV_MPU9250_ACCEL_CONFIG2_A_DLPFCFG_Msk       0x3ul << _DRV_MPU9250_ACCEL_CONFIG2_A_DLPFCFG_Pos

#define _DRV_MPU9250_LP_ACCEL_ODR                  0x1E
#define _DRV_MPU9250_LP_ACCEL_ODR_LPOSC_CLKSEL_Pos ( 0 )
#define _DRV_MPU9250_LP_ACCEL_ODR_LPOSC_CLKSEL_Msk 0x7ul << _DRV_MPU9250_LP_ACCEL_ODR_LPOSC_CLKSEL_Pos

#define _DRV_MPU9250_WOM_THR   0x1F
#define _DRV_MPU9250_MOT_DUR   0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define _DRV_MPU9250_ZMOT_THR  0x21  // Zero-motion detection threshold bits [7:0]
#define _DRV_MPU9250_ZRMOT_DUR 0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define _DRV_MPU9250_FIFO_EN               0x23
#define _DRV_MPU9250_FIFO_EN_TEMP_OUT_Pos  ( 7 )
#define _DRV_MPU9250_FIFO_EN_TEMP_OUT_Msk  0x1ul << _DRV_MPU9250_FIFO_EN_TEMP_OUT_Pos
#define _DRV_MPU9250_FIFO_EN_GYRO_XOUT_Pos ( 6 )
#define _DRV_MPU9250_FIFO_EN_GYRO_XOUT_Msk 0x1ul << _DRV_MPU9250_FIFO_EN_GYRO_XOUT_Pos
#define _DRV_MPU9250_FIFO_EN_GYRO_YOUT_Pos ( 5 )
#define _DRV_MPU9250_FIFO_EN_GYRO_YOUT_Msk 0x1ul << _DRV_MPU9250_FIFO_EN_GYRO_YOUT_Pos
#define _DRV_MPU9250_FIFO_EN_GYRO_ZOUT_Pos ( 4 )
#define _DRV_MPU9250_FIFO_EN_GYRO_ZOUT_Msk 0x1ul << _DRV_MPU9250_FIFO_EN_GYRO_ZOUT_Pos
#define _DRV_MPU9250_FIFO_EN_ACCEL_Pos     ( 3 )
#define _DRV_MPU9250_FIFO_EN_ACCEL_Msk     0x1ul << _DRV_MPU9250_FIFO_EN_ACCEL_Pos
#define _DRV_MPU9250_FIFO_EN_SLV_2_Pos     ( 2 )
#define _DRV_MPU9250_FIFO_EN_SLV_2_Msk     0x1ul << _DRV_MPU9250_FIFO_EN_SLV_2_Pos
#define _DRV_MPU9250_FIFO_EN_SLV_1_Pos     ( 1 )
#define _DRV_MPU9250_FIFO_EN_SLV_1_Msk     0x1ul << _DRV_MPU9250_FIFO_EN_SLV_1_Pos
#define _DRV_MPU9250_FIFO_EN_SLV_0_Pos     ( 0 )
#define _DRV_MPU9250_FIFO_EN_SLV_0_Msk     0x1ul << _DRV_MPU9250_FIFO_EN_SLV_0_Pos

#define _DRV_MPU9250_I2C_MST_CTRL                   0x24
#define _DRV_MPU9250_I2C_MST_CTRL_MULT_MST_EN_Pos   ( 7 )
#define _DRV_MPU9250_I2C_MST_CTRL_MULT_MST_EN_Msk   0x1ul << _DRV_MPU9250_I2C_MST_CTRL_MULT_MST_EN_Pos
#define _DRV_MPU9250_I2C_MST_CTRL_WAIT_FOR_ES_Pos   ( 6 )
#define _DRV_MPU9250_I2C_MST_CTRL_WAIT_FOR_ES_Msk   0x1ul << _DRV_MPU9250_I2C_MST_CTRL_WAIT_FOR_ES_Pos
#define _DRV_MPU9250_I2C_MST_CTRL_SLV_3_FIFO_EN_Pos ( 5 )
#define _DRV_MPU9250_I2C_MST_CTRL_SLV_3_FIFO_EN_Msk 0x1ul << _DRV_MPU9250_I2C_MST_CTRL_SLV_3_FIFO_EN_Pos
#define _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_P_NSR_Pos ( 4 )
#define _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_P_NSR_Msk 0x1ul << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_P_NSR_Pos
#define _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos   ( 0 )
#define _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk   0xFul << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos

#define _DRV_MPU9250_I2C_MST_CLK_348K ( ( 0 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_333K ( ( 1 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_320K ( ( 2 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_308K ( ( 3 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_296K ( ( 4 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_286K ( ( 5 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_276K ( ( 6 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_267K ( ( 7 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_258K ( ( 8 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_500K ( ( 9 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_471K ( ( 10 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_444K ( ( 11 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_421K ( ( 12 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_400K ( ( 13 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_381K ( ( 14 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk
#define _DRV_MPU9250_I2C_MST_CLK_364K ( ( 15 ) << _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Pos ) & _DRV_MPU9250_I2C_MST_CTRL_I2C_MST_CLK_Msk

#define _DRV_MPU9250_I2C_MST_READ  0x80
#define _DRV_MPU9250_I2C_MST_WRITE 0x00

#define _DRV_MPU9250_I2C_SLV0 0
#define _DRV_MPU9250_I2C_SLV1 1
#define _DRV_MPU9250_I2C_SLV2 2
#define _DRV_MPU9250_I2C_SLV3 3
#define _DRV_MPU9250_I2C_SLV4 4

#define _DRV_MPU9250_I2C_SLV_ENABLE 0x80

#define _DRV_MPU9250_I2C_SLV_BASE( slv ) ( _DRV_MPU9250_I2C_SLV0_ADDR + ( slv * 3 ) )

#define _DRV_MPU9250_I2C_SLV0_ADDR                  0x25
#define _DRV_MPU9250_I2C_SLV0_ADDR_I2C_SLV0_RNW_Pos ( 7 )
#define _DRV_MPU9250_I2C_SLV0_ADDR_I2C_SLV0_RNW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV0_ADDR_I2C_SLV0_RNW_Pos
#define _DRV_MPU9250_I2C_SLV0_ADDR_I2C_ID_0_Pos     ( 0 )
#define _DRV_MPU9250_I2C_SLV0_ADDR_I2C_ID_0_Msk     0x7Ful << _DRV_MPU9250_I2C_SLV0_ADDR_I2C_ID_0_Pos

#define _DRV_MPU9250_I2C_SLV0_REG 0x26

#define _DRV_MPU9250_I2C_SLV0_CTRL                      0x27
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_EN_Pos      ( 7 )
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_EN_Msk      0x1ul << _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_EN_Pos
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_BYTE_SW_Pos ( 6 )
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_BYTE_SW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_BYTE_SW_Pos
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_REG_DIS_Pos ( 5 )
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_REG_DIS_Msk 0x1ul << _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_REG_DIS_Pos
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_GRP_Pos     ( 4 )
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_GRP_Msk     0x1ul << _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_GRP_Pos
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_LENG_Pos    ( 0 )
#define _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_LENG_Msk    0xFul << _DRV_MPU9250_I2C_SLV0_CTRL_I2C_SLV0_LENG_Pos

#define _DRV_MPU9250_I2C_SLV1_ADDR                  0x28
#define _DRV_MPU9250_I2C_SLV1_ADDR_I2C_SLV1_RNW_Pos ( 7 )
#define _DRV_MPU9250_I2C_SLV1_ADDR_I2C_SLV1_RNW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV1_ADDR_I2C_SLV1_RNW_Pos
#define _DRV_MPU9250_I2C_SLV1_ADDR_I2C_ID_1_Pos     ( 0 )
#define _DRV_MPU9250_I2C_SLV1_ADDR_I2C_ID_1_Msk     0x7Ful << _DRV_MPU9250_I2C_SLV1_ADDR_I2C_ID_1_Pos

#define _DRV_MPU9250_I2C_SLV1_REG 0x29

#define _DRV_MPU9250_I2C_SLV1_CTRL                      0x2A
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_EN_Pos      ( 7 )
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_EN_Msk      0x1ul << _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_EN_Pos
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_BYTE_SW_Pos ( 6 )
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_BYTE_SW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_BYTE_SW_Pos
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_REG_DIS_Pos ( 5 )
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_REG_DIS_Msk 0x1ul << _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_REG_DIS_Pos
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_GRP_Pos     ( 4 )
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_GRP_Msk     0x1ul << _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_GRP_Pos
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_LENG_Pos    ( 0 )
#define _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_LENG_Msk    0xFul << _DRV_MPU9250_I2C_SLV1_CTRL_I2C_SLV1_LENG_Pos

#define _DRV_MPU9250_I2C_SLV2_ADDR                  0x2B
#define _DRV_MPU9250_I2C_SLV2_ADDR_I2C_SLV2_RNW_Pos ( 7 )
#define _DRV_MPU9250_I2C_SLV2_ADDR_I2C_SLV2_RNW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV2_ADDR_I2C_SLV2_RNW_Pos
#define _DRV_MPU9250_I2C_SLV2_ADDR_I2C_ID_2_Pos     ( 0 )
#define _DRV_MPU9250_I2C_SLV2_ADDR_I2C_ID_2_Msk     0x7Ful << _DRV_MPU9250_I2C_SLV2_ADDR_I2C_ID_2_Pos

#define _DRV_MPU9250_I2C_SLV2_REG 0x2C

#define _DRV_MPU9250_I2C_SLV2_CTRL                      0x2D
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_EN_Pos      ( 7 )
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_EN_Msk      0x1ul << _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_EN_Pos
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_BYTE_SW_Pos ( 6 )
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_BYTE_SW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_BYTE_SW_Pos
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_REG_DIS_Pos ( 5 )
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_REG_DIS_Msk 0x1ul << _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_REG_DIS_Pos
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_GRP_Pos     ( 4 )
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_GRP_Msk     0x1ul << _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_GRP_Pos
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_LENG_Pos    ( 0 )
#define _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_LENG_Msk    0xFul << _DRV_MPU9250_I2C_SLV2_CTRL_I2C_SLV2_LENG_Pos

#define _DRV_MPU9250_I2C_SLV3_ADDR                  0x2E
#define _DRV_MPU9250_I2C_SLV3_ADDR_I2C_SLV3_RNW_Pos ( 7 )
#define _DRV_MPU9250_I2C_SLV3_ADDR_I2C_SLV3_RNW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV3_ADDR_I2C_SLV3_RNW_Pos
#define _DRV_MPU9250_I2C_SLV3_ADDR_I2C_ID_3_Pos     ( 0 )
#define _DRV_MPU9250_I2C_SLV3_ADDR_I2C_ID_3_Msk     0x7Ful << _DRV_MPU9250_I2C_SLV3_ADDR_I2C_ID_3_Pos

#define _DRV_MPU9250_I2C_SLV3_REG 0x2F

#define _DRV_MPU9250_I2C_SLV3_CTRL                      0x30
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_EN_Pos      ( 7 )
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_EN_Msk      0x1ul << _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_EN_Pos
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_BYTE_SW_Pos ( 6 )
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_BYTE_SW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_BYTE_SW_Pos
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_REG_DIS_Pos ( 5 )
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_REG_DIS_Msk 0x1ul << _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_REG_DIS_Pos
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_GRP_Pos     ( 4 )
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_GRP_Msk     0x1ul << _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_GRP_Pos
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_LENG_Pos    ( 0 )
#define _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_LENG_Msk    0xFul << _DRV_MPU9250_I2C_SLV3_CTRL_I2C_SLV3_LENG_Pos

#define _DRV_MPU9250_I2C_SLV4_ADDR                  0x31
#define _DRV_MPU9250_I2C_SLV4_ADDR_I2C_SLV4_RNW_Pos ( 7 )
#define _DRV_MPU9250_I2C_SLV4_ADDR_I2C_SLV4_RNW_Msk 0x1ul << _DRV_MPU9250_I2C_SLV4_ADDR_I2C_SLV4_RNW_Pos
#define _DRV_MPU9250_I2C_SLV4_ADDR_I2C_ID_4_Pos     ( 0 )
#define _DRV_MPU9250_I2C_SLV4_ADDR_I2C_ID_4_Msk     0x7Ful << _DRV_MPU9250_I2C_SLV4_ADDR_I2C_ID_0_Pos

#define _DRV_MPU9250_I2C_SLV4_REG 0x32

#define _DRV_MPU9250_I2C_SLV4_DO 0x33

#define _DRV_MPU9250_I2C_SLV4_CTRL                      0x34
#define _DRV_MPU9250_I2C_SLV4_CTRL_I2C_SLV4_EN_Pos      ( 7 )
#define _DRV_MPU9250_I2C_SLV4_CTRL_I2C_SLV4_EN_Msk      0x1ul << _DRV_MPU9250_I2C_SLV4_CTRL_I2C_SLV4_EN_Pos
#define _DRV_MPU9250_I2C_SLV4_CTRL_SLV4_DONE_INT_EN_Pos ( 6 )
#define _DRV_MPU9250_I2C_SLV4_CTRL_SLV4_DONE_INT_EN_Msk 0x1ul << _DRV_MPU9250_I2C_SLV4_CTRL_SLV4_DONE_INT_EN_Pos
#define _DRV_MPU9250_I2C_SLV4_CTRL_I2C_SLV4_REG_DIS_Pos ( 5 )
#define _DRV_MPU9250_I2C_SLV4_CTRL_I2C_SLV4_REG_DIS_Msk 0x1ul << _DRV_MPU9250_I2C_SLV4_CTRL_I2C_SLV4_REG_DIS_Pos
#define _DRV_MPU9250_I2C_SLV4_CTRL_I2C_MST_DLY_Pos      ( 0 )
#define _DRV_MPU9250_I2C_SLV4_CTRL_I2C_MST_DLY_Msk      0x1Ful << _DRV_MPU9250_I2C_SLV4_CTRL_I2C_MST_DLY_Pos

#define _DRV_MPU9250_I2C_SLV4_DI 0x35

#define _DRV_MPU9250_I2C_MST_STATUS                   0x36
#define _DRV_MPU9250_I2C_MST_STATUS_PASS_THROUGH_Pos  ( 7 )
#define _DRV_MPU9250_I2C_MST_STATUS_PASS_THROUGH_Msk  0x1ul << _DRV_MPU9250_I2C_MST_STATUS_PASS_THROUGH_Pos
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV4_DONE_Pos ( 6 )
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV4_DONE_Msk 0x1ul << _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV4_DONE_Pos
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_LOST_ARB_Pos  ( 5 )
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_LOST_ARB_Msk  0x1ul << _DRV_MPU9250_I2C_MST_STATUS_I2C_LOST_ARB_Pos
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV4_NACK_Pos ( 4 )
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV4_NACK_Msk 0x1ul << _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV4_NACK_Pos
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV3_NACK_Pos ( 3 )
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV3_NACK_Msk 0x1ul << _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV3_NACK_Pos
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV2_NACK_Pos ( 2 )
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV2_NACK_Msk 0x1ul << _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV2_NACK_Pos
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV1_NACK_Pos ( 1 )
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV1_NACK_Msk 0x1ul << _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV1_NACK_Pos
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV0_NACK_Pos ( 0 )
#define _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV0_NACK_Msk 0x1ul << _DRV_MPU9250_I2C_MST_STATUS_I2C_SLV0_NACK_Pos

#define _DRV_MPU9250_INT_PIN_CFG                       0x37
#define _DRV_MPU9250_INT_PIN_CFG_ACTL_Pos              ( 7 )
#define _DRV_MPU9250_INT_PIN_CFG_ACTL_Msk              0x1ul << _DRV_MPU9250_INT_PIN_CFG_ACTL_Pos
#define _DRV_MPU9250_INT_PIN_CFG_OPEN_Pos              ( 6 )
#define _DRV_MPU9250_INT_PIN_CFG_OPEN_Msk              0x1ul << _DRV_MPU9250_INT_PIN_CFG_OPEN_Pos
#define _DRV_MPU9250_INT_PIN_CFG_LATCH_INT_EN_Pos      ( 5 )
#define _DRV_MPU9250_INT_PIN_CFG_LATCH_INT_EN_Msk      0x1ul << _DRV_MPU9250_INT_PIN_CFG_LATCH_INT_EN_Pos
#define _DRV_MPU9250_INT_PIN_CFG_INT_ANYRD_2CLEAR_Pos  ( 4 )
#define _DRV_MPU9250_INT_PIN_CFG_INT_ANYRD_2CLEAR_Msk  0x1ul << _DRV_MPU9250_INT_PIN_CFG_INT_ANYRD_2CLEAR_Pos
#define _DRV_MPU9250_INT_PIN_CFG_ACTL_FSYNC_Pos        ( 3 )
#define _DRV_MPU9250_INT_PIN_CFG_ACTL_FSYNC_Msk        0x1ul << _DRV_MPU9250_INT_PIN_CFG_ACTL_FSYNC_Pos
#define _DRV_MPU9250_INT_PIN_CFG_FSYNC_INT_MODE_EN_Pos ( 2 )
#define _DRV_MPU9250_INT_PIN_CFG_FSYNC_INT_MODE_EN_Msk 0x1ul << _DRV_MPU9250_INT_PIN_CFG_FSYNC_INT_MODE_EN_Pos
#define _DRV_MPU9250_INT_PIN_CFG_BYPASS_EN_Pos         ( 1 )
#define _DRV_MPU9250_INT_PIN_CFG_BYPASS_EN_Msk         0x1ul << _DRV_MPU9250_INT_PIN_CFG_BYPASS_EN_Pos

#define _DRV_MPU9250_INT_ENABLE                      0x38
#define _DRV_MPU9250_INT_ENABLE_WOM_EN_Pos           ( 6 )
#define _DRV_MPU9250_INT_ENABLE_WOM_EN_Msk           0x1ul << _DRV_MPU9250_INT_ENABLE_WOM_EN_Pos
#define _DRV_MPU9250_INT_ENABLE_FIFO_OVERFLOW_EN_Pos ( 4 )
#define _DRV_MPU9250_INT_ENABLE_FIFO_OVERFLOW_EN_Msk 0x1ul << _DRV_MPU9250_INT_ENABLE_FIFO_OVERFLOW_EN_Pos
#define _DRV_MPU9250_INT_ENABLE_FSYNC_INT_EN_Pos     ( 3 )
#define _DRV_MPU9250_INT_ENABLE_FSYNC_INT_EN_Msk     0x1ul << _DRV_MPU9250_INT_ENABLE_FSYNC_INT_EN_Pos
#define _DRV_MPU9250_INT_ENABLE_RAW_RDY_EN_Pos       ( 0 )
#define _DRV_MPU9250_INT_ENABLE_RAW_RDY_EN_Msk       0x1ul << _DRV_MPU9250_INT_ENABLE_RAW_RDY_EN_Pos

#define _DRV_MPU9250_DMP_INT_STATUS                       0x39  // Check DMP interrupt
#define _DRV_MPU9250_DMP_INT_STATUS_WOM_INT_Pos           ( 6 )
#define _DRV_MPU9250_DMP_INT_STATUS_WOM_INT_Msk           0x1ul << _DRV_MPU9250_DMP_INT_STATUS_WOM_INT_Pos
#define _DRV_MPU9250_DMP_INT_STATUS_FIFO_OVERFLOW_INT_Pos ( 4 )
#define _DRV_MPU9250_DMP_INT_STATUS_FIFO_OVERFLOW_INT_Msk 0x1ul << _DRV_MPU9250_DMP_INT_STATUS_FIFO_OVERFLOW_INT_Pos
#define _DRV_MPU9250_DMP_INT_STATUS_FSYNC_INT_Pos         ( 3 )
#define _DRV_MPU9250_DMP_INT_STATUS_FSYNC_INT_Msk         0x1ul << _DRV_MPU9250_DMP_INT_STATUS_FSYNC_INT_Pos
#define _DRV_MPU9250_DMP_INT_STATUS_RAW_DATA_RDY_INT_Pos  ( 0 )
#define _DRV_MPU9250_DMP_INT_STATUS_RAW_DATA_RDY_INT_Msk  0x1ul << _DRV_MPU9250_DMP_INT_STATUS_RAW_DATA_RDY_INT_Pos

#define _DRV_MPU9250_INT_STATUS                       0x3A
#define _DRV_MPU9250_INT_STATUS_WON_INT_Pos           ( 6 )
#define _DRV_MPU9250_INT_STATUS_WON_INT_Msk           0x1ul << _DRV_MPU9250_INT_STATUS_WON_INT_Pos
#define _DRV_MPU9250_INT_STATUS_FIFO_OVERFLOW_INT_Pos ( 4 )
#define _DRV_MPU9250_INT_STATUS_FIFO_OVERFLOW_INT_Msk 0x1ul << _DRV_MPU9250_INT_STATUS_FIFO_OVERFLOW_INT_Pos
#define _DRV_MPU9250_INT_STATUS_FSYNC_INT_INT_Pos     ( 3 )
#define _DRV_MPU9250_INT_STATUS_FSYNC_INT_INT_Msk     0x1ul << _DRV_MPU9250_INT_STATUS_FSYNC_INT_INT_Pos
#define _DRV_MPU9250_INT_STATUS_RAW_RDY_INT_Pos       ( 0 )
#define _DRV_MPU9250_INT_STATUS_RAW_RDY_INT_Msk       0x1ul << _DRV_MPU9250_INT_STATUS_RAW_RDY_INT_Pos

#define _DRV_MPU9250_ACCEL_XOUT_H 0x3B
#define _DRV_MPU9250_ACCEL_XOUT_L 0x3C
#define _DRV_MPU9250_ACCEL_YOUT_H 0x3D
#define _DRV_MPU9250_ACCEL_YOUT_L 0x3E
#define _DRV_MPU9250_ACCEL_ZOUT_H 0x3F
#define _DRV_MPU9250_ACCEL_ZOUT_L 0x40

#define _DRV_MPU9250_TEMP_OUT_H 0x41
#define _DRV_MPU9250_TEMP_OUT_L 0x42

#define _DRV_MPU9250_GYRO_XOUT_H 0x43
#define _DRV_MPU9250_GYRO_XOUT_L 0x44
#define _DRV_MPU9250_GYRO_YOUT_H 0x45
#define _DRV_MPU9250_GYRO_YOUT_L 0x46
#define _DRV_MPU9250_GYRO_ZOUT_H 0x47
#define _DRV_MPU9250_GYRO_ZOUT_L 0x48

#define _DRV_MPU9250_EXT_SENS_DATA_00 0x49
#define _DRV_MPU9250_EXT_SENS_DATA_01 0x4A
#define _DRV_MPU9250_EXT_SENS_DATA_02 0x4B
#define _DRV_MPU9250_EXT_SENS_DATA_03 0x4C
#define _DRV_MPU9250_EXT_SENS_DATA_04 0x4D
#define _DRV_MPU9250_EXT_SENS_DATA_05 0x4E
#define _DRV_MPU9250_EXT_SENS_DATA_06 0x4F
#define _DRV_MPU9250_EXT_SENS_DATA_07 0x50
#define _DRV_MPU9250_EXT_SENS_DATA_08 0x51
#define _DRV_MPU9250_EXT_SENS_DATA_09 0x52
#define _DRV_MPU9250_EXT_SENS_DATA_10 0x53
#define _DRV_MPU9250_EXT_SENS_DATA_11 0x54
#define _DRV_MPU9250_EXT_SENS_DATA_12 0x55
#define _DRV_MPU9250_EXT_SENS_DATA_13 0x56
#define _DRV_MPU9250_EXT_SENS_DATA_14 0x57
#define _DRV_MPU9250_EXT_SENS_DATA_15 0x58
#define _DRV_MPU9250_EXT_SENS_DATA_16 0x59
#define _DRV_MPU9250_EXT_SENS_DATA_17 0x5A
#define _DRV_MPU9250_EXT_SENS_DATA_18 0x5B
#define _DRV_MPU9250_EXT_SENS_DATA_19 0x5C
#define _DRV_MPU9250_EXT_SENS_DATA_20 0x5D
#define _DRV_MPU9250_EXT_SENS_DATA_21 0x5E
#define _DRV_MPU9250_EXT_SENS_DATA_22 0x5F
#define _DRV_MPU9250_EXT_SENS_DATA_23 0x60

#define _DRV_MPU9250_MOT_DETECT_STATUS               0x61
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_XNEG_Pos  ( 7 )
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_XNEG_Msk  0x1ul << _DRV_MPU9250_MOT_DETECT_STATUS_MOT_XNEG_Pos
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_XPOS_Pos  ( 6 )
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_XPOS_Msk  0x1ul << _DRV_MPU9250_MOT_DETECT_STATUS_MOT_XPOS_Pos
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_YNEG_Pos  ( 5 )
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_YNEG_Msk  0x1ul << _DRV_MPU9250_MOT_DETECT_STATUS_MOT_YNEG_Pos
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_YPOS_Pos  ( 4 )
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_YPOS_Msk  0x1ul << _DRV_MPU9250_MOT_DETECT_STATUS_MOT_YPOS_Pos
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZNEG_Pos  ( 3 )
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZNEG_Msk  0x1ul << _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZNEG_Pos
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZPOS_Pos  ( 2 )
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZPOS_Msk  0x1ul << _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZPOS_Pos
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZRMOT_Pos ( 0 )
#define _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZRMOT_Msk 0x1ul << _DRV_MPU9250_MOT_DETECT_STATUS_MOT_ZRMOT_Pos

#define _DRV_MPU9250_I2C_SLV0_DO 0x63
#define _DRV_MPU9250_I2C_SLV1_DO 0x64
#define _DRV_MPU9250_I2C_SLV2_DO 0x65
#define _DRV_MPU9250_I2C_SLV3_DO 0x66

#define _DRV_MPU9250_I2C_MST_DELAY_CTRL                     0x67
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_DELAY_ES_SHADOW_Pos ( 7 )
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_DELAY_ES_SHADOW_Msk 0x1ul << _DRV_MPU9250_I2C_MST_DELAY_CTRL_DELAY_ES_SHADOW_Pos
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV4_DLY_EN_Pos ( 4 )
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV4_DLY_EN_Msk 0x1ul << _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV4_DLY_EN_Pos
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV3_DLY_EN_Pos ( 3 )
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV3_DLY_EN_Msk 0x1ul << _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV3_DLY_EN_Pos
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV2_DLY_EN_Pos ( 2 )
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV2_DLY_EN_Msk 0x1ul << _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV2_DLY_EN_Pos
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV1_DLY_EN_Pos ( 1 )
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV1_DLY_EN_Msk 0x1ul << _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV1_DLY_EN_Pos
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV0_DLY_EN_Pos ( 0 )
#define _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV0_DLY_EN_Msk 0x1ul << _DRV_MPU9250_I2C_MST_DELAY_CTRL_I2C_SLV0_DLY_EN_Pos

#define _DRV_MPU9250_SIGNAL_PATH_RESET               0x68
#define _DRV_MPU9250_SIGNAL_PATH_RESET_GYRO_RST_Pos  ( 2 )
#define _DRV_MPU9250_SIGNAL_PATH_RESET_GYRO_RST_Msk  0x1ul << _DRV_MPU9250_SIGNAL_PATH_RESET_GYRO_RST_Pos
#define _DRV_MPU9250_SIGNAL_PATH_RESET_ACCEL_RST_Pos ( 1 )
#define _DRV_MPU9250_SIGNAL_PATH_RESET_ACCEL_RST_Msk 0x1ul << _DRV_MPU9250_SIGNAL_PATH_RESET_ACCEL_RST_Pos
#define _DRV_MPU9250_SIGNAL_PATH_RESET_TEMP_RST_Pos  ( 0 )
#define _DRV_MPU9250_SIGNAL_PATH_RESET_TEMP_RST_Msk  0x1ul << _DRV_MPU9250_SIGNAL_PATH_RESET_TEMP_RST_Pos

#define _DRV_MPU9250_MOT_DETECT_CTRL                      0x69
#define _DRV_MPU9250_MOT_DETECT_CTRL_ACCEL_INTEL_EN_Pos   ( 7 )
#define _DRV_MPU9250_MOT_DETECT_CTRL_ACCEL_INTEL_EN_Msk   0x1ul << _DRV_MPU9250_MOT_DETECT_CTRL_ACCEL_INTEL_EN_Pos
#define _DRV_MPU9250_MOT_DETECT_CTRL_ACCEL_INTEL_MODE_Pos ( 6 )
#define _DRV_MPU9250_MOT_DETECT_CTRL_ACCEL_INTEL_MODE_Msk 0x1ul << _DRV_MPU9250_MOT_DETECT_CTRL_ACCEL_INTEL_MODE_Pos

#define _DRV_MPU9250_USER_CTRL                  0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define _DRV_MPU9250_USER_CTRL_FIFO_EN_Pos      ( 6 )
#define _DRV_MPU9250_USER_CTRL_FIFO_EN_Msk      0x1ul << _DRV_MPU9250_USER_CTRL_FIFO_EN_Pos
#define _DRV_MPU9250_USER_CTRL_I2C_MST_EN_Pos   ( 5 )
#define _DRV_MPU9250_USER_CTRL_I2C_MST_EN_Msk   0x1ul << _DRV_MPU9250_USER_CTRL_I2C_MST_EN_Pos
#define _DRV_MPU9250_USER_CTRL_I2C_IF_DIS_Pos   ( 4 )
#define _DRV_MPU9250_USER_CTRL_I2C_IF_DIS_Msk   0x1ul << _DRV_MPU9250_USER_CTRL_I2C_IF_DIS_Pos
#define _DRV_MPU9250_USER_CTRL_RESET_DMP_Pos    ( 3 )
#define _DRV_MPU9250_USER_CTRL_RESET_DMP_Msk    0x1ul << _DRV_MPU9250_USER_CTRL_RESET_DMP_Pos
#define _DRV_MPU9250_USER_CTRL_FIFO_RST_Pos     ( 2 )
#define _DRV_MPU9250_USER_CTRL_FIFO_RST_Msk     0x1ul << _DRV_MPU9250_USER_CTRL_FIFO_RST_Pos
#define _DRV_MPU9250_USER_CTRL_I2C_MST_RST_Pos  ( 1 )
#define _DRV_MPU9250_USER_CTRL_I2C_MST_RST_Msk  0x1ul << _DRV_MPU9250_USER_CTRL_I2C_MST_RST_Pos
#define _DRV_MPU9250_USER_CTRL_SIG_COND_RST_Pos ( 0 )
#define _DRV_MPU9250_USER_CTRL_SIG_COND_RST_Msk 0x1ul << _DRV_MPU9250_USER_CTRL_SIG_COND_RST_Pos

#define _DRV_MPU9250_PWR_MGMT_1                  0x6B  // Device defaults to the SLEEP mode
#define _DRV_MPU9250_PWR_MGMT_1_H_RESET_Pos      ( 7 )
#define _DRV_MPU9250_PWR_MGMT_1_H_RESET_Msk      0x1ul << _DRV_MPU9250_PWR_MGMT_1_H_RESET_Pos
#define _DRV_MPU9250_PWR_MGMT_1_SLEEP_Pos        ( 6 )
#define _DRV_MPU9250_PWR_MGMT_1_SLEEP_Msk        0x1ul << _DRV_MPU9250_PWR_MGMT_1_SLEEP_Pos
#define _DRV_MPU9250_PWR_MGMT_1_CYCLE_Pos        ( 5 )
#define _DRV_MPU9250_PWR_MGMT_1_CYCLE_Msk        0x1ul << _DRV_MPU9250_PWR_MGMT_1_CYCLE_Pos
#define _DRV_MPU9250_PWR_MGMT_1_GYRO_STANDBY_Pos ( 4 )
#define _DRV_MPU9250_PWR_MGMT_1_GYRO_STANDBY_Msk 0x1ul << _DRV_MPU9250_PWR_MGMT_1_GYRO_STANDBY_Pos
#define _DRV_MPU9250_PWR_MGMT_1_PD_PTAT_Pos      ( 3 )
#define _DRV_MPU9250_PWR_MGMT_1_PD_PTAT_Msk      0x1ul << _DRV_MPU9250_PWR_MGMT_1_PD_PTAT_Pos
#define _DRV_MPU9250_PWR_MGMT_1_CLKSEL_Pos       ( 0 )
#define _DRV_MPU9250_PWR_MGMT_1_CLKSEL_Msk       0x7ul << _DRV_MPU9250_PWR_MGMT_1_CLKSEL_Pos

#define _DRV_MPU9250_PWR_MGMT_2                0x6C
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_XA_Pos ( 5 )
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_XA_Msk 0x1ul << _DRV_MPU9250_PWR_MGMT_2_DISABLE_XA_Pos
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_YA_Pos ( 4 )
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_YA_Msk 0x1ul << _DRV_MPU9250_PWR_MGMT_2_DISABLE_YA_Pos
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_ZA_Pos ( 3 )
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_ZA_Msk 0x1ul << _DRV_MPU9250_PWR_MGMT_2_DISABLE_ZA_Pos
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_XG_Pos ( 2 )
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_XG_Msk 0x1ul << _DRV_MPU9250_PWR_MGMT_2_DISABLE_XG_Pos
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_YG_Pos ( 1 )
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_YG_Msk 0x1ul << _DRV_MPU9250_PWR_MGMT_2_DISABLE_YG_Pos
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_ZG_Pos ( 0 )
#define _DRV_MPU9250_PWR_MGMT_2_DISABLE_ZG_Msk 0x1ul << _DRV_MPU9250_PWR_MGMT_2_DISABLE_ZG_Pos

#define _DRV_MPU9250_DMP_BANK   0x6D  // Activates a specific bank in the DMP
#define _DRV_MPU9250_DMP_RW_PNT 0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define _DRV_MPU9250_DMP_REG    0x6F  // Register in DMP from which to read or to which to write
#define _DRV_MPU9250_DMP_REG_1  0x70
#define _DRV_MPU9250_DMP_REG_2  0x71

#define _DRV_MPU9250_FIFO_COUNTH              0x72
#define _DRV_MPU9250_FIFO_COUNTH_FIFO_CNT_Pos ( 0 )
#define _DRV_MPU9250_FIFO_COUNTH_FIFO_CNT_Msk 0x1F << _DRV_MPU9250_FIFO_COUNTH_FIFO_CNT_Pos

#define _DRV_MPU9250_FIFO_COUNTL 0x73

#define _DRV_MPU9250_FIFO_R_W 0x74

#define _DRV_MPU9250_WHO_AM_I 0x75  // Should return 0x71

#define _DRV_MPU9250_XA_OFFSET_H 0x77
#define _DRV_MPU9250_XA_OFFSET_L 0x78
#define _DRV_MPU9250_YA_OFFSET_H 0x7A
#define _DRV_MPU9250_YA_OFFSET_L 0x7B
#define _DRV_MPU9250_ZA_OFFSET_H 0x7D
#define _DRV_MPU9250_ZA_OFFSET_L 0x7E

#endif  //__MPU9250_H__
