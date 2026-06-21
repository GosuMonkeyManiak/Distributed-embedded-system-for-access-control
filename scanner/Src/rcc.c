/*
 * rcc_config.c
 *
 *  Created on: Feb 13, 2026
 *      Author: dkupanov
 */

#include "rcc.h"
#include "stm32wbxx_ll_cortex.h"
#include "stm32wbxx_ll_system.h"
#include "stm32wbxx_ll_bus.h"
#include "stm32wbxx_ll_pwr.h"
#include "stm32wbxx_ll_rcc.h"


static void low_speed_clocks_init(void);
static void high_speed_clocks_init(void);

static void plls_init(void);
static void peripheral_clocks_init(void);

void rcc_init(void) {

	LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_3) {

	}

	low_speed_clocks_init();
	high_speed_clocks_init();

	/* Disable MCO (Microcontroller clock output) */
	LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_NOCLOCK, LL_RCC_MCO1_DIV_1);

	/* Disable LSCO (Low speed clock output) */
	LL_RCC_LSCO_Disable();

	/* HSEPRE divider disable */
	LL_RCC_HSE_DisableDiv2();

	plls_init();

	peripheral_clocks_init();

	/* Sysclk activation on the PLLRCLK 64Mhz */
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {

	}

	/* Configure I2C clock source */
	LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);
}

static void low_speed_clocks_init(void) {

	/* Enable RTC clock */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTCAPB);

	LL_PWR_EnableBkUpAccess();

	/* LSE (Low speed external) configuration and activation crystal oscillator 32.768kHz */
	// TODO: adjust drive force
	LL_RCC_LSE_DisableBypass();
	LL_RCC_LSE_Enable();
	while(LL_RCC_LSE_IsReady() != 1) {

	}

	LL_PWR_DisableBkUpAccess();

	/* LSI1 (low speed internal) configuration and activation RC oscillator 32.768kHz */
	LL_RCC_LSI1_Enable();
	while (LL_RCC_LSI1_IsReady() != 1) {

	}

	/* LSI2 (low speed internal) disable RC oscillator ~32kHz */
	LL_RCC_LSI2_Disable();
}

static void high_speed_clocks_init(void) {

	/* HSE (High speed external) configuration and activation crystal oscillator 32Mhz */
	// TODO: enable clock security system
	// TODO: tune it for better RF
	LL_RCC_HSE_Enable();
	while (LL_RCC_HSE_IsReady() != 1) {

	}

	/* MSI (Multi-speed internal) configuration and activation RC oscillator 4Mhz used after reset */
	// TODO: calibration
	// TODO: hardware auto calibration with LSE (PLL mode)
	LL_RCC_MSI_Enable();
	while (LL_RCC_MSI_IsReady() != 1) {

	}

	/* HSI16 (High speed internal) configuration and activation RC oscillator 16Mhz */
	// TODO: calibration
	LL_RCC_HSI_Enable();
	while (LL_RCC_HSI_IsReady() != 1) {

	}

	/* HSI48 (High speed internal) configuration and activation RC oscillator 48Mhz */
	// TODO: look up in datasheet for adjustments
	LL_RCC_HSI48_Enable();
	while (LL_RCC_HSI48_IsReady() != 1) {

	}
}

static void plls_init(void) {

	/* PLLs disable */
	LL_RCC_PLL_Disable();
	while (LL_RCC_PLL_IsReady() != 0) {

	}

	LL_RCC_PLLSAI1_Disable();
	while (LL_RCC_PLLSAI1_IsReady() != 0) {

	}

	/* PLL configuration for SYSCLK domain: source:HSE, M:4, N:16, R:2, output:64Mhz */
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 16, LL_RCC_PLLR_DIV_2);

	/* Enable SYSCLK domain PLL */
	LL_RCC_PLL_Enable();
	while (LL_RCC_PLL_IsReady() != 1) {

	}

	/* Enable R output of SYSCLK domain PLL */
	LL_RCC_PLL_EnableDomain_SYS();
}

static void peripheral_clocks_init(void) {

	/* RF wakeup clock source selection (LSE) */
	//LL_RCC_SetRFWKPClockSource(LL_RCC_RFWKP_CLKSOURCE_LSE);

	/* CPU1 AHB (HCLK1) prescaler configuration max 64Mhz current 64Mhz */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	while (LL_RCC_IsActiveFlag_HPRE() != 1) {

	}

	while (LL_RCC_GetAHBPrescaler() != LL_RCC_SYSCLK_DIV_1) {

	}

	/* CPU1 APB1 (PCLK1) prescaler configuration max 64MHz current 64Mhz */
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	while (LL_RCC_IsActiveFlag_PPRE1() != 1) {

	}

	/* CPU1 APB2 (PCLK2) prescaler configuration max 64MHz current 64Mhz */
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	while (LL_RCC_IsActiveFlag_PPRE2() != 1) {

	}

	/* CPU2 AHB (HCLK2) prescaler configuration max 32Mhz current 32Mhz */
	LL_C2_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
	while (LL_RCC_IsActiveFlag_C2HPRE() != 1) {

	}

	while (LL_C2_RCC_GetAHBPrescaler() != LL_RCC_SYSCLK_DIV_2) {

	}

	/* AHB4 (HCLK4) prescaler configuration max 64Mhz current 64Mhz */
	LL_RCC_SetAHB4Prescaler(LL_RCC_SYSCLK_DIV_1);
	while (LL_RCC_IsActiveFlag_SHDHPRE() != 1) {

	}

	while (LL_RCC_GetAHB4Prescaler() != LL_RCC_SYSCLK_DIV_1) {

	}

	/* SMPS (Switch Mode Power Supply) configuration 4Mhz */
	LL_RCC_SetSMPSClockSource(LL_RCC_SMPS_CLKSOURCE_HSE);
	LL_RCC_SetSMPSPrescaler(LL_RCC_SMPS_DIV_1);

}
