#include <malloc.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

#include "camera.hpp"

int main(int argc, char** argv) {	

	Camera cam;

	std::string device = "/dev/video0";

	if (cam.openDevice(device, (Camera::Format){640, 480, \
			Camera::Encoding::MJPEG}) != Camera::Error::OK ) {
		cam.closeDevice();
		return 1;
	}

	cam.start();

	std::string capture_str = "/home/josep/Desktop/out/outx.jpg";
	std::string capture_str_rect1 = "/home/josep/Desktop/out/outx1.jpg";
	std::string capture_str_rect2 = "/home/josep/Desktop/out/outx2.jpg";
	std::string capture_str_final = "/home/josep/Desktop/out/outx3.jpg";

	uint32_t start;
	uint32_t end;
	uint32_t offsetx;
	uint32_t offsety;

	for (uint32_t i = 0; i < 7; i++) {
		sleep(1);
		capture_str[27] = (char)(i+0x30);
		capture_str_rect1[27] = (char)(i+0x30);
		capture_str_rect2[27] = (char)(i+0x30);
		capture_str_final[27] = (char)(i+0x30);

		cam.capture(capture_str);

		start = 50*i;
		end = 25*i;
		offsetx = 200;
		offsety = 100;

		cam.drawRectangle(capture_str, capture_str_rect1, 
				Camera::Point2D(start, end), Camera::Point2D(start+offsetx, 
				end+offsety), Camera::Color(0), Camera::Color(255, 0, 255, 0));
		cam.drawRectangle(capture_str_rect1, capture_str_rect2, 
				Camera::Point2D(start, end), Camera::Point2D(start+70, end-20),
				Camera::Color(255, 255, 255, 255), Camera::Color(0));
		cam.drawText(capture_str_rect2, capture_str_final, "gun: 80%", 
				Camera::Point2D(start+5, end-15), 25, 15, Camera::Color(255, 0, 0, 0));
	}

	cam.stop();

	cam.closeDevice();

	return 0;
}