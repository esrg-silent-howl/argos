# CC =	gcc
CC = g++
RM = /bin/rm -f

SOURCES = main.cpp camera.cpp
LIBS = -lpthread
OUT = main

all:
	$(CC) $(SOURCES) -o $(OUT) $(LIBS)
debug:
	$(CC) -g $(SOURCES) -o $(OUT) $(LIBS)
clean:
	${RM} *.o

