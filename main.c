/* -------------------------------------------------------------------------------------------------------------------- */
// F28335 - Base Project
/* -------------------------------------------------------------------------------------------------------------------- */
//
// Author:      Renan R. Duarte
// Date:        27/09/19
// Device:      Piccolo F28335 Launchpad
//
/* -------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------- */
// Includes
/* -------------------------------------------------------------------------------------------------------------------- */

// Devide macros and defines - Includes constant definitions and macros used on TI examples
#include "DSP28x_Project.h"

/* -------------------------------------------------------------------------------------------------------------------- */
// Hardware definitions
/* -------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------- */
// Program definitions
/* -------------------------------------------------------------------------------------------------------------------- */
#define LED0_ON             GpioDataRegs.GPACLEAR.bit.GPIO31 = 1
#define LED1_ON             GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1

#define LED0_OFF            GpioDataRegs.GPASET.bit.GPIO31 = 1
#define LED1_OFF            GpioDataRegs.GPBSET.bit.GPIO34 = 1

#define LED0_TOGGLE         GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1
#define LED1_TOGGLE         GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1
/* -------------------------------------------------------------------------------------------------------------------- */
// Variables
/* -------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------- */
// Function prototypes
/* -------------------------------------------------------------------------------------------------------------------- */
   __interrupt void cpu_timer0_isr(void);


/* -------------------------------------------------------------------------------------------------------------------- */
// Main function
/* -------------------------------------------------------------------------------------------------------------------- */

void main(void)
{
    /* ---------------------------------------------------------------------------------------------------------------- */
    // Memory settings
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Copy functions from flash to RAM - DO NOT REMOVE (Uses F28027.cmd instead of 28027_RAM_Ink.cmd)
    #ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
    #endif

    /* ---------------------------------------------------------------------------------------------------------------- */
    // MCU settings
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Basic Core Init
    InitSysCtrl();

    // Disable all interrupts
    DINT;

    // Basic setup of PIE table
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags
    IER = 0x0000;
    IFR = 0x0000;

    // Set default ISR's in PIE
    InitPieVectTable ();


    /* ---------------------------------------------------------------------------------------------------------------- */
    // GPIO configuration - LEDs
    /* ---------------------------------------------------------------------------------------------------------------- */

    EALLOW;

    // LED pins as outputs
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1; //registrador LD2
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1; //registrador LD3

    EDIS;

    // Set initial state - LEDs off
    LED0_OFF;
    LED1_OFF;


    /* ---------------------------------------------------------------------------------------------------------------- */
    // Timer configuration
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Config Timer
    EALLOW;
    PieVectTable.TINT0 = &cpu_timer0_isr;
    EDIS;

    InitCpuTimers();

    // uC de 150MHz com interrupção a cada 1 segundo (em microsegundos)
    #if (CPU_FQR_150MHZ)
    ConfigCpuTimer(&CpuTimer0, 150, 1000000); //aqui foi editado para 100 (estava em 1000000)
    #endif

    //referente à biblioteca DSP2833x_CpuTimers.h
    CpuTimer0Regs.TCR.all = 0x4000;

    // Enable CPU int1 which is connected to CPU-Timer 0
    IER |= M_INT1;

    // Enable TINT0 in the PIE: Group 1 interrupt 7
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    // Enable global Interrupts and higher priority real-time debug events:
    EINT;
    ERTM;

    //IDLE loop
    for(;;);

}

    /* ---------------------------------------------------------------------------------------------------------------- */
    // Main loop
    /* ---------------------------------------------------------------------------------------------------------------- */



    // cpu_timer0_isr -
__interrupt void
cpu_timer0_isr(void)
{
	LED0_TOGGLE;
	LED1_TOGGLE;

    // Acknowledge this interrupt to receive more interrupts from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

/* -------------------------------------------------------------------------------------------------------------------- */
// End of file
/* -------------------------------------------------------------------------------------------------------------------- */



