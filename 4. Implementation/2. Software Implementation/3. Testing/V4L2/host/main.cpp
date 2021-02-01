#include <unistd.h>

#include "camera.hpp"
#include "log.hpp"

int main(int argc, char** argv) {	

	Camera cam;

	std::string device = "/dev/video0";

	if (cam.openDevice(device, (Camera::Format){640, 480, \
			Camera::Encoding::MJPEG}) != Camera::Error::OK ) {
		cam.closeDevice();
		return 1;
	}

	cam.start();

	string capture_str = "/home/josep/Desktop/out/outx.jpg";

	for (uint32_t i = 0; i < 7; i++) {
		sleep(1);
		capture_str[27] = (char)(i+0x30);
		cam.capture(capture_str);
	}

	cam.stop();

	cam.closeDevice();
}