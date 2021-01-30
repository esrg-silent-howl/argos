#include "camera.hpp"
#include "log.hpp"

int main(int argc, char** argv) {	

	/* Log testing *//*
	Log log;
	log.error << "Isto e assim, alguma coisa correu mal aqui e foi no ficheiro" << std::endl;
	log.info << "Ta tudo no controlo, parceiro" << std::endl;
	log.warning << "Cuidaaado que a coisa ainda pode dar para o torto" << std::endl;
	log::error << "Isto e assim, alguma coisa correu mal aqui e foi no ficheiro" << std::endl;
	log::info << "Ta tudo no controlo, parceiro "  << dev_name << std::endl;
	log::warning << "Cuidaaado que a coisa ainda pode dar para o torto" << std::endl;*/
	
	Camera cam;

	if (cam.openDevice("/dev/video0", (Camera::Format){640, 480, \
			Camera::Encoding::MJPEG}) != Camera::Error::OK ) {
		cam.closeDevice();
	}

}