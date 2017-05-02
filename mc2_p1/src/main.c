/*
 * mc2_p1
 * Noah Huetter
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
#include "stm32f429i_discovery_ts.h"

#define PINK (0xff<<24) | (0xd6<<16) | (0x64<<8) | (0xbe<<0)
#define BLUE (0xff<<24) | (0x00<<16) | (0x00<<8) | (0xff<<0)
#define RED (0xff<<24) | (0xff<<16) | (0x00<<8) | (0x00<<0)
#define BLACK (0xff<<24) | (0x00<<16) | (0x00<<8) | (0x00<<0)
#define WHITE 0xffffffff

#define BALL_RADIUS 10
#define HIT_TOLERANCE 20

/* mm: added helper variable to support FreeRTOS thread list support in gdb
 * Remark: OpenOCD gdbserver references xTopReadyPriority which was removed in newer FreeRTOS releases.
 * The declaration below together with linker flag '-Wl,--undefined=uxTopUsedPriority' solves the problem */
const int __attribute__((used)) uxTopUsedPriority = configMAX_PRIORITIES;

SemaphoreHandle_t LCDSemaphore;
SemaphoreHandle_t BallHitSemaphore;

typedef struct BallPosStruct
{
	int16_t x;
	int16_t y;
} BallPos_t;

QueueHandle_t BallPosQueue;

static void BallTask(void *pvParameters);
static void TouchTask(void *pvParameters);
static void lcd_printf(uint32_t Color, uint16_t X, uint16_t Y, Text_AlignModeTypdef mode, const char *fmt, ...) __attribute__((__format__(__printf__,5,6)));
static BaseType_t isBallPosValid (BallPos_t pos);
static void paintBall(BallPos_t *oldpos, BallPos_t newpos, uint32_t color);

int main(void)
{
	SysTick->CTRL = 0; // Disable SYSTICK timer to init LCD
	blink_led_init();
	lcd_init();
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

	LCDSemaphore = xSemaphoreCreateMutex();
	BallHitSemaphore = xSemaphoreCreateBinary();

	BallPosQueue = xQueueCreate( 2, sizeof( BallPos_t ) );

	xTaskCreate(BallTask, "BallTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 3), NULL);
	xTaskCreate(TouchTask, "TouchTask", (configMINIMAL_STACK_SIZE + 50), NULL, (tskIDLE_PRIORITY + 2), NULL);
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

static BaseType_t isBallPosValid (BallPos_t pos)
{
	if( (pos.x + BALL_RADIUS) < ((int16_t)BSP_LCD_GetXSize()-1) )
	{
		if( (pos.x - BALL_RADIUS) > 0 )
		{
			if( (pos.y + BALL_RADIUS) < ((int16_t)BSP_LCD_GetYSize()-1) )
			{
				if( (pos.y - BALL_RADIUS) > 0 )
				{
					return pdTRUE;	
				}
			}	
		}
	}
	return pdFALSE;
}

static void paintBall(BallPos_t *oldpos, BallPos_t newpos, uint32_t color)
{
	if(isBallPosValid(newpos) == pdFALSE) return;
	if(isBallPosValid(*oldpos) == pdFALSE)
	{
		// if wrong old pos, corect with valid new pos
		// oldpos->x = newpos.x;
		// oldpos->y = newpos.y;
		return;	
	} 
	if( xSemaphoreTake( LCDSemaphore, ( TickType_t ) 10 ) == pdTRUE )
	{
		BSP_LCD_SetTextColor(WHITE);
		BSP_LCD_FillCircle(oldpos->x, oldpos->y, BALL_RADIUS);
		BSP_LCD_SetTextColor(color);
		BSP_LCD_FillCircle(newpos.x, newpos.y, BALL_RADIUS);
		xSemaphoreGive( LCDSemaphore );
		oldpos->x = newpos.x;
		oldpos->y = newpos.y;
	}
}

static void BallTask(__attribute__ ((unused)) void *pvParameters)
{
	BallPos_t oldpos;
	oldpos.x = 20; oldpos.y = 20;
	uint16_t x_rand, y_rand;
	int16_t dx_rand, dy_rand;
	BallPos_t pos, dpos;
	pos.x = 20;
	pos.y = 20;
	dpos.x = 1;
	dpos.y = 1;
	while (1)
	{
		if( xSemaphoreTake( BallHitSemaphore, ( TickType_t ) 20 ) == pdTRUE )
		{
			x_rand = (uint16_t)((float)rand()* (BSP_LCD_GetXSize()-1) / RAND_MAX);
			y_rand = (uint16_t)((float)rand()* (BSP_LCD_GetYSize()-1) / RAND_MAX);
			pos.x = x_rand;
			pos.y = y_rand;
			dx_rand = (int16_t)((float)rand()* (5) / RAND_MAX) - 2;
			dy_rand = (int16_t)((float)rand()* (5) / RAND_MAX) - 2;
			dpos.x = dx_rand;
			dpos.y = dy_rand;
		}
		pos.x += dpos.x;
		pos.y += dpos.y;
		// check x overrun
		if((((pos.x + BALL_RADIUS) > ((int16_t)BSP_LCD_GetXSize()-1))) || ((pos.x - BALL_RADIUS) < 0) )
		{
			dpos.x = -dpos.x;
			pos.x += 2*dpos.x;
		}
		// check y overrun
		if( ((pos.y + BALL_RADIUS) > ((int16_t)BSP_LCD_GetYSize()-1)) || ((pos.y - BALL_RADIUS) < 0) )
		{
			dpos.y = -dpos.y;
			pos.y += 2*dpos.y;
		}
		paintBall(&oldpos, pos, BLUE);
		xQueueSend(BallPosQueue, &pos, 0);
	}
}

static void TouchTask(__attribute__ ((unused)) void *pvParameters)
{
	static BallPos_t oldpos;
	static uint16_t events = 0;
	oldpos.x = 20; oldpos.y = 20;
	BallPos_t bpos, touchpos;
	TS_StateTypeDef tsState;
	while(1)
	{	
		xQueueReceive(BallPosQueue, &bpos, -1);
		BSP_TS_GetState(&tsState);
		if(tsState.TouchDetected)
		{
			touchpos.x = tsState.X;
			touchpos.y = tsState.Y;
			paintBall(&oldpos, touchpos, RED);
			if( (touchpos.x <= bpos.x+HIT_TOLERANCE) && (touchpos.x >= bpos.x-HIT_TOLERANCE)
				&& (touchpos.y <= bpos.y+HIT_TOLERANCE) && (touchpos.y >= bpos.y-HIT_TOLERANCE) )
			{
				xSemaphoreGive( BallHitSemaphore );
				events++;
			}
			lcd_printf(BLACK, 8, 0, RIGHT_MODE, "x=%03d y=%03d", touchpos.x, touchpos.y);
		}
		lcd_printf(BLACK, 0, 0, LEFT_MODE, "hit=%d", events);
		vTaskDelay(100);
	}
}
// ----------------------------------------------------------------------------
