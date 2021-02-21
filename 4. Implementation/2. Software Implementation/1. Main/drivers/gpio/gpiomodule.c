#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/io.h>
//#include <linux/kernel.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "gpio_util.h"
#include "utils.h"
#include "gpiomodule.h"

#define DEVICE_NAME "gpio"
#define CLASS_NAME "GPIO"

#define MAX_PIN_NUMBER	28


MODULE_LICENSE("GPL");

/* Device variables */
static struct class* dev_class = NULL;
static struct device* dev = NULL;
static dev_t dev_majorminor;
static struct cdev c_dev;  // Character device structure

GPIORegisters_t *gpio_regs;

ssize_t gpio_write(struct file *pfile, const char __user *pbuff, size_t len, loff_t *off) {
	
	int pin_number, pin_value;
	GPIORegisters_t *pdev;
	char request[7];
	
	pr_alert("%s: called (%u)\n",__FUNCTION__,len);
	
	/* Check for file nullity */
	if(unlikely(pfile->private_data == NULL))
		return -EFAULT;

	/* Parse input */
	pr_alert("%s:pbuff.str = %s\n", __FUNCTION__, pbuff);
	sscanf(pbuff, "%u,%u", &pin_number, &pin_value);
	printk("%s: parsed values: pin_number = %u, pin_value = %u\n", __FUNCTION__, pin_number, pin_value);

	/* Assert pin number's validity */
	if (pin_number<0 || pin_number>=MAX_PIN_NUMBER)
		return -EBADRQC;
	
	/* Fetch peripheral address */
	pdev = (struct GPIORegisters_t *)pfile->private_data;
	
	/* Set pin value */
	setGPIOOutputValue(pdev, pin_number, pin_value == 0 ? 0 : 1);

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

	/* Get pin number Fetch peripheral addressfrom user string */
	// pin_number = strToInt(p_buff);

	pin_number = len;
	printk("%s: pin number: %d\n", __FUNCTION__, pin_number);

	/* Assert pin number's validity */
	if (pin_number<0 || pin_number>=MAX_PIN_NUMBER)
		return -EBADRQC;

	/* Get pin value */
	pin_value = getGPIOInputValue(pdev, pin_number);

	printk("%s: pin value: %d\n", __FUNCTION__, pin_value);

	// copy_to_user(buf, memory_buffer, 1);
	raw_copy_to_user(p_buff, (uint8_t*)&pin_value, 1);
	
	return 0;
}

long gpio_ioctl(struct file* pfile, uint32_t request, long unsigned int value) {

	GPIORegisters_t *pdev;

	pr_alert("%s: called\n",__FUNCTION__);

	switch (request) {
	
	case GPIO_IOCTL_FUNCTION:

		pr_alert("%s: Defining GPIO function \n",__FUNCTION__);

		/* Check for file nullity */
		if(unlikely(pfile->private_data == NULL))
			return -EFAULT;

		/* Fetch peripheral address */
		pdev = (struct GPIORegisters_t *)pfile->private_data;
		
		setGPIOFunction(pdev, value);
		return 0;

	default:
		pr_alert("%s: Invalid request\n",__FUNCTION__);
		return -EBADRQC;	
	}
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
	.unlocked_ioctl = gpio_ioctl
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
	
	return 0;
}

static void __exit gpio_module_exit(void) {
	
	pr_alert("%s: called\n",__FUNCTION__);
	
	iounmap(gpio_regs);
	cdev_del(&c_dev);
	device_destroy(dev_class, dev_majorminor);
	class_destroy(dev_class);
	unregister_chrdev_region(dev_majorminor, 1);
}

module_init(gpio_module_init);
module_exit(gpio_module_exit);