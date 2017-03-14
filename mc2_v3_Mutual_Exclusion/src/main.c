/*
 * m2_v1_FreeRTOS_Intro_Vorlage
 * (c) Matthias Meier 2017
 */

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"
#include "semphr.h"

#include "Timer.h"
#include "BlinkLed.h"
#include "Lcd.h"
#include "stm32f429i_discovery_lcd.h"

#define BLUE (0xff<<24) | (0x00<<16) | (0x00<<8) | (0xff<<0)
#define RED (0xff<<24) | (0xff<<16) | (0x00<<8) | (0x00<<0)
#define BLACK (0xff<<24) | (0x00<<16) | (0x00<<8) | (0x00<<0)

/* mm: added helper variable to support FreeRTOS thread list support in gdb
 * Remark: OpenOCD gdbserver references xTopReadyPriority which was removed in newer FreeRTOS releases.
 * The declaration below together with linker flag '-Wl,--undefined=uxTopUsedPriority' solves the problem */
const int __attribute__((used)) uxTopUsedPriority = configMAX_PRIORITIES;

SemaphoreHandle_t LCDSemaphore;

static void CounterTask(void *pvParameters);
static void HeartbeatTask(void *pvParameters);
static void WorkerTask(void *pvParameters);


int main(void)
{
	SysTick->CTRL = 0; // Disable SYSTICK timer to init LCD
	blink_led_init();
	lcd_init();

	LCDSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive( LCDSemaphore );

	xTaskCreate(CounterTask, "CntTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 3), NULL);
	xTaskCreate(HeartbeatTask, "HeartTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 1), NULL);
	xTaskCreate(WorkerTask, "WorkTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 1), NULL);


	vTaskStartScheduler();

	/* the FreeRTOS scheduler never returns to here except when out of memory at creating the idle task */
	for (;;) ;
}


static void CounterTask(__attribute__ ((unused)) void *pvParameters)
{
	int n = 0;
	int take_err = 0;
	UBaseType_t uxHighWaterMark = 0;
	BSP_LCD_DisplayStringAtLine(0, "mc2 Vorlage");

	while (1) {
		char s[20];

		blink_led_on();
		vTaskDelay(1);
		blink_led_off();
		vTaskDelay(1);
		uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
		snprintf(s, sizeof(s), "C=%d S=%d E=%d", n++, (unsigned int)uxHighWaterMark, take_err);
//		snprintf(s, sizeof(s), "C=%d", n++);

		if( xSemaphoreTake( LCDSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
			/* We were able to obtain the semaphore and can now access the
						shared resource. */
			BSP_LCD_SetTextColor(RED);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2, s, CENTER_MODE);
			/* We have finished accessing the shared resource.  Release the
						semaphore. */
			xSemaphoreGive( LCDSemaphore );
		}
		else
		{
			/* We could not obtain the semaphore and can therefore not access
						the shared resource safely. */
			take_err++;
		}
	}
}

static void HeartbeatTask(__attribute__ ((unused)) void *pvParameters)
{
	while (1) {
		blink_led2_on();
		vTaskDelay(100);
		blink_led2_off();
		vTaskDelay(100);
		blink_led2_on();
		vTaskDelay(100);
		blink_led2_off();
		vTaskDelay(700);
	}
}

static void WorkerTask(__attribute__ ((unused)) void *pvParameters)
{
//	lcd_init();
//	while(1);
	vTaskDelay(50);
	while (1) {
		/* See if we can obtain the semaphore.  If the semaphore is not
			available wait 10 ticks to see if it becomes free. */
		if( xSemaphoreTake( LCDSemaphore, ( TickType_t ) 10 ) == pdTRUE )
		{
			/* We were able to obtain the semaphore and can now access the
				shared resource. */
			BSP_LCD_SetTextColor(BLUE);
			BSP_LCD_DisplayStringAtLine(1, "Hi from Worker Task!");
			/* We have finished accessing the shared resource.  Release the
				semaphore. */
			xSemaphoreGive( LCDSemaphore );
		}
		else
		{
			/* We could not obtain the semaphore and can therefore not access
				the shared resource safely. */
		}

	}
}

// ----------------------------------------------------------------------------
