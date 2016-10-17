#include "EERTOS.h"
#include "EERTOSHAL.h"
#include "EERTOS_DEBUG.h"

 char WorkLog[LogBufSize];
 volatile uint16_t LogIndex = 0;
 
void  SendStr(char* buff)
{
	#warning define it!
}


/////// Отладочный кусок.
//Вывод лога работы конечного автомата в буфер памяти, а потом.
//По окончании работы через UART на волю

void WorkLogPutChar(unsigned char symbol)
{
ATOMIC_BLOCK_FORCEON
	{
		if (LogIndex <LogBufSize-10)            // Если лог не переполнен
		{
				WorkLog[LogIndex]= symbol;    // Пишем статус в лог
				LogIndex++;
		}
	}
}

//#warning  -добавить после каждого слова прибавление \r\n для экономии RАМ?
void Put_In_Log (unsigned char * data)
{
  while(*data)
  {
    WorkLogPutChar(*data++);
  }
  /// WorkLogPutChar(10);//\r
   //WorkLogPutChar(13);//\n
}

extern void SendStr(char *Str);
void LogOut(void)				// Выброс логов
{
SuspendRTOS();

  ATOMIC_BLOCK_FORCEON
  {
    if(LogIndex < LogBufSize - 2)
    {
    WorkLog[LogIndex]= '+';
    LogIndex++;
    }
    //USART_Send_Str(SYSTEM_USART, WorkLog);
    SendStr(WorkLog);
    while(LogIndex){WorkLog[LogIndex] = 0; LogIndex--;};
  }
RunRTOS();
//LogIndex = 0;
//FLAG_SET(g_tcf, FLUSH_WORKLOG);
//SetTimerTask(Task_Flush_WorkLog,25,0);//очистка лог буффера
}



//получить размер памяти, статически выделеной под задачи
uint32_t Get_StaticTasksRam(uint16_t amount_of_tasks)
{
return sizeof(TASK_STRUCT)*amount_of_tasks;
}


//=====================================================================================
void DebugLedsInit(void)
{
  GPIO_InitTypeDef    LED_INIT;
  RCC_AHB1PeriphClockCmd((RCC_GIO(A) | RCC_GIO(B) | RCC_GIO(C) | RCC_GIO(D) | RCC_GIO(E)), ENABLE);

  LED_INIT.GPIO_Mode  = GPIO_Mode_OUT;
  LED_INIT.GPIO_OType = GPIO_OType_PP;
  LED_INIT.GPIO_Speed = GPIO_Speed_2MHz;
  LED_INIT.GPIO_PuPd  = GPIO_PuPd_NOPULL;

//Леды сгруппированы по одинаковым портам, в GPIO_Init достаточно указать любой 1 из группы
  LED_INIT.GPIO_Pin = LED_T3_PIN | LED_T4_PIN;
  GPIO_Init(LED_T3_PORT, &LED_INIT);
}

//=====================================================================================
void DebugBtnsInit(void)
{
  GPIO_InitTypeDef    BTN_INIT;
  RCC_AHB1PeriphClockCmd((RCC_GIO(A) | RCC_GIO(B) | RCC_GIO(C) | RCC_GIO(D) | RCC_GIO(E)), ENABLE);

  BTN_INIT.GPIO_Pin   = BTN_PIN;

  BTN_INIT.GPIO_Mode  = GPIO_Mode_IN;
  BTN_INIT.GPIO_OType = GPIO_OType_PP;
  BTN_INIT.GPIO_Speed = GPIO_Speed_2MHz;
  BTN_INIT.GPIO_PuPd  = GPIO_PuPd_NOPULL;

GPIO_Init(BTN_PORT, &BTN_INIT);
}

//=====================================================================================
int BTN_tst(void)
{
  return (BTN_PORT->IDR & BTN_PIN) ? 1 : 0;
}

//=====================================================================================

// ПРИМ - DWT, это отрадочный счетчик STM32.
//        В отличие от TIM2, этот счетчик работает на полной частоте, а не на половинной.
//        ВАЖНО! - DWT необязательный компонент камня. Т.е., этот счетчик может быть не у всех производителей Кортексов.
//                 При портировании этого кода на Кортекс от другого производителя, обратить на это внимание.

//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0439b/BABJFFGJ.html

//DWT registers, код отсюда - http://community.arm.com/message/5153#5153
uint32_t *DWT_CYCCNT  = (uint32_t*)0xE0001004;
uint32_t *DWT_CONTROL = (uint32_t*)0xE0001000;
uint32_t *SCB_DEMCR   = (uint32_t*)0xE000EDFC;

#ifdef __cplusplus
  extern "C" void DWT_CYCCNT_init(void);
  extern "C" uint32_t DWT_val_get(void);
#endif

//=====================================================================================
// код отсюда - http://community.arm.com/message/5153#5153
void DWT_CYCCNT_init(void)
{
        *SCB_DEMCR   = *SCB_DEMCR | 0x01000000;
        *DWT_CYCCNT  = 0;                 // reset the counter
        *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
}

uint32_t DWT_Get(void) //вернуть значение
{
    return *DWT_CYCCNT;
}
uint32_t DWT_Compare(int32_t tp)  //сравнение... используется при организации временной задержки
{
    return (((int32_t)DWT_Get() - tp) < 0);
}

void DWT_Delay(uint32_t ms) // задержка в миллисекундах
{
    int32_t tp = DWT_Get() + ms * (SystemCoreClock/1000);
    while (DWT_Compare(tp));
}

