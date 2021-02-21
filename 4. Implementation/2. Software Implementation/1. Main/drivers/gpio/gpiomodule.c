#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/io.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "gpio_util.h"
#include "utils.h"


#define DEVICE_NAME "gpio"
#define CLASS_NAME "GPIO"


MODULE_LICENSE("GPL");

/* Device variables */
static struct class* dev_class = NULL;
static struct device* dev = NULL;
static dev_t dev_majorminor;
static struct cdev c_dev;  // Character device structure

GPIORegisters_t *gpio_regs;

static const int gpio_pin = 4;

ssize_t gpio_write(struct file *pfile, const char __user *pbuff, size_t len, loff_t *off) {
	
	GPIORegisters_t *pdev; 
	
	pr_alert("%s: called (%u)\n",__FUNCTION__,len);
	
	if(unlikely(pfile->private_data == NULL))
		return -EFAULT;

	pdev = (struct GPIORegisters_t *)pfile->private_data;
	
	if (pbuff[0]=='0')
		setGPIOOutputValue(pdev, gpio_pin, 0);
	else
		setGPIOOutputValue(pdev, gpio_pin, 1);

	return len;
}

ssize_t gpio_read(struct file *pfile, char __user *p_buff, size_t len, loff_t *poffset){
	
	int pin_number, pin_value;
	GPIORegisters_t *pdev;

	/* Check for file nullity */
	if(unlikely(pfile->private_data == NULL))
		return -EFAULT;

	/* Fetch peripheral address */
	pdev = (struct GPIORegisters_t *)pfile->private_data;

	/* Get pin number from user string */
	pin_number = strToInt(p_buff);

	/* Assert pin number's validity */
	if (pin_number<0 || pin_number>64)
		return -EBADRQC;

	/* Get pin value */
	pin_value = getGPIOInputValue(pdev, pin_number);

	// copy_to_user(buf, memory_buffer, 1);
	raw_copy_to_user(p_buff, (uint8_t*)&pin_value, 1);
	
	return 0;
}

int gpio_close(struct inode *p_inode, struct file * pfile){
	
	pr_alert("%s: called\n",__FUNCTION__);
	pfile->private_data = NULL;
	return 0;
}


int gpio_open(struct inode* p_indode, struct file *p_file){

	pr_alert("%s: called\n",__FUNCTION__);
	p_file->private_data = (struct GPIORegisters_t *) gpio_regs;
	return 0;
	
}


static struct file_operations device_file_ops = {
	.owner = THIS_MODULE,
	.write = gpio_write,
	.read = gpio_read,
	.release = gpio_close,
	.open = gpio_open,
};

static int __init gpio_module_init(void) {
	int ret;
	struct device *dev_ret;

	pr_alert("%s: called\n",__FUNCTION__);

	if ((ret = alloc_chrdev_region(&dev_majorminor, 0, 1, DEVICE_NAME)) < 0) {
		return ret;
	}

	if (IS_ERR(dev_class = class_create(THIS_MODULE, CLASS_NAME))) {
		unregister_chrdev_region(dev_majorminor, 1);
		return PTR_ERR(dev_class);
	}
	if (IS_ERR(dev_ret = device_create(dev_class, NULL, dev_majorminor, NULL, DEVICE_NAME))) {
		class_destroy(dev_class);
		unregister_chrdev_region(dev_majorminor, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &device_file_ops);
	c_dev.owner = THIS_MODULE;
	if ((ret = cdev_add(&c_dev, dev_majorminor, 1)) < 0) {
		printk(KERN_NOTICE "Error %d adding device", ret);
		device_destroy(dev_class, dev_majorminor);
		class_destroy(dev_class);
		unregister_chrdev_region(dev_majorminor, 1);
		return ret;
	}


	//gpio_regs = (struct GPIORegisters_t *)ioremap(GPIO_BASE, sizeof(struct GPIORegisters_t));
	gpio_regs = (struct GPIORegisters_t *)ioremap_nocache(GPIO_BASE, sizeof(struct GPIORegisters_t));
	
	pr_alert("map to virtual adresse: 0x%x\n", (unsigned)gpio_regs);
	
	setGPIOFunction(gpio_regs, gpio_pin, WRITE); //Output

	return 0;
}

static void __exit gpio_module_exit(void) {
	
	pr_alert("%s: called\n",__FUNCTION__);
	
	setGPIOFunction(gpio_regs, gpio_pin, 0); //Configure the pin as input
	iounmap(gpio_regs);
	cdev_del(&c_dev);
	device_destroy(dev_class, dev_majorminor);
	class_destroy(dev_class);
	unregister_chrdev_region(dev_majorminor, 1);
}

module_init(gpio_module_init);
module_exit(gpio_module_exit);
