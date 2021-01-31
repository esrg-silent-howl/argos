#include <unistd.h>

#include "camera.hpp"
#include "log.hpp"

int main(int argc, char** argv) {	

	Camera cam;

	if (cam.openDevice("/dev/video0", (Camera::Format){640, 480, \
			Camera::Encoding::MJPEG}) != Camera::Error::OK ) {
		cam.closeDevice();
	}

	cam.start();

	string capture_str = "/home/josep/Desktop/foutx.jpg";

	for (uint32_t i = 5; i > 0; i--) {
		sleep(1);
		capture_str[24] = (char)(i+0x30);
		cam.capture(capture_str);
	}

	cam.stop();

	cam.closeDevice();
}