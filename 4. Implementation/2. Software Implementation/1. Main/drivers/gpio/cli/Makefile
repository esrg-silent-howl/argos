# CC =	gcc
CC = ~/Desktop/buildroot-2020.11.1/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-gcc
RM = /bin/rm -f
objects = gpio_cli
all: $(objects)

$(objects): %: %.c
	$(CC) $(CFLAGS) -lpthread -o  $@ $<
clean:
	${RM} *.o gpio_cli

