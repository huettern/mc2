/*
 * m2_v1_FreeRTOS_Intro_Vorlage
 * (c) Matthias Meier 2017
 */

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"
//#include "semphr.h"

#include "Timer.h"
#include "BlinkLed.h"
#include "Lcd.h"
#include "stm32f429i_discovery_lcd.h"

/* mm: added helper variable to support FreeRTOS thread list support in gdb
 * Remark: OpenOCD gdbserver references xTopReadyPriority which was removed in newer FreeRTOS releases.
 * The declaration below together with linker flag '-Wl,--undefined=uxTopUsedPriority' solves the problem */
const int __attribute__((used)) uxTopUsedPriority = configMAX_PRIORITIES;


static void CounterTask(void *pvParameters);


int main(void)
{
	blink_led_init();

	xTaskCreate(CounterTask, "CntTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 1), NULL);

	vTaskStartScheduler();

	/* the FreeRTOS scheduler never returns to here except when out of memory at creating the idle task */
	for (;;) ;
}


static void CounterTask(__attribute__ ((unused)) void *pvParameters)
{
	int n = 0;
	lcd_init();
	BSP_LCD_DisplayStringAtLine(0, "mc2 Vorlage");

	while (1) {
		char s[20];

		blink_led_on();
		vTaskDelay(100);
		blink_led_off();
		vTaskDelay(400);
		snprintf(s, sizeof(s), "Counter = %d", n++);
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2, s, CENTER_MODE);
	}
}

// ----------------------------------------------------------------------------
