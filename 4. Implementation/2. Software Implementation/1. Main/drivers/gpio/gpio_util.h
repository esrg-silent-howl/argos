#ifndef _GPIO_UTIL_H
#define _GPIO_UTIL_H

#include <linux/types.h>

#define BCM2711_PERI_BASE       0xfe000000
#define GPIO_BASE (BCM2711_PERI_BASE + 0x200000) // GPIO controller


typedef enum GPIOFunction {

	INPUT=0, OUTPUT//, ALT0, ALT1, ALT2, ALT3, ALT4, ALT5

} GPIOFunction_t;

typedef struct GPIOPinFunction {

	uint32_t pin;
	GPIOFunction_t function;

} GPIOPinFunction_t ;

typedef struct GPIORegisters_t {

	uint32_t GPFSEL[6];
	uint32_t reserved1;
	uint32_t GPSET[2];
	uint32_t reserved2;
	uint32_t GPCLR[2];
	uint32_t reserved3;
	uint32_t GPLEV[2];
	uint32_t reserved4;
	uint32_t GPEDS[2];
	uint32_t reserved5;
	uint32_t GPREN[2];
	uint32_t reserved6;
	uint32_t GPFEN[2];
	uint32_t reserved7;
	uint32_t GPHEN[2];
	uint32_t reserved8;
	uint32_t GPLEN[2];
	uint32_t reserved9;
	uint32_t GPAREN[2];
	uint32_t reserved10;
	uint32_t GPAFEN[2];
	uint32_t reserved11;
	uint32_t GPIO_PUP_PDN_CNTRL_REG[4];	

} GPIORegisters_t;

void setGPIOFunction(struct GPIORegisters_t *gpio_regs, GPIOPinFunction_t* data);

void setGPIOOutputValue(struct GPIORegisters_t *gpio_regs, int gpio, bool out_val);
uint32_t getGPIOInputValue(struct GPIORegisters_t *gpio_regs, int gpio);

#endif
