#include "gpio_util.h"
#include <linux/module.h>

void setGPIOFunction(struct GPIORegisters_t *gpio_regs, GPIOPinFunction_t* data) {
	
	int gpio = data->pin;
	int function_code = data->function;

	int register_index = gpio / 10;		/* GPFSEL register */
	int bit = (gpio % 10) * 3;			/* Position of first bit in register */

	unsigned old_value = gpio_regs->GPFSEL[register_index];
	unsigned mask = 0b111 << bit;

	/* Inform of results */
	pr_alert("%s: register index is %d\n",__FUNCTION__,register_index);
	pr_alert("%s: mask is 0x%x\n",__FUNCTION__,mask);
	pr_alert("%s: update value is 0x%x\n",__FUNCTION__,((function_code << bit) & mask));
	
	/* Keep the old value of GPFSELx generally but write the function code bits */
	gpio_regs->GPFSEL[register_index] = (old_value & ~mask) | ((function_code << bit) & mask);
}

void setGPIOOutputValue(struct GPIORegisters_t *gpio_regs, int gpio, bool output_value) {

	pr_alert("%s: register value is 0x%x\n",__FUNCTION__,(1<<(gpio %32)));

	if (gpio_regs == NULL) 
		return;
			 
	if (output_value)
		gpio_regs->GPSET[gpio / 32] = (1 << (gpio % 32));
	else
		gpio_regs->GPCLR[gpio / 32] = (1 << (gpio % 32));
}

uint32_t getGPIOInputValue(struct GPIORegisters_t *gpio_regs, int gpio) {

	if (gpio_regs == NULL) 
		return -EFAULT;

	printk("%s: value: %d\n", __FUNCTION__, (gpio_regs->GPLEV[gpio/32] >> gpio%32) & 1);

	return (gpio_regs->GPLEV[gpio/32] >> gpio%32) & 1;
}