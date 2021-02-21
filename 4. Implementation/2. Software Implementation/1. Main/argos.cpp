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

static Camera cam;

static std::string root_dir = "/home/silent/";
static std::string capture_str = root_dir + "Desktop/out/outx.jpg";
static std::string capture_str_rect1 = root_dir + "Desktop/out/outx1.jpg";
static std::string capture_str_rect2 = root_dir + "Desktop/out/outx2.jpg";
static std::string capture_str_final = root_dir + "Desktop/out/outx3.jpg";

static bool camera_init = false;
static bool capture_done = false;

static uint32_t start;
static uint32_t end;
static uint32_t offsetx;
static uint32_t offsety;

/* Static Condition Variables */
static pthread_cond_t cTerminate = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cCameraInit = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cInferenceStopped = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cThreatFound = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cCaptureDone = PTHREAD_COND_INITIALIZER;

/* Static Mutexes */
static pthread_mutex_t mCamera = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mNotifyCache = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mFrameCache = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mMetaCache = PTHREAD_MUTEX_INITIALIZER;

static void split_str(std::string str, std::vector<std::string>& aux){

	// std::vector<std::string> aux;

    std::string word = ""; 
    for (auto x : str)  
    { 
        if (x == ' ') 
        { 	
			aux.push_back(word);
            word = ""; 
        } 
        else { 
            word = word + x; 
        } 
    }
	aux.push_back(word);
} 


static void* cameraHandler(void* arg){
    perror("[ARGOS] Starting tCameraControl");

    /* Init I2C and light sensor */

	std::string device = "/dev/video0";
    std::string target_dir = root_dir + "Desktop/ObjectDetection/object_detection";
	std::string project_dir = root_dir + "Desktop/ARGOS/argos";

    /* Transform the directory strings into C strings */
	const char*  _target_dir = target_dir.c_str();
	const char*  _project_dir = project_dir.c_str();

    /* Prepare the camera */ 
	if (cam.openDevice(device, (Camera::Format){640, 480, \
			Camera::Encoding::MJPEG}) != Camera::Error::OK ) {
		cam.closeDevice();
		return (void*)1;
	}

    camera_init = true;
    /* Signal the condition variable */
    if (pthread_cond_signal(&cCameraInit) > 0)
        perror("[tCameraControl] pthread_cond_signal failed on cCameraInit");

    /* Start Camera */
    cam.start();

	/* Sample frames periodically */
    for (uint32_t i = 0; i < 14; i++) {
        sleep(1);
        capture_str[28] = (char)(i+0x30);
        capture_str_rect1[28] = (char)(i+0x30);
        capture_str_rect2[28] = (char)(i+0x30);
        capture_str_final[28] = (char)(i+0x30);

		/* Lock the mutex for the frame cache */
		// int frame_mtx_status_lock = pthread_mutex_lock(&mFrameCache);

		cam.capture(capture_str);

		/* 	Unlock the mutex for the frame cache */
    	// int frame_mtx_status_unlock = pthread_mutex_unlock(&mFrameCache);

	}

	cam.stop();
	cam.closeDevice();

	capture_done = true;
    /* Signal the condition variable */
	if (pthread_cond_signal(&cCaptureDone) > 0)
		perror("[tCameraControl] pthread_cond_signal failed on cCaptureDone");

    while(true){

        /* Adjust camera exposure */

		perror("[tCameraControl] Infinite loop");
		sleep(3);
    }

    pthread_exit(NULL);
}

static void* inferenceHandler(void* arg){
    perror("[ARGOS] Starting tMLInference");

	std::string filename = root_dir + "Desktop/ARGOS/argos/argos_tflite_detection_image.py";
	int cam_cond_wait_status, cap_cond_wait_status;
	
	/* Transform the directory strings into C strings */
    const char* _filename = filename.c_str();

	/* Blocks on camera mutex waiting for the camera init */
    pthread_mutex_lock(&mCamera);
    while (!camera_init){
        /* Automaticlly unlocks mutex and locks it when signalled */
        // cam_cond_wait_status = pthread_cond_wait(&cCameraInit, &mCamera);
        if (pthread_cond_wait(&cCameraInit, &mCamera) > 0)
            perror("[tMLInference] pthread_cond_wait failed on mCamera");    
    }
    pthread_mutex_unlock(&mCamera);

	/* Blocks on camera mutex waiting for the camera captures */
    pthread_mutex_lock(&mFrameCache);
    while (!capture_done){
        /* Automaticlly unlocks mutex and locks it when signalled */
        // cap_cond_wait_status = pthread_cond_wait(&cCaptureDone, &mFrameCache);
        if (pthread_cond_wait(&cCaptureDone, &mFrameCache) > 0)
            perror("[tMLInference] pthread_cond_wait failed on mFrameCache");    
    }
    pthread_mutex_unlock(&mFrameCache);
	capture_done = false;

	/* Python Plugin */
	Py_Initialize();

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");

	PyRun_SimpleString("print('[tMLInference] Executing Inference Python Script')");

	/* Run Inference Script */
	FILE* file = fopen(_filename, "r");
	PyRun_SimpleFile(file, _filename);

	/* End of Python Plugin */
	Py_Finalize();

	std::vector<std::string> lines;
	std::vector<std::string> aux;
	std::string temp;	

	std::ifstream info;

	info.open(root_dir + "Desktop/model_output.txt");
	if(info.is_open()){
		perror("[tMLInference] Model output file opened");

		while(std::getline(info,temp)){
			if(temp.size() > 0)
				lines.push_back(temp);
		}
	}
	info.close();

	for(size_t i = 0; i < lines.size(); i++){
		std::vector<std::string> aux;
		split_str(lines.at(i), aux);

			int img = atoi(aux.at(0).c_str());
			capture_str[28] = (char)(img+0x30);

			//f.write(str(index) + " " + str(ymin) + " " + str(xmin) + " " + str(ymax) + " " + str(xmax) + " " + str(label) +"\n")
			
			int ymin = atoi(aux.at(1).c_str());
			int xmin = atoi(aux.at(2).c_str());
			int ymax = atoi(aux.at(3).c_str());
			int xmax = atoi(aux.at(4).c_str());

			std::string label = aux.at(5) + " " + aux.at(6); 

			cam.drawRectangle(capture_str, capture_str, 
					Camera::Point2D(xmin, ymin), Camera::Point2D(xmax, 
					ymax), Camera::Color(0), Camera::Color(255, 0, 255, 0));
			cam.drawRectangle(capture_str, capture_str, 
					Camera::Point2D(xmin, ymin), Camera::Point2D(xmin+70, ymin-20), //-20
					Camera::Color(255, 255, 255, 255), Camera::Color(0));
			cam.drawText(capture_str, capture_str, label, 
					Camera::Point2D(xmin+5, ymin-15), 25, 15, Camera::Color(255, 0, 0, 0));

	}

    while(true){

		perror("[tMLInference] Infinite loop");
		sleep(3);

    }

    pthread_exit(NULL);
}

static void* rclientHandler(void* arg){
    perror("[ARGOS] Starting tRemoteClient");

	/* Listen for connection */

    while(true){

	// perror("Listening for Remote Client");
	// sleep(2);

    }

    pthread_exit(NULL);
}

static void* cloudHandler(void* arg){
    perror("[ARGOS] Starting tCloud");

	/* Request connection */

    while(true){

	// perror("Requesting Cloud Connection");
	// sleep(2);

    }

    pthread_exit(NULL);
}

static void* databaseHandler(void* arg){
    perror("[ARGOS] Starting tDatabase");

	/* Request connection */

    while(true){

	// perror("Requesting Database Connection");
	// sleep(2);

    }

    pthread_exit(NULL);
}

static void* mainHandler(void* arg){
    perror("[ARGOS] Starting tMain");

	/* Inference thread setup */
	Thread::Config inference;
	Thread* inference_thread;
	inference.name = "tMLInference";
	inference.priority = HIGH;
	inference.args = NULL;
	inference.start_routine = inferenceHandler;

	/* Inference thread creation */
	inference_thread = const_cast<Thread*>(createThread(inference));

	/* Camera control thread setup */
	Thread::Config camera_ctrl;
	Thread* cam_ctrl_thread;
	camera_ctrl.name = "tCameraControl";
	camera_ctrl.priority = REAL_TIME;
	camera_ctrl.args = NULL;
	camera_ctrl.start_routine = cameraHandler;

	/* Camera control thread creation */
	cam_ctrl_thread = const_cast<Thread*>(createThread(camera_ctrl));

    /* Attempt to connect the network */

	/* Blocks on camera mutex waiting for the camera init */
    int cam_mtx_status_lock = pthread_mutex_lock(&mCamera);
    while (!camera_init){
        /* Automaticlly unlocks mutex and locks it when signalled */
        int cond_wait_status = pthread_cond_wait(&cCameraInit, &mCamera);
        if (cond_wait_status > 0)
            perror("[ARGOS] pthread_cond_wait failed on tMain:");    
    }
    int cam_mtx_status_unlock = pthread_mutex_unlock(&mCamera);

	/* Remote client thread setup */
	Thread::Config remote_client;
	Thread* remote_client_thread;
	remote_client.name = "tRemoteClient";
	remote_client.priority = ABOVE_NORMAL;
	remote_client.args = NULL;
	remote_client.start_routine = rclientHandler;

	/* Remote client thread creation */
	remote_client_thread = const_cast<Thread*>(createThread(remote_client));

	/* Cloud thread setup */
	Thread::Config cloud;
	Thread* cloud_thread;
	cloud.name = "tCloud";
	cloud.priority = NORMAL;
	cloud.args = NULL;
	cloud.start_routine = cloudHandler;

	/* Cloud thread creation */
	cloud_thread = const_cast<Thread*>(createThread(cloud));

	/* Database thread setup */
	Thread::Config database;
	Thread* database_thread;
	database.name = "tDatabase";
	database.priority = BELOW_NORMAL;
	database.args = NULL;
	database.start_routine = databaseHandler;

	/* Database thread creation */
	database_thread = const_cast<Thread*>(createThread(database));

    while(true){

    }

    pthread_exit(NULL);
}


// extern "C" void* mainRoutine (void* arg) {

// 	uint32_t answer = *(static_cast<uint32_t*>(arg));

// 	std::cout << "What is the answer to the universe?" << std::endl;
// 	sleep(2);
// 	std::cout << "Suspense..." << std::endl;
// 	sleep(2);
// 	std::cout << answer << std::endl;

// 	pthread_exit(arg);
// }

int main(int argc, char** argv) {	

	/* perror is thread-safe */
    perror("[ARGOS] Starting");

	Thread::Config main;
	Thread* main_thread;
	// uint32_t answer = 42;
	// uint32_t* answer_ptr = &answer;

	main.name = "tMain";
	main.priority = LOW;
	main.args = NULL;
	main.start_routine = mainHandler;

	main_thread = const_cast<Thread*>(createThread(main));
	// pthread_join(test_thread->native, (void**)(&answer_ptr));
	// if (*answer_ptr != 42) {
	// 	std::cout << "You little shit, you've learned nothing!" << std::endl;
	// } else {
	// 	std::cout << "Ah, I see you are a man of culture as well" << std::endl;
	// }

	while (true)
	{
		//break;
	}

	perror("[ARGOS] Shutting Down");

	return 0;
}