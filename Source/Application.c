#include "TableTennisX3_Receiver.h"

DRV_SPI* spi0;
DRV_SPI* spi2;
DRV_NRF* nrf;

void NRF_SetCE( uint8_t en ) {
    if ( en ) { _NRF_CE_PIN = 1; }
    else {
        _NRF_CE_PIN = 0;
    }
}

void NRF_Init( void ) {
    nrf = DRV_nrfNew();
    nrf->Initialize( nrf, ( DRV_NRF_PARAMS ){ spi0, NRF_SetCE, 60, 3, 1, 3, 15, 15 } );
    GPIO_EnableInt( _GET_GPIO_PORT( _NRF_INT0_PIN ), _GET_GPIO_PIN( _NRF_INT0_PIN ), GPIO_INT_LOW );
}

void MCU_SysTickInit( float sysTickFrequency ) {
    uint32_t clksrc = CLK_GetHCLKFreq();
    if ( ( ( float )clksrc / sysTickFrequency ) > 0xFFFFFF ) {
        SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
        SysTick->LOAD = ( uint32_t )( ( float )( clksrc >> 3 ) / sysTickFrequency );
    }
    else {
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
        SysTick->LOAD = ( uint32_t )( ( float )clksrc / sysTickFrequency );
    }
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void MCU_SysTickDelayUs( uint32_t delayTime ) {
    uint32_t tempLoad = SysTick->LOAD;
    uint32_t tempCtrl = SysTick->CTRL;
    CLK_SysTickDelay( delayTime );
    SysTick->LOAD = tempLoad;
    SysTick->CTRL = tempCtrl;
}

void MCU_PinInit( void ) {
    // Pin fuction
    //----------------------------------------------------------------
    SYS->GPA_MFPL |=
        ( SYS_GPA_MFPL_PA2MFP_UART1_RXD |  //
          SYS_GPA_MFPL_PA3MFP_UART1_TXD |  //
          SYS_GPA_MFPL_PA4MFP_GPIO |       //
          SYS_GPA_MFPL_PA5MFP_GPIO |       //
          SYS_GPA_MFPL_PA6MFP_GPIO );
    SYS->GPA_MFPH |=
        ( SYS_GPA_MFPH_PA8MFP_GPIO |        //
          SYS_GPA_MFPH_PA9MFP_GPIO |        // useless
          SYS_GPA_MFPH_PA14MFP_UART0_RXD |  //
          SYS_GPA_MFPH_PA15MFP_UART0_TXD    //
        );
    SYS->GPB_MFPL |=
        ( SYS_GPB_MFPL_PB0MFP_SPI1_MOSI0 |  //
          SYS_GPB_MFPL_PB1MFP_GPIO |        //
          SYS_GPB_MFPL_PB2MFP_SPI1_CLK |    //
          SYS_GPB_MFPL_PB3MFP_SPI1_SS0 |    //
          SYS_GPB_MFPL_PB4MFP_GPIO |        //
          SYS_GPB_MFPL_PB5MFP_GPIO          //
        );
    SYS->GPB_MFPH |=
        ( SYS_GPB_MFPH_PB8MFP_GPIO |       // useless
          SYS_GPB_MFPH_PB9MFP_GPIO |       // useless
          SYS_GPB_MFPH_PB10MFP_GPIO |      // useless
          SYS_GPB_MFPH_PB11MFP_PWM0_CH4 |  //
          SYS_GPB_MFPH_PB13MFP_GPIO |      // useless
          SYS_GPB_MFPH_PB14MFP_GPIO |      //
          SYS_GPB_MFPH_PB15MFP_GPIO        //
        );
    SYS->GPC_MFPL |=
        ( SYS_GPC_MFPL_PC0MFP_SPI0_SS0 |    //
          SYS_GPC_MFPL_PC1MFP_SPI0_CLK |    //
          SYS_GPC_MFPL_PC2MFP_SPI0_MISO0 |  //
          SYS_GPC_MFPL_PC3MFP_SPI0_MOSI0 );
    SYS->GPF_MFPL |=
        ( SYS_GPF_MFPL_PF0MFP_ICE_DAT |  //
          SYS_GPF_MFPL_PF1MFP_ICE_CLK |  //
          SYS_GPF_MFPL_PF2MFP_XT1_OUT |  //
          SYS_GPF_MFPL_PF3MFP_XT1_IN |   //
          SYS_GPF_MFPL_PF7MFP_GPIO       //
        );
}

void MCU_GpioInit( void ) {
    // Disable  digital signal input;
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _XT1_OUT_PIN ), _MASK( _GET_GPIO_PIN( _XT1_OUT_PIN ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _XT1_IN_PIN ), _MASK( _GET_GPIO_PIN( _XT1_IN_PIN ) ) );

    // Debounce
    GPIO_SET_DEBOUNCE_TIME( GPIO_DBCLKSRC_IRC10K, GPIO_DBCLKSEL_128 );  // Debounce clock 10K*128 = 78.125Hz ; 12.8ms
    GPIO_ENABLE_DEBOUNCE( _GET_GPIO_PORT( _KEY0_PIN ), _MASK( _GET_GPIO_PIN( _KEY0_PIN ) ) );
    GPIO_ENABLE_DEBOUNCE( _GET_GPIO_PORT( _KEY1_PIN ), _MASK( _GET_GPIO_PIN( _KEY1_PIN ) ) );
    GPIO_ENABLE_DEBOUNCE( _GET_GPIO_PORT( _KEY2_PIN ), _MASK( _GET_GPIO_PIN( _KEY2_PIN ) ) );

    // GPIO mode;
    GPIO_SetMode( _GET_GPIO_PORT( _NRF_INT0_PIN ), _MASK( _GET_GPIO_PIN( _NRF_INT0_PIN ) ), GPIO_PMD_INPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _KEY0_PIN ), _MASK( _GET_GPIO_PIN( _KEY0_PIN ) ), GPIO_PMD_INPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _KEY1_PIN ), _MASK( _GET_GPIO_PIN( _KEY1_PIN ) ), GPIO_PMD_INPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _KEY2_PIN ), _MASK( _GET_GPIO_PIN( _KEY2_PIN ) ), GPIO_PMD_INPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _NRF_CE_PIN ), _MASK( _GET_GPIO_PIN( _NRF_CE_PIN ) ), GPIO_PMD_OUTPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _LED0_PIN ), _MASK( _GET_GPIO_PIN( _LED0_PIN ) ), GPIO_PMD_OUTPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _LED1_PIN ), _MASK( _GET_GPIO_PIN( _LED1_PIN ) ), GPIO_PMD_OUTPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _LED2_PIN ), _MASK( _GET_GPIO_PIN( _LED2_PIN ) ), GPIO_PMD_OUTPUT );

    // pull up
    GPIO_ENABLE_PULL_UP( _GET_GPIO_PORT( _NRF_CE_PIN ), _MASK( _GET_GPIO_PIN( _NRF_CE_PIN ) ) );

    // GPIO default value
    _LED0_PIN = 1;
    _LED1_PIN = 1;
    _LED2_PIN = 1;
}
void MCU_ClkInit( void ) {
    //----------------------------------------------------------------
    // Enable Clock

    // HXT
    CLK_EnableXtalRC( CLK_PWRCTL_HXTEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk ) ) {};

    // MIRC
    CLK_EnableXtalRC( CLK_PWRCTL_MIRCEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_MIRCSTB_Msk ) ) {};

    // HIRC0
    CLK_EnableXtalRC( CLK_PWRCTL_HIRC0EN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_HIRC0STB_Msk ) ) {};

    // HIRC1
    CLK_EnableXtalRC( CLK_PWRCTL_HIRC1EN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_HIRC1STB_Msk ) ) {};

    // LIRC
    CLK_EnableXtalRC( CLK_PWRCTL_LIRCEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_LIRCSTB_Msk ) ) {};

    //----------------------------------------------------------------
    // Setting BUS clock
    CLK->CLKSEL0 |= CLK_CLKSEL0_HIRCSEL_Msk;  // Select HIRC clock source from HIRC0.

    CLK_SetHCLK( CLK_CLKSEL0_HCLKSEL_HIRC1, CLK_HCLK_CLK_DIVIDER( 1 ) );
    CLK_SetPCLK0( CLK_APB0DIV_HCLK );
    CLK_SetPCLK1( CLK_APB1DIV_HCLK );
}

void MCU_UartInit( void ) {
#if _OPEN_DEBUG_PORT
    CLK_SetModuleClock( UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_UART0_CLK_DIVIDER( 1 ) );
    CLK_EnableModuleClock( UART0_MODULE );
    SYS_ResetModule( UART0_RST );
    while ( SYS->IPRST2 & SYS_IPRST2_UART0RST_Msk ) {};
    UART_Open( UART0, _UART0_BAUD );
    printf( "Debug serial port has been opened!!\n" );
#endif

    CLK_SetModuleClock( UART1_MODULE, CLK_CLKSEL2_UART1SEL_HIRC, CLK_UART1_CLK_DIVIDER( 1 ) );
    CLK_EnableModuleClock( UART1_MODULE );
    UART_Open( UART1, 115200 );
}

void MCU_SpiInit( void ) {
    CLK_SetModuleClock( SPI0_MODULE, CLK_CLKSEL1_SPI0SEL_HCLK, NULL );
    CLK_EnableModuleClock( SPI0_MODULE );
    spi0 = DRV_SpiNew( ( DRV_SPI_PARAMS ){ SPI0, 0, 0, 8, 1000000 } );
    spi0->Initialize( spi0 );
}

void MCU_NvicInit( void ) {
    NVIC_EnableIRQ( GPABC_IRQn );
}
// ------------------------------------------------
// Function
void DelayUs( uint32_t delayTime ) {
    do {
        uint32_t t = ( delayTime > ( 0x1000000 / ( CLK_GetHCLKFreq() / 1000000 ) ) ) ? ( 0x1000000 / ( CLK_GetHCLKFreq() / 1000000 ) ) : delayTime;
        MCU_SysTickDelayUs( t );
        delayTime -= t;
    } while ( delayTime );
}
void PrintClockStatus( void ) {
    printf( "--------------------------------\n" );
    printf( "Clock config : \n" );
    printf( "HXT : %d Hz\n", CLK_GetHXTFreq() );
    printf( "LXT : %d Hz\n", CLK_GetLXTFreq() );
    printf( "HIRC0 : %d Hz\n", CLK_GetHIRC0Freq() );
    printf( "HIRC1 : %d Hz\n", CLK_GetHIRC1Freq() );
    printf( "HIRC : %d Hz\n", CLK_GetHIRCFreq() );
    printf( "MIRC : %d Hz\n", CLK_GetMIRCFreq() );
    printf( "LIRC : %d Hz\n", CLK_GetLIRCFreq() );
    printf( "CPU : %d Hz\n", CLK_GetCPUFreq() );
    printf( "HCLK : %d Hz\n", CLK_GetHCLKFreq() );
    printf( "PCLK0 : %d Hz\n", CLK_GetPCLK0Freq() );
    printf( "PCLK1 : %d Hz\n", CLK_GetPCLK1Freq() );
    printf( "PLL : %d Hz\n", CLK_GetPLLClockFreq() );
}
// ------------------------------------------------
// IRQHandler
void GPABC_IRQHandler( void ) {
    if ( GPIO_GET_INT_FLAG( _GET_GPIO_PORT( _NRF_INT0_PIN ), _MASK( _GET_GPIO_PIN( _NRF_INT0_PIN ) ) ) ) {
        uint8_t intStatus = DRV_NRF_NOP( nrf );
        if ( intStatus & _DRV_NRF_REG_STATUS_TX_DS_MSK ) nrf->Event_TX_DS( nrf );
        if ( intStatus & _DRV_NRF_REG_STATUS_RX_DR_MSK ) nrf->Event_RX_DR( nrf );
        if ( intStatus & _DRV_NRF_REG_STATUS_MAX_RT_MSK ) nrf->Event_MAX_RT( nrf );
        GPIO_CLR_INT_FLAG( _GET_GPIO_PORT( _NRF_INT0_PIN ), _MASK( _GET_GPIO_PIN( _NRF_INT0_PIN ) ) );
    }
}
void SysTick_Handler( void ) {}
