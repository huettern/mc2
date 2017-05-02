

#include "stm32f429i_discovery_lcd.h"
#include "system_stm32f4xx.h"



void isr_systick(void)
//void SysTick_Handler(void)
{
  HAL_IncTick();
}


void lcd_init(void)
{
  SystemCoreClockUpdate();
  HAL_Init();

  if (BSP_LCD_Init() !=0)
	  while (1);
  BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER);

  BSP_LCD_SelectLayer(1);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetFont(&Font16);
  HAL_Delay(100);
}

