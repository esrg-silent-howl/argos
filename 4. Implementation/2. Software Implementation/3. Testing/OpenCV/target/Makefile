# CC =	gcc
CC =	~/Desktop/buildroot-2020.11.1/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-g++
RM = /bin/rm -f
objects = opencv_test

OPENCVLIBS = \
-L/home/josep/Desktop/buildroot-2020.11.1/output/target/usr/lib32 \
-L/home/josep/Desktop/buildroot-2020.11.1/output/build/opencv3-3.4.12/buildroot-build/lib \
-L/home/josep/Desktop/buildroot-2020.11.1/output/target/usr/lib -lopencv_videoio -lopencv_imgcodecs -lopencv_core \
OPENCVFLAGS = \
-I/home/josep/Desktop/buildroot-2020.11.1/output/build/opencv3-3.4.12/include/ \
-I/home/josep/Desktop/buildroot-2020.11.1/output/build/opencv3-3.4.12/modules/videoio/include/ \
-I/home/josep/Desktop/buildroot-2020.11.1/output/build/opencv3-3.4.12/modules/core/include/ \
-I/home/josep/Desktop/buildroot-2020.11.1/output/build/opencv3-3.4.12/modules/imgcodecs/include/ \
-I/home/josep/Desktop/buildroot-2020.11.1/output/build/opencv3-3.4.12/buildroot-build/include
#-I~/Desktop/buildroot-2020.11.1/output/build/opencv3-3.4.12/include/opencv
#-I~/Desktop/buildroot-2020.11.1/output/build/opencv3-3.4.12/include

CPPFLAGS = $(OPENCVFLAGS)
LDLIBS = $(OPENCVLIBS)

#CPPFLAGS = `pkg-config --cflags arm-opencv`
#LDLIBS = `pkg-config --libs arm-opencv`

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CPPFLAGS) $(LDLIBS) -lpthread -o  $@ $< 
clean:
	${RM} *.o

