/**
 * @file    system_stm32f4xx.c
 * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File
 * @details This file contains the system clock configuration for STM32F4xx devices.
 *          
 *          The system clock is configured as follows:
 *          - System Clock source: PLL (HSE)
 *          - SYSCLK(Hz):         168000000
 *          - HCLK(Hz):           168000000
 *          - AHB Prescaler:      1
 *          - APB1 Prescaler:     4
 *          - APB2 Prescaler:     2
 *          - HSE Frequency(Hz):  8000000
 *          - PLL_M:              8
 *          - PLL_N:              336
 *          - PLL_P:              2
 *          - PLL_Q:              7
 *          - VDD:                3.3V
 *          - Flash Latency:      5 WS
 * 
 * @author  [小昭debug]
 * @date    2026-03-15
 */

#include <stdint.h>

/* Forward declarations */
static void SetSysClock(void);
void SystemInit(void);
void SystemCoreClockUpdate(void);

/**
 * @brief Uncomment the line corresponding to the desired system clock frequency
 * @note  Uncomment only one line
 */
/* #define SYSCLK_FREQ_HSE    HSE_VALUE */
/* #define SYSCLK_FREQ_24MHz  24000000 */
/* #define SYSCLK_FREQ_36MHz  36000000 */
/* #define SYSCLK_FREQ_48MHz  48000000 */
/* #define SYSCLK_FREQ_56MHz  56000000 */
/* #define SYSCLK_FREQ_72MHz  72000000 */
/* #define SYSCLK_FREQ_84MHz  84000000 */
/* #define SYSCLK_FREQ_96MHz  96000000 */
/* #define SYSCLK_FREQ_108MHz 108000000 */
/* #define SYSCLK_FREQ_120MHz 120000000 */
/* #define SYSCLK_FREQ_144MHz 144000000 */
#define SYSCLK_FREQ_168MHz 168000000

/**
 * @brief System Clock Frequency (Core Clock)
 */
uint32_t SystemCoreClock = SYSCLK_FREQ_168MHz;

/**
 * @brief AHB Prescaler Table
 */
static const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

/**
 * @brief APB1 Prescaler Table  
 */
/* APBPrescTable is reserved for future use */
#if 0
static const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};
#endif

/**
 * @brief  Setup the microcontroller system
 *         Initialize the Embedded Flash Interface, the PLL and update the
 *         SystemCoreClock variable.
 * @note   This function should be used only after reset.
 * @param  None
 * @return None
 */
void SystemInit(void)
{
    /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    /* set CP10 and CP11 Full Access */
    *(volatile uint32_t *)0xE000ED88 |= ((3UL << 10 * 2) | (3UL << 11 * 2));
#endif

    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    *(volatile uint32_t *)0x40023800 |= (uint32_t)0x00000001;

    /* Reset CFGR register */
    *(volatile uint32_t *)0x40023808 = 0x00000000;

    /* Reset HSEON, CSSON and PLLON bits */
    *(volatile uint32_t *)0x40023800 &= (uint32_t)0xFEF6FFFF;

    /* Reset PLLCFGR register */
    *(volatile uint32_t *)0x40023804 = 0x24003010;

    /* Reset HSEBYP bit */
    *(volatile uint32_t *)0x40023800 &= (uint32_t)0xFFFBFFFF;

    /* Disable all interrupts */
    *(volatile uint32_t *)0x4002380C = 0x00000000;

    /* Configure the System clock source, PLL Multiplier and Divider factors,
       AHB/APBx prescalers and Flash settings ----------------------------------*/
    SetSysClock();
}

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 *         The SystemCoreClock variable contains the core clock (HCLK), it can
 *         be used by the user application to setup the SysTick timer or configure
 *         other parameters.
 * @note   Each time the core clock (HCLK) changes, this function must be called
 *         to update SystemCoreClock variable value. Otherwise, any configuration
 *         based on this variable will be incorrect.
 * @param  None
 * @return None
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = (*(volatile uint32_t *)0x40023808 & 0x0000000C);

    switch (tmp) {
    case 0x00:  /* HSI used as system clock source */
        SystemCoreClock = 16000000;
        break;
    case 0x04:  /* HSE used as system clock source */
        SystemCoreClock = 8000000;
        break;
    case 0x08:  /* PLL used as system clock source */

        /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
           SYSCLK = PLL_VCO / PLL_P
           */
        pllsource = (*(volatile uint32_t *)0x40023804 & 0x00400000) >> 22;
        pllm = (*(volatile uint32_t *)0x40023804 & 0x0000003F);

        if (pllsource != 0) {
            /* HSE used as PLL clock source */
            pllvco = (uint32_t)(((uint64_t)8000000 * ((uint64_t)((*(volatile uint32_t *)0x40023804 & 0x00007FC0) >> 6))) / (uint64_t)pllm);
        } else {
            /* HSI used as PLL clock source */
            pllvco = (uint32_t)(((uint64_t)16000000 * ((uint64_t)((*(volatile uint32_t *)0x40023804 & 0x00007FC0) >> 6))) / (uint64_t)pllm);
        }

        pllp = (((*(volatile uint32_t *)0x40023804 & 0x00030000) >> 16) + 1) * 2;
        SystemCoreClock = pllvco / pllp;
        break;
    default:
        SystemCoreClock = 16000000;
        break;
    }

    /* Compute HCLK frequency --------------------------------------------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((*(volatile uint32_t *)0x40023808 & 0x000000F0) >> 4)];
    /* HCLK frequency */
    SystemCoreClock >>= tmp;
}

/**
 * @brief  Configure the System clock source, PLL Multiplier and Divider factors,
 *         AHB/APBx prescalers and Flash settings
 * @note   This function should be called only once after reset.
 * @param  None
 * @return None
 */
static void SetSysClock(void)
{
    /* 1. Enable HSE (High Speed External) oscillator */
    /* Set HSEON bit */
    *(volatile uint32_t *)0x40023800 |= (uint32_t)0x00010000;

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
    } while ((*(volatile uint32_t *)0x40023800 & 0x00020000) == 0);

    /* 2. Enable high performance mode, System frequency up to 168 MHz */
    *(volatile uint32_t *)0x40023800 |= (uint32_t)0x00000100;

    /* 3. Flash latency configuration for 168 MHz @ 3.3V: 5 WS */
    *(volatile uint32_t *)0x40023C00 = (uint32_t)0x00000507;

    /* 4. Configure PLL --------------------------------------------------------*/
    /* Disable PLL before configuration */
    *(volatile uint32_t *)0x40023800 &= ~(uint32_t)0x01000000;

    /* Wait till PLL is disabled */
    while ((*(volatile uint32_t *)0x40023800 & 0x02000000) != 0) {}

    /* Configure the main PLL clock source, multiplication and division factors */
    /* PLL_VCO = (HSE / PLL_M) * PLL_N = (8MHz / 8) * 336 = 336 MHz */
    /* SYSCLK = PLL_VCO / PLL_P = 336 / 2 = 168 MHz */
    /* USB OTG FS clock = PLL_VCO / PLL_Q = 336 / 7 = 48 MHz */
    *(volatile uint32_t *)0x40023804 = (uint32_t)0x07405408;

    /* Enable the main PLL */
    *(volatile uint32_t *)0x40023800 |= (uint32_t)0x01000000;

    /* Wait till the main PLL is ready */
    while ((*(volatile uint32_t *)0x40023800 & 0x02000000) == 0) {}

    /* 5. Configure bus prescalers ---------------------------------------------*/
    /* HCLK = SYSCLK / 1 (AHB Prescaler = 1) */
    /* PCLK2 = HCLK / 2 (APB2 Prescaler = 2) */
    /* PCLK1 = HCLK / 4 (APB1 Prescaler = 4) */
    *(volatile uint32_t *)0x40023808 |= (uint32_t)0x00009400;

    /* 6. Select the main PLL as system clock source ---------------------------*/
    *(volatile uint32_t *)0x40023808 &= ~(uint32_t)0x00000003;
    *(volatile uint32_t *)0x40023808 |= (uint32_t)0x00000002;

    /* Wait till the main PLL is used as system clock source */
    while ((*(volatile uint32_t *)0x40023808 & 0x0000000C) != 0x08) {}
}
