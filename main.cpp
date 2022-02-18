/**
 * 48 Mhz: M=6,  N=1
 * 96 Mhz: M=12, N=1
 */
#include "mbed.h"
#include "math.h"
#include "FreeRTOS.h"
#include "task.h"
#include "CoreMark/core_portme.h"
#include "ClockControl/ClockControl.h"

#define M_RANGE 7   // M from 6 to 12

#ifndef configSYSTICK_CLOCK_HZ
    #define configSYSTICK_CLOCK_HZ      configCPU_CLOCK_HZ
    /* Ensure the SysTick is clocked at the same frequency as the core. */
    #define portNVIC_SYSTICK_CLK_BIT    ( 1UL << 2UL )
#else

/* The way the SysTick is clocked is not modified in case it is not the same
 * as the core. */
    #define portNVIC_SYSTICK_CLK_BIT    ( 0 )
#endif


#define portNVIC_SYSTICK_CTRL_REG             ( *( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG             ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG    ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SHPR3_REG                    ( *( ( volatile uint32_t * ) 0xe000ed20 ) )
/* ...then bits in the registers. */
#define portNVIC_SYSTICK_INT_BIT              ( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT           ( 1UL << 0UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT       ( 1UL << 16UL )
#define portNVIC_PENDSVCLEAR_BIT              ( 1UL << 27UL )
#define portNVIC_PEND_SYSTICK_CLEAR_BIT       ( 1UL << 25UL )

#define portNVIC_PENDSV_PRI                   ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 16UL )
#define portNVIC_SYSTICK_PRI                  ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )

DigitalOut led1(LED1);
Serial pc(USBTX, USBRX); // tx, rx

float eatCpu()
{
    volatile int i = 0;
    volatile float k = 0;
    for (i = 0; i < 0x1FFF; i++) {
        k = (rand() % 100) + (i + sin(k)) * (cos(i) / k) * (tan(k) / (float) i);
    }
    return k;
}

void task(void* p) {
    TickType_t period = 1000;

    BaseType_t m_range[] = {12, 11, 10, 9, 8, 7, 6};
    BaseType_t m_index = 0;

    unsigned short newM;
    unsigned short newN = 1;

    TickType_t xLastWakeUp = 0;

    unsigned int newSpeed = 0;

    for(;;) {
        newM = m_range[m_index];
        portDISABLE_INTERRUPTS();
        /* Stop and clear the SysTick. */
        portNVIC_SYSTICK_CTRL_REG = 0UL;
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

        newSpeed = setSystemFrequency(0x3, 0x1, newM, newN);

        /* Configure SysTick to interrupt at the requested rate. */
        portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
        portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );

        portENABLE_INTERRUPTS();
        //pc.printf("m = %d\n\r", (int) newM);
        eatCpu();
        //pc.printf("m=%d\n\r", newM);
        /*
        int Fin = 12000000; // 12MHz XTAL
        pc.printf("PLL Registers:\r\n");
        pc.printf(" - PLL0CFG = 0x%08X\r\n", LPC_SC->PLL0CFG);
        pc.printf(" - CLKCFG  = 0x%08X\r\n", LPC_SC->CCLKCFG);

        int M = (LPC_SC->PLL0CFG & 0xFFFF) + 1;
        int N = (LPC_SC->PLL0CFG >> 16) + 1;
        int CCLKDIV = LPC_SC->CCLKCFG + 1;

        pc.printf("Clock Variables:\r\n");
        pc.printf(" - Fin = %d\r\n", Fin);
        pc.printf(" - M   = %d\r\n", M);
        pc.printf(" - N   = %d\r\n", N);
        pc.printf(" - CCLKDIV = %d\r\n", CCLKDIV);

        int Fcco = (2 * M * 12000000) / N;
        int CCLK = Fcco / CCLKDIV;

        pc.printf("Clock Results:\r\n");
        pc.printf(" - Fcco = %d\r\n", Fcco);

        pc.printf(" - CCLK = %d\r\n", CCLK);

        pc.printf("Running CoreMark");
        //mainCoreMark();
*/
        vTaskDelayUntil( &xLastWakeUp, 1000 );

        m_index = (m_index + 1) % M_RANGE;
    }
}

int main()
{
    // Initializes the trace recorder, but does not start the tracing.
    vTraceEnable( TRC_INIT );

    srand(0);

    xTaskCreate(task, "task", 256, NULL, configMAX_PRIORITIES - 1, NULL);

    vTraceEnable( TRC_START );

    vTaskStartScheduler();

    while(1)
    {
        led1 = 0;
        wait(0.5);
        led1 = 1;
        wait(0.5);
    }
}
