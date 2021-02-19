/* C++ includes */
#include <fstream>
#include <iostream>
#include <vector>
#include <cinttypes>

/* C includes */
#include <malloc.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <Python.h>

/* Project includes */
#include "threads.hpp"
#include "camera.hpp"

//#define RELEASE

// static Camera cam;

static std::string root_dir = "/home/silent/";
static std::string capture_str = root_dir + "Desktop/out/outx.jpg";
static std::string capture_str_rect1 = root_dir + "Desktop/out/outx1.jpg";
static std::string capture_str_rect2 = root_dir + "Desktop/out/outx2.jpg";
static std::string capture_str_final = root_dir + "Desktop/out/outx3.jpg";

static bool camera_init = false;

static uint32_t start;
static uint32_t end;
static uint32_t offsetx;
static uint32_t offsety;

// Static Condition Variables
static pthread_cond_t cTerminate = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cCameraInit = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cInferenceStopped = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cThreatFound = PTHREAD_COND_INITIALIZER;

// Static Mutexes
static pthread_mutex_t mCamera = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mNotifyCache = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mFrameCache = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mMetaCache = PTHREAD_MUTEX_INITIALIZER;

static void* cameraHandler(void* arg){
    perror("[ARGOS] Starting tCameraControl");

    // Init I2C and light sensor

    while(true){

        // Adjust camera exposure
    }

    pthread_exit(NULL);
}

static void* inferenceHandler(void* arg){
    perror("[ARGOS] Starting tMLInference");

    Camera cam;

	std::string device = "/dev/video0";
    std::string target_dir = root_dir + "Desktop/ObjectDetection/object_detection";
	std::string project_dir = root_dir + "Desktop/ARGOS/argos";
	std::string filename = root_dir + "Desktop/ObjectDetection/object_detection/argos_tflite_detection_image.py";
	// std::vector<std::string> lines;
	// std::vector<int> values1;
	// std::vector<int> values2;
	// std::string temp;
	// std::string box;
	// std::string l_box;
	// std::string label;
	// std::ifstream info;


    // Transform the directory strings into C strings
    const char* _filename = filename.c_str();
	const char*  _target_dir = target_dir.c_str();
	const char*  _project_dir = project_dir.c_str();

    // Prepare the camera 
	if (cam.openDevice(device, (Camera::Format){640, 480, \
			Camera::Encoding::MJPEG}) != Camera::Error::OK ) {
		cam.closeDevice();
		return (void*)1;
	}

    camera_init = true;
    // Signal the condition variable
    int cond_sig_status = pthread_cond_signal(&cCameraInit);
        if (cond_sig_status > 0)
            perror("[ARGOS] pthread_cond_signal failed on tMLInference");  

    // Start Camera
    cam.start();

	    // Sample frames periodically
	int frame_mtx_status_lock = pthread_mutex_lock(&mFrameCache);
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

	}
    
	// --- Python Plugin ---
	Py_Initialize();

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");

	PyRun_SimpleString("print('[ARGOS] Executing Inference Python Script')");
	
	//Run Inference Script
	FILE* file = fopen(_filename, "r");
	PyRun_SimpleFile(file, _filename);

	Py_Finalize();
	// --- End of Python Plugin ---
    int frame_mtx_status_unlock = pthread_mutex_unlock(&mFrameCache);

	cam.stop();
	cam.closeDevice();

    while(true){

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

    }

    pthread_exit(NULL);
}

static void* rclientHandler(void* arg){
    perror("[ARGOS] Starting tRemoteClient");

    while(true){

    }

    pthread_exit(NULL);
}

static void* cloudHandler(void* arg){
    perror("[ARGOS] Starting tCloud");

    while(true){

    }

    pthread_exit(NULL);
}

static void* databaseHandler(void* arg){
    perror("[ARGOS] Starting tDatabase");

    while(true){

    }

    pthread_exit(NULL);
}

static void* mainHandler(void* arg){
    perror("[ARGOS] Starting tMain");

    // Thread tCameraControl("tCameraControl", 50, cameraHandler, NULL);
    // Thread tMLInference("tMLInference", 60, inferenceHandler, NULL);

    // Attempt to connect the network

    int cam_mtx_status_lock = pthread_mutex_lock(&mCamera);
    while (!camera_init){
        // Automaticlly unlocks mutex and locks it when signalled
        int cond_wait_status = pthread_cond_wait(&cCameraInit, &mCamera);
        if (cond_wait_status > 0)
            perror("[ARGOS] pthread_cond_wait failed on tMain:");    
    }
    int cam_mtx_status_unlock = pthread_mutex_unlock(&mCamera);

    // Thread tRemoteClient("tRemoteClient", 40, rclientHandler, NULL);
    // Thread tCloud("tCloud", 30, cloudHandler, NULL);
    // Thread tDatabase("tDatabase", 20, databaseHandler, NULL);

    while(true){

    }

    pthread_exit(NULL);
}


extern "C" void* mainRoutine (void* arg) {

	uint32_t answer = *(static_cast<uint32_t*>(arg));

	std::cout << "What is the answer to the universe?" << std::endl;
	sleep(2);
	std::cout << "Suspense..." << std::endl;
	sleep(2);
	std::cout << answer << std::endl;

	pthread_exit(arg);
}

void startScheduler(){

    // Creating and starting the main thread
    // Priority from 0-99
    // Thread tMain("tMain", 10, mainHandler, NULL);
}

int main(int argc, char** argv) {	

	// perror is thread-safe
    perror("[ARGOS] Starting");

	//startScheduler();

	Thread::Config config;
	Thread* test_thread;
	uint32_t answer = 42;
	uint32_t* answer_ptr = &answer;

	config.name = "Test thread";
	config.priority = 2;
	config.args = &answer;
	config.start_routine = mainRoutine;

	test_thread = const_cast<Thread*>(createThread(config));
	pthread_join(test_thread->native, (void**)(&answer_ptr));
	if (*answer_ptr != 42) {
		std::cout << "You little shit, you've learned nothing!" << std::endl;
	} else {
		std::cout << "Ah, I see you are a man of culture as well" << std::endl;
	}

	while (true)
	{
		break;
	}

	perror("[ARGOS] Shutting Down");

	return 0;
}