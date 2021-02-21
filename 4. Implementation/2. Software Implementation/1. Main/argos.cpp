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

#define FRAME_GRAB 7
#define THREAT_THRESHOLD 0

static Camera cam;

static std::string root_dir = "/home/silent/";
static std::string capture_str = root_dir + "Desktop/out/outx.jpg";
static std::string capture_str_rect1 = root_dir + "Desktop/out/outx1.jpg";
static std::string capture_str_rect2 = root_dir + "Desktop/out/outx2.jpg";
static std::string capture_str_final = root_dir + "Desktop/out/outx3.jpg";

static bool camera_init = false;
static bool capture_done = false;
static bool notify_rc = false;
static bool threads_created = false;
static bool threat_found = false;
static bool thread_unblocked[2] = {false, false};

/* Static Condition Variables */
static pthread_cond_t cCameraInit = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cThreatFoundDB = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cThreatFoundCloud = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cCaptureDone = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cThreadsCreated = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cNotification = PTHREAD_COND_INITIALIZER;

static pthread_cond_t cInferenceStopped = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cTerminate = PTHREAD_COND_INITIALIZER;

/* Static Mutexes */
static pthread_mutex_t mCamera = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mNotifyCache = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mFrameCache = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mMetaCacheDB = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mMetaCacheCloud = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mCameraWait = PTHREAD_MUTEX_INITIALIZER;

static void split_str(std::string str, std::vector<std::string>& aux){

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

	/* Blocks on camera wait mutex waiting for tMain to create the other threads */
    pthread_mutex_lock(&mCameraWait);
    while (!threads_created){
        /* Automatically unlocks mutex when signalled */
        if (pthread_cond_wait(&cThreadsCreated, &mCameraWait) > 0)
            perror("[tCameraControl] pthread_cond_wait failed on cThreadsCreated");    
    }

    /* Start Camera */
    cam.start();

	/* Sample frames periodically */
    for (uint32_t i = 0; i < FRAME_GRAB; i++) {

        sleep(1);
        capture_str[28] = (char)(i+0x30);

		cam.capture(capture_str);
	}

	cam.stop();
	cam.closeDevice();

	capture_done = true;
    /* Signal the condition variable */
	if (pthread_cond_signal(&cCaptureDone) > 0)
		perror("[tCameraControl] pthread_cond_signal failed on cCaptureDone");

    while(true){

        /* Adjust camera exposure */

		// perror("[tCameraControl] Infinite loop");
		// sleep(3);
    }

    pthread_exit(NULL);
}

static void* inferenceHandler(void* arg){
    perror("[ARGOS] Starting tMLInference");

	std::string filename = root_dir + "Desktop/ARGOS/argos/argos_tflite_detection_image.py";
	
	/* Transform the directory string into a C string */
    const char* _filename = filename.c_str();

	/* Blocks on frame cache mutex waiting for the camera captures */
    pthread_mutex_lock(&mFrameCache);
    while (!capture_done){
        /* Automatically unlocks mutex when signalled */
        if (pthread_cond_wait(&cCaptureDone, &mFrameCache) > 0)
            perror("[tMLInference] pthread_cond_wait failed on cCaptureDone");    
    }
	/* Clear shared variable */
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
			
			int ymin = atoi(aux.at(1).c_str());
			int xmin = atoi(aux.at(2).c_str());
			int ymax = atoi(aux.at(3).c_str());
			int xmax = atoi(aux.at(4).c_str());

			std::string label = aux.at(5) + " " + aux.at(6); 

			cam.drawRectangle(capture_str, capture_str, 
					Camera::Point2D(xmin, ymin), Camera::Point2D(xmax, 
					ymax), Camera::Color(0), Camera::Color(255, 0, 255, 0));
			cam.drawRectangle(capture_str, capture_str, 
					Camera::Point2D(xmin, ymin), Camera::Point2D(xmin+70, ymin-20),
					Camera::Color(255, 255, 255, 255), Camera::Color(0));
			cam.drawText(capture_str, capture_str, label, 
					Camera::Point2D(xmin+5, ymin-15), 25, 15, Camera::Color(255, 0, 0, 0));

	}

	if(lines.size() > THREAT_THRESHOLD){

		threat_found = true;

		/* Condition variable broadcast would be better
		for signalling the three threads */

		/* Signal the condition variable */
		if (pthread_cond_signal(&cThreatFoundCloud) > 0)
			perror("[tMLInference] pthread_cond_signal failed on cThreatFoundCloud");

		/* Signal the condition variable */
		if (pthread_cond_signal(&cThreatFoundDB) > 0)
			perror("[tMLInference] pthread_cond_signal failed on cThreatFoundDB");

		notify_rc = true;	
		/* Signal the condition variable */
		if (pthread_cond_signal(&cNotification) > 0)
			perror("[tMLInference] pthread_cond_signal failed on cNotification");
	}

    while(true){

		if(thread_unblocked[0] && thread_unblocked[1])
			threat_found = false;

		// perror("[tMLInference] Infinite loop");
		// sleep(3);

    }

    pthread_exit(NULL);
}

static void* d_rclientHandler(void* arg){
    perror("[d_tRemoteClient] Starting d_tRemoteClient");

	/* Send threat notification to the remote client */
	perror("[d_tRemoteClient] Last notification sent");

	pthread_exit(NULL);
}

static void* rclientHandler(void* arg){
    perror("[ARGOS] Starting tRemoteClient");

	/* Listen for connection */

    while(true){
		
		/* Blocks on notify cache mutex waiting for a threat detection */
		if(pthread_mutex_lock(&mNotifyCache) > 0)
			perror("[tRemoteClient] pthread_mutex_lock failed on mNotifyCache");
		while (!threat_found){
			/* Automatically unlocks mutex when signalled */
			if (pthread_cond_wait(&cNotification, &mNotifyCache) > 0)
				perror("[tRemoteClient] pthread_cond_wait failed on cNotification");    
		}
		/* Clear shared variable */
		notify_rc = false;
		
		perror("[tRemoteClient] Remote client notification trial"); 

		/* Remote client daughter thread setup */
		Thread::Config rc_daughter;
		const Thread* rc_daughter_thread;
		rc_daughter.name = "d_tRemoteClient";
		rc_daughter.priority = NORMAL;
		rc_daughter.args = NULL;
		rc_daughter.start_routine = d_rclientHandler;

		/* Remote client thread creation */
		rc_daughter_thread = const_cast<Thread*>(createThread(rc_daughter));

		if(pthread_join(rc_daughter_thread->native, NULL) > 0)
			perror("[tRemoteClient] pthread_join on d_tRemoteClient failed");
		else 
			perror("[tRemoteClient] pthread_join on d_tRemoteClient");
    }

    pthread_exit(NULL);
}

static void* d_cloudHandler(void* arg){
    perror("[d_tCloud] Starting d_tCloud");

	/* Submmit Frames to cloud */
	perror("[tCloud] Last Frames uploaded");

	pthread_exit(NULL);
}

static void* cloudHandler(void* arg){
    perror("[ARGOS] Starting tCloud");

	/* Request connection */

    while(true){

		/* Blocks on metadata cache mutex waiting for a threat detection */
		if(pthread_mutex_lock(&mMetaCacheCloud) > 0)
			perror("[tCloud] pthread_mutex_lock failed on mMetaCacheCloud");
		while (!threat_found){
			/* Automatically unlocks mutex when signalled */
			if (pthread_cond_wait(&cThreatFoundCloud, &mMetaCacheCloud) > 0)
				perror("[tCloud] pthread_cond_wait failed on cThreatFoundCloud");    
		}
		/* Clear shared variable */
		thread_unblocked[0] = true;

		perror("[tCloud] Frame upload trial"); 

		/* Cloud daughter thread setup */
		Thread::Config cloud_daughter;
		const Thread* cloud_daughter_thread;
		cloud_daughter.name = "d_tCloud";
		cloud_daughter.priority = ABOVE_NORMAL;
		cloud_daughter.args = NULL;
		cloud_daughter.start_routine = d_cloudHandler;

		/* Cloud daughter thread creation */
		cloud_daughter_thread = const_cast<Thread*>(createThread(cloud_daughter));

		if(pthread_join(cloud_daughter_thread->native, NULL) > 0)
			perror("[tCloud] pthread_join on d_tCloud failed");
		else 
			perror("[tCloud] pthread_join on d_tCloud");

	}

	pthread_exit(NULL);
}

static void* d_databaseHandler(void* arg){
    perror("[d_tDatabase] Starting d_tDatabase");

	/* Update database */
	perror("[d_tDatabase] Updated database");

	pthread_exit(NULL);
}

static void* databaseHandler(void* arg){
    perror("[ARGOS] Starting tDatabase");

	/* Request connection */

    while(true){	
			
			/* Blocks on metadata cache mutex waiting for a threat detection */
			if(pthread_mutex_lock(&mMetaCacheDB) > 0)
				perror("[tDatabase] pthread_mutex_lock failed on mMetaCacheDB");
			while (!threat_found){
				/* Automatically unlocks mutex when signalled */
				if (pthread_cond_wait(&cThreatFoundDB, &mMetaCacheDB) > 0)
					perror("[tDatabase] pthread_cond_wait failed on cThreatFoundDB");    
			}
			/* Clear shared variable */
			thread_unblocked[1] = true;

			perror("[tDatabase] Database update trial");

			/* Database daughter thread setup */
			Thread::Config db_daughter;
			const Thread* db_daughter_thread;
			db_daughter.name = "d_tDatabase";
			db_daughter.priority = BELOW_NORMAL;
			db_daughter.args = NULL;
			db_daughter.start_routine = d_databaseHandler;

			/* Database daughter thread creation */
			db_daughter_thread = const_cast<Thread*>(createThread(db_daughter));

			if(pthread_join(db_daughter_thread->native, NULL) > 0)
				perror("[tDatabase] pthread_join on d_tDatabase failed");
			else 
				perror("[tDatabase] pthread_join on d_tDatabase");
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
    pthread_mutex_lock(&mCamera);
    while (!camera_init){
        /* Automatically unlocks mutex when signalled */
        if (pthread_cond_wait(&cCameraInit, &mCamera) > 0)
            perror("[tMain] pthread_cond_wait failed on cCameraInit");    
    }

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

	threads_created = true;
    /* Signal the condition variable */
    if (pthread_cond_signal(&cThreadsCreated) > 0)
        perror("[tMain] pthread_cond_signal failed on cThreadsCreated");

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