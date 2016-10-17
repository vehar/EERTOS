#ifndef EERTOS_DEBUG_H
#define EERTOS_DEBUG_H

#include "..\platform.h"

//portability F4->F3 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define RCC_AHB1PeriphClockCmd RCC_AHBPeriphClockCmd
#define RCC_GIO(x) RCC_AHBPeriph_GPIO##x


#define _LED1_ON  STM_EVAL_LEDOn(LED3); //LED_PORT|=(1<<LED1)
#define _LED1_OFF STM_EVAL_LEDOff(LED3); //LED_PORT&=~(1<<LED1)


#define  LED_T9_PIN  GPIO_Pin_12 //
#define  LED_T9_PORT GPIOE
#define  LED_T10_PIN  GPIO_Pin_13 //
#define  LED_T10_PORT GPIOE


#define _LED_ON(placement, x)      GPIO_SetBits   (LED_##placement####x##_PORT, LED_##placement####x##_PIN)
#define _LED_OFF(placement, x)     GPIO_ResetBits (LED_##placement####x##_PORT, LED_##placement####x##_PIN)
#define _LED_TOGGLE(placement, x)  GPIO_ToggleBits(LED_##placement####x##_PORT, LED_##placement####x##_PIN)
//================================================================================
//Example:     _LED_ON(B,1);        _LED_OFF(T,1);        _LED_TOGGLE(T,1);        _LED_TOGGLE(B,1);

#define RED_UP          3
#define BLUE_UP         4
#define ORANGE_UP       5
#define GREEN_L         6
#define GREEN_R         7
#define ORANGE_DOWN     8
#define BLUE_DOWN       9
#define RED_DOWN        10


#define BTN_PIN                GPIO_Pin_0
#define BTN_PORT               GPIOA
#define BTN_CLK                RCC_AHBPeriph_GPIOA
#define BTN_EXTI_LINE          EXTI_Line0
#define BTN_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define BTN_EXTI_PIN_SOURCE    EXTI_PinSource0
#define BTN_EXTI_IRQn          EXTI0_IRQn

#define LogBufSize 3064 //Размер буффера для логов

void DebugLedsInit(void);
void DebugBtnsInit(void);

int BTN_tst(void);
void DWT_CYCCNT_init(void);
uint32_t DWT_val_get(void);

uint32_t DWT_Get(void);
uint32_t DWT_Compare(int32_t tp);
void DWT_Delay(uint32_t us);


void WorkLogPutChar(unsigned char symbol);
void Put_In_Log (unsigned char * data);
void LogOut(void);


//получить размер памяти, статически выделеной под задачи
uint32_t Get_StaticTasksRam(uint16_t amount_of_tasks);

#endif
