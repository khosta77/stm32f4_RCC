#include "../system/include/cmsis/stm32f4xx.h"

// System Clocks
#define CPU_PLL_CLOCK			168000000		// 168 MHz
#define APB1_CLOCK				42000000		//  42 MHz
#define APB2_CLOCK				84000000		//  84 MHz

void System_Clock_Config(void);
void SysTick_Init(void);

void System_Clock_Config(void)
{	
	
	// Prescaler Configrations
	RCC->CFGR 	|= (5 << 10);			// APB1 Prescaler = 4
	RCC->CFGR 	|= (4 << 13);			// APB2 Prescaler = 2

	RCC->CR 	|= (1 << 16);			// HSE Clock Enable - HSEON
	while(!(RCC->CR & 0x00020000));		// Wait until HSE Clock Ready - HSERDY

	// PLL Configrations
	RCC->PLLCFGR = 0;					// Clear all PLLCFGR register
	RCC->PLLCFGR |=  (8		<<  0);		// PLLM = 8;
	RCC->PLLCFGR |=  (336 	<<  6);		// PLLN = 336;
	RCC->PLLCFGR &= ~(3 	<< 16);		// PLLP = 2; // For 2, Write 0
	RCC->PLLCFGR |=  (1 	<< 22);		// HSE Oscillator clock select as PLL
	RCC->PLLCFGR |=  (7 	<< 24);		// PLLQ = 7;

	RCC->CR 		|=  (1 		<< 24); // PLL Clock Enable - PLLON
	while(!(RCC->CR & 0x02000000)); 	// Wait until PLL Clock Ready - PLLRDY

	// Flash Configrations
	FLASH->ACR = 0;						// Clear all ACR register (Access Control Register)
	FLASH->ACR 		|= (5 <<  0); 		// Latency - 5 Wait State
	FLASH->ACR 		|= (1 <<  9);		// Instruction Cache Enable
	FLASH->ACR 		|= (1 << 10);		// Data Cache Enable

	RCC->CFGR 		|= (2 <<  0);		// PLL Selected as System Clock
	while((RCC->CFGR & 0x0F) != 0x0A); 	// Wait PLL On

}

/* SysTick Configrations */
void SysTick_Init(void)
{
	SysTick->CTRL = 0;									// Close SysTick for configrations
	//SysTick->LOAD = 0x00014821;				// 0.5ms counter
	//SysTick->LOAD = 0x00029041;				// 1ms counter
	SysTick->LOAD = 0x000000A8;					// 1us counter
	SysTick->VAL  = 0;									// SysTick Current Value Register 
	SysTick->CTRL |= (1 << 2);					// Clock source selection, 1 = AHB Source
	SysTick->CTRL |= (1 << 1);					// SysTick exception request enable (SysTick Interrupt)
	SysTick->CTRL |= (1 << 0);					// Counter enable
}

#define HIGH		1
#define LOW			0

void GPIO_Init(GPIO_TypeDef * GPIOx, uint8_t GPIO_Pin)
{
	GPIOx->MODER 		|=  (1 << (2 * GPIO_Pin));		// GPIO Pin 12
	
	// Output State
	GPIOx->OSPEEDR 	|=  (3 << (2 * GPIO_Pin));		// Output Speed - Very High Mode
	GPIOx->OTYPER 	&= ~(1 << (1 * GPIO_Pin));		// Output Type = Push-pull
	GPIOx->PUPDR 		&= ~(3 << (2 * GPIO_Pin));		// Pull-up/down Mode = No pull up/down
}

void GPIO_Write(GPIO_TypeDef * GPIOx, uint8_t GPIO_Pin, uint8_t State)
{
	if(State == 1)
	{
		GPIOx->ODR |= (1 << GPIO_Pin);
	} else if(State == 0) {
		GPIOx->ODR &= ~(3 << GPIO_Pin);
	} else {
		GPIOx->ODR &= ~(3 << GPIO_Pin);
	}
}

/*
	Systick Handler (Interrupt Function)
*/
__IO uint32_t Delay_Time;
void SysTick_Handler(void)
{
  if(Delay_Time !=0)
	{
		Delay_Time --;
	}
}         

// Microsecond Delay (us)
void Delay(uint32_t time)
{
	Delay_Time = time;
	while(Delay_Time !=0);
}

int main(void)
{
	//System_Clock_Config();
	SysTick_Init();
	
	RCC->AHB1ENR |=  (1 << 3);			// GPIOD Peripheral clock enable
	
	GPIO_Init(GPIOD, 12);
	
	while (1)
	{
		GPIO_Write(GPIOD, 12, HIGH);
		Delay(1000000);						// 1 second
		
		GPIO_Write(GPIOD, 12, LOW);
		Delay(1000000);						// 1 second
	}
	
	return 0;
}


