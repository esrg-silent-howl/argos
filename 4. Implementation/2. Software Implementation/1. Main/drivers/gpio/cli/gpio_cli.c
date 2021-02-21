#include "../gpiomodule.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>


typedef enum GPIOFunction {

	INPUT=0, OUTPUT//, ALT0, ALT1, ALT2, ALT3, ALT4, ALT5

} GPIOFunction_t;

typedef struct GPIOPinFunction {

	unsigned int pin;
	GPIOFunction_t function;

} GPIOPinFunction_t ;


int main(int argc, char** argv) {

	GPIOPinFunction_t pin_assign17 = {.pin = 17, .function = OUTPUT};
	GPIOPinFunction_t pin_assign27 = {.pin = 27, .function = INPUT};
	
	int fd = open("/dev/gpio", O_RDWR);
	char input[5] = {0};

	if (fd == -1) {
		printf("Could not open file\n");
		return -1;
	}

	printf("Open successful\n");

	int result = ioctl(fd, GPIO_IOCTL_FUNCTION, &pin_assign17);

	if (result == -EBADRQC) {
		printf("Could not assign function\n");
		return -2;
	}

	printf("IOCTL successful\n");


	result = ioctl(fd, GPIO_IOCTL_FUNCTION, &pin_assign27);

	if (result == -EBADRQC) {
		printf("Could not assign function\n");
		return -2;
	}

	printf("IOCTL successful\n");


	result = write(fd, "17,0", 4);
	
	if (result != 4) {
		printf("Could not output value\n");
		return -2;
	}

	printf("Write successful\n");

	
	result = read(fd, input, 27);

	if (result != 0) {
		printf("Could not get value\n");
		return -2;
	}

	printf("Read: %s\n", input);

	
	sleep(1);

	result = write(fd, "17,1", 4);
	
	if (result != 4) {
		printf("Could not output value\n");
		return -2;
	}

	printf("Write successful\n");

	result = read(fd, input, 27);

	if (result != 0) {
		printf("Could not get value\n");
		return -2;
	}

	printf("Read: %s\n", input);

	close(fd);
	
	return 0;
}