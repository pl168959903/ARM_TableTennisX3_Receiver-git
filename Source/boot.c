#include "TableTennisX3_Receiver.h"

void MCU_Init( void ){
    SYS_UnlockReg();
    // -------------------------------
    MCU_PinInit();
    MCU_ClkInit();
    MCU_SysTickInit(5);
    MCU_GpioInit();
    MCU_UartInit();
    MCU_SpiInit();
    MCU_NvicInit();
    // -------------------------------
    //SYS_LockReg();
}
