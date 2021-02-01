#include <unistd.h>

#include "camera.hpp"
#include "log.hpp"

int main(int argc, char** argv) {	

	Camera cam;

	int dev_index = 3;

	if (cam.openDevice(dev_index, (Camera::Format){640, 480, \
			Camera::Encoding::YUV420}) != Camera::Error::OK ) {
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