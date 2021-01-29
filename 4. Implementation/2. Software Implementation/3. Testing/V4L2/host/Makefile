# CC =	gcc
CC = g++
RM = /bin/rm -f
objects = main
all: $(objects)

$(objects): %: %.cpp
	$(CC) -lpthread -o  $@ $<
clean:
	${RM} *.o hello

