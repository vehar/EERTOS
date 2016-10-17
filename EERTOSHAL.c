#include "EERTOSHAL.h"

//+++++++++++++PRIVATE RTOS INT VARS+++++++++++++++++++++++++++++++
 uint32_t CPSR_;
 bit	       global_nointerrupted_flag;
 U_ALU_INT      global_interrupt_mask;
 U_ALU_INT      global_interrupt_cond;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#ifdef AVR_CORE
	//RTOS Запуск системного таймера
	void RTOS_timer_init (void)
	{
		TCNT2 = 0;                                // Установить начальное значение счётчиков
		OCR2  = LO(RtosTimerDivider);         		// Установить значение в регистр сравнения
		TIMSK = (0<<TOIE2)|(1<<OCIE2);
	}

	void RunRTOS (void)
	{
		ATOMIC_BLOCK_FORCEON
		{
				TCCR2 = (1<<WGM21)|(1<<CS22)|(0<<CS20)|(0<<CS21);    // Freq = CK/256 - Установить режим и предделитель
					TIMSK = (0<<TOIE2)|(1<<OCIE2);
		}// Разрешаем прерывание RTOS - запуск ОС // Автосброс после достижения регистра сравнения
	}

		#warning поэкспериментировать с переменным RtosTimerDivider в SuspendRTOS и в зависимости от задач(возможно их приоритета)
	//RTOS увеличение предделителя системного таймера
	void SuspendRTOS (void)//Фактически снижение частоты системного таймера
	{
		ATOMIC_BLOCK_FORCEON
		{
			TCCR2 = (0<<CS21)|(1<<CS22)|(1<<CS20); // Freq = CK/1024
		}
	}

	//RTOS Остановка системного таймера
	void FullStopRTOS (void)
	{
		ATOMIC_BLOCK_FORCEON
		{
		TCCR2 = 0;                        // Сбросить режим и предделитель
		TIMSK = (0<<TOIE2)|(0<<OCIE2);	 // запрещаем прерывание RTOS - остановка ОС
		}
	}

	void DeadTimerInit (void)
	{
		TCCR0 = (1<<WGM01)|(1<<CS02)|(0<<CS00)|(0<<CS01);
		TCNT0=0x00;
		OCR0=LO(DeadTimerDivider);
		TIMSK = (0<<TOIE0)|(1<<OCIE0);
	}
#endif


#ifdef ARM_CORE

	#define TM3_INIT_VALUE 0x000000FF
	//RTOS Запуск системного таймера
	void RTOS_timer_init (void)
	{
			NVIC_InitTypeDef         NVIC_InitStructure;
			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
			NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
			NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
			NVIC_Init(&NVIC_InitStructure);

			TIM_TimeBaseStructure.TIM_Period        = 1000;//TM3_INIT_VALUE;
			TIM_TimeBaseStructure.TIM_Prescaler     = 0;
			TIM_TimeBaseStructure.TIM_ClockDivision = 0;
			TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Down;
			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

			TIM_PrescalerConfig  (TIM3, 0, TIM_PSCReloadMode_Immediate);
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);   // Без этого сразу входит в обработчик, даже без наступления нужного события

			TIM3->CNT = TM3_INIT_VALUE;
			//TIM_Cmd(TIM3, ENABLE);
	}

	void RunRTOS (void)
	{
		ATOMIC_BLOCK_FORCEON
		{
			TIM_Cmd(TIM3, ENABLE);
		}
	}

		#warning поэкспериментировать с переменным RtosTimerDivider в StopRTOS и в зависимости от задач(возможно их приоритета)
	//RTOS увеличение предделителя системного таймера
	void SuspendRTOS (void)//Фактически снижение частоты системного таймера
	{
		ATOMIC_BLOCK_FORCEON
		{
			#warning temporary
			FullStopRTOS();
		}
	}

	//RTOS Остановка системного таймера
	void FullStopRTOS (void)
	{
		ATOMIC_BLOCK_FORCEON
		{
			TIM_Cmd(TIM3, DISABLE);
		}
	}

	void DeadTimerInit (void)
	{
		#warning FOR wdt
	}

#endif
