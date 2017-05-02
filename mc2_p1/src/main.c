/*
 * m2_v1_FreeRTOS_Intro_Vorlage
 * (c) Noah Huetter 2017
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "Timer.h"
#include "BlinkLed.h"
#include "Lcd.h"
#include "stm32f429i_discovery_lcd.h"

#define PINK (0xff<<24) | (0xd6<<16) | (0x64<<8) | (0xbe<<0)
#define BLUE (0xff<<24) | (0x00<<16) | (0x00<<8) | (0xff<<0)
#define RED (0xff<<24) | (0xff<<16) | (0x00<<8) | (0x00<<0)
#define BLACK (0xff<<24) | (0x00<<16) | (0x00<<8) | (0x00<<0)

/* mm: added helper variable to support FreeRTOS thread list support in gdb
 * Remark: OpenOCD gdbserver references xTopReadyPriority which was removed in newer FreeRTOS releases.
 * The declaration below together with linker flag '-Wl,--undefined=uxTopUsedPriority' solves the problem */
const int __attribute__((used)) uxTopUsedPriority = configMAX_PRIORITIES;

SemaphoreHandle_t LCDSemaphore;

static void WasserTask(void *pvParameters);
static void TrommelTask(void *pvParameters);
static void SteuerTask(void *pvParameters);
static void lcd_printf(uint32_t Color, uint16_t X, uint16_t Y, Text_AlignModeTypdef mode, const char *fmt, ...) __attribute__((__format__(__printf__,5,6)));

int main(void)
{
	SysTick->CTRL = 0; // Disable SYSTICK timer to init LCD
	blink_led_init();
	lcd_init();
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

	LCDSemaphore = xSemaphoreCreateMutex();

	xTaskCreate(WasserTask, "WasserTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 3), NULL);
	xTaskCreate(TrommelTask, "TrommelTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 2), NULL);
	xTaskCreate(SteuerTask, "SteuerTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 1), NULL);

	vTaskStartScheduler();

	/* the FreeRTOS scheduler never returns to here except when out of memory at creating the idle task */
	for (;;) ;
}

static void lcd_printf(uint32_t Color, uint16_t X, uint16_t line, Text_AlignModeTypdef mode, const char *fmt, ...)
{
	char s[30];
    va_list ap;
	va_start(ap, fmt);
	vsnprintf(s, sizeof(s), fmt, ap);
	va_end(ap);
	if( xSemaphoreTake( LCDSemaphore, ( TickType_t ) 10 ) == pdTRUE )
	{
		BSP_LCD_SetTextColor(Color);
		BSP_LCD_DisplayStringAt(X, LINE(line), s, mode);
		xSemaphoreGive( LCDSemaphore );
	}
}

static void WasserTask(__attribute__ ((unused)) void *pvParameters)
{
	lcd_printf(BLACK, 0, 0, CENTER_MODE, "mc2_p1_huetter");

	while (1)
	{
		vTaskDelay(100);
	}
}

static void TrommelTask(__attribute__ ((unused)) void *pvParameters)
{
	while(1)
	{
		vTaskDelay(100);
	}
}

static void SteuerTask(__attribute__ ((unused)) void *pvParameters)
{
	while(1)
	{
		vTaskDelay(100);
	}
}
// ----------------------------------------------------------------------------
