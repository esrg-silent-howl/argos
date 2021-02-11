#include <malloc.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>

#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <Python.h>

#include "threads.hpp"
#include "camera.hpp"

int main(int argc, char** argv) {	

	//startScheduler();

	Camera cam;

	std::string device = "/dev/video0";

	if (cam.openDevice(device, (Camera::Format){640, 480, \
			Camera::Encoding::MJPEG}) != Camera::Error::OK ) {
		cam.closeDevice();
		return 1;
	}

	cam.start();

	std::string root_dir = "/home/silent/";

	std::string capture_str = root_dir + "Desktop/out/outx.jpg";
	std::string capture_str_rect1 = root_dir + "Desktop/out/outx1.jpg";
	std::string capture_str_rect2 = root_dir + "Desktop/out/outx2.jpg";
	std::string capture_str_final = root_dir + "Desktop/out/outx3.jpg";

	std::string target_dir = root_dir + "Desktop/ObjectDetection/object_detection";
	std::string project_dir = root_dir + "Desktop/ARGOS/argos";
	std::string filename = root_dir + "Desktop/ObjectDetection/object_detection/argos_tflite_detection_image.py";
	std::string args = " --modeldir=TFLite_model --image=gun1.jpeg";
	std::string command_1 = "cd " + target_dir;
	std::string command_2 = "python " + filename + args;
	std::string command_3 = "cd " + project_dir;

	uint32_t start;
	uint32_t end;
	uint32_t offsetx;
	uint32_t offsety;

	for (uint32_t i = 0; i < 7; i++) {
		sleep(1);
		capture_str[28] = (char)(i+0x30);
		capture_str_rect1[28] = (char)(i+0x30);
		capture_str_rect2[28] = (char)(i+0x30);
		capture_str_final[28] = (char)(i+0x30);

		cam.capture(capture_str);

		start = 50*i;
		end = 25*i;
		offsetx = 200;
		offsety = 100;

		// cam.drawRectangle(capture_str, capture_str_rect1, 
		// 		Camera::Point2D(start, end), Camera::Point2D(start+offsetx, 
		// 		end+offsety), Camera::Color(0), Camera::Color(255, 0, 255, 0));
		// cam.drawRectangle(capture_str_rect1, capture_str_rect2, 
		// 		Camera::Point2D(start, end), Camera::Point2D(start+70, end-20),
		// 		Camera::Color(255, 255, 255, 255), Camera::Color(0));
		// cam.drawText(capture_str_rect2, capture_str_final, "gun: 80%", 
		// 		Camera::Point2D(start+5, end-15), 25, 15, Camera::Color(255, 0, 0, 0));
	}

	cam.stop();

	cam.closeDevice();

	const char* _filename = filename.c_str();
	const char*  _target_dir = target_dir.c_str();
	const char*  _project_dir = project_dir.c_str();
 
	// --- Python Plugin ---
	Py_Initialize();

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");

	PyRun_SimpleString("print('[EMBEDDED_PY] Executing Inference Python Script')");
	
	//Run Inference Script
	FILE* file = fopen(_filename, "r");
	PyRun_SimpleFile(file, _filename);

	Py_Finalize();
	// End fo Python Plugin

	// std::vector<std::string> lines;
	// std::vector<int> values1;
	// std::vector<int> values2;
	// std::string temp;
	// std::string box;
	// std::string l_box;
	// std::string label;
	// std::ifstream info;
	// info.open(root_dir + "Desktop/out/model_output.txt");

	// while(std::getline(info,temp)){
	// 	if(temp.size() > 0)
	// 		lines.push_back(temp);
	// }

	// for (int i = 0; i < lines.size(); i++){

	// 	if(lines[i] == std::to_string(i)){
	// 		box = lines[i+1];
	// 		l_box = lines[i+2];
	// 		label = lines[i+3];
	// 	}

	// 	std::string aux;
	// 	for(auto x : box)
	// 		if(x == ' '){
	// 			values1.push_back(std::stoi(aux));
	// 			aux = " ";
	// 		}
	// 		else
	// 			aux += x;

	// 	for(auto x : l_box)
	// 		if(x == ' '){
	// 			values2.push_back(std::stoi(aux));
	// 			aux = " ";
	// 		}
	// 		else
	// 			aux += x;

	// 	capture_str[28] = (char)(i+0x30);

	// 	cam.drawRectangle(capture_str, capture_str_rect1, 
	// 			Camera::Point2D(start, end), Camera::Point2D(start+offsetx, 
	// 			end+offsety), Camera::Color(0), Camera::Color(255, 0, 255, 0));
	// 	cam.drawRectangle(capture_str_rect1, capture_str_rect2, 
	// 			Camera::Point2D(start, end), Camera::Point2D(start+70, end-20),
	// 			Camera::Color(255, 255, 255, 255), Camera::Color(0));
	// 	cam.drawText(capture_str_rect2, capture_str_final, "gun: 80%", 
	// 			Camera::Point2D(start+5, end-15), 25, 15, Camera::Color(255, 0, 0, 0));
	//}

	//info.close();

	return 0;
}