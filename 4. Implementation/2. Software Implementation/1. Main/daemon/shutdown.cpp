#include "shutdown.hpp"
#include "_threads.hpp"
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

void daemonSetup(){

    /* Fork off the parent process and create a new child process */
    switch (fork()){
    case -1: exit(EXIT_FAILURE);    // An error occurred
    case 0: break;                  // Child proceeds   
    default: exit(EXIT_SUCCESS);    // While parent terminates
    }

	/*Start a new session ( setsid() )*/
    /* On success: The child process becomes session leader now detached 
	from its controlling terminal (CTTY)*/
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

	/* Redundancy Fork*/
    /* Fork off for the second time to ensure that the daemon doesn't
	have access to the TTY again (it's not the session leader)*/
    switch (fork()){
    case -1: exit(EXIT_FAILURE);    // An error occurred
    case 0: break;                  // Child proceeds   
    default: exit(EXIT_SUCCESS);    // While parent terminates
    }

    /* Set new file permissions by changing the file mode mask:
	may vary depending on the daemon's permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    // int x;
    // for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    // {
    //     close (x);
    // }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);

    /* Open the log file */
    openlog ("shutdownDaemon", LOG_PID, LOG_DAEMON);

}

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

    // int frame_mtx_status_lock = pthread_mutex_lock(&mFrameCache);
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
        //         Camera::Point2D(start, end), Camera::Point2D(start+offsetx, 
        //         end+offsety), Camera::Color(0), Camera::Color(255, 0, 255, 0));
        // cam.drawRectangle(capture_str_rect1, capture_str_rect2, 
        //         Camera::Point2D(start, end), Camera::Point2D(start+70, end-20),
        //         Camera::Color(255, 255, 255, 255), Camera::Color(0));
        // cam.drawText(capture_str_rect2, capture_str_final, "gun: 80%", 
        //         Camera::Point2D(start+5, end-15), 25, 15, Camera::Color(255, 0, 0, 0));
	}

    cam.stop();
	cam.closeDevice();
    
	// --- Python Plugin ---
    // int frame_mtx_status_lock = pthread_mutex_lock(&mFrameCache);
	Py_Initialize();

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");

	PyRun_SimpleString("print('[ARGOS] Executing Inference Python Script')");
	
	//Run Inference Script
	FILE* file = fopen(_filename, "r");
	PyRun_SimpleFile(file, _filename);

	Py_Finalize();
    // int frame_mtx_status_unlock = pthread_mutex_unlock(&mFrameCache);
	// --- End of Python Plugin ---

    while(true){

        // Sample frames periodically
        #ifdef RELEASE
        //stuff 
        #endif

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

    threads tCameraControl("tCameraControl", 50, cameraHandler, NULL);
    threads tMLInference("tMLInference", 60, inferenceHandler, NULL);

    // Attempt to connect the network

    int cam_mtx_status_lock = pthread_mutex_lock(&mCamera);
    while (!camera_init){
        // Automaticlly unlocks mutex and locks it when signalled
        int cond_wait_status = pthread_cond_wait(&cCameraInit, &mCamera);
        if (cond_wait_status > 0)
            perror("[ARGOS] pthread_cond_wait failed on tMain:");    
    }
    int cam_mtx_status_unlock = pthread_mutex_unlock(&mCamera);

    threads tRemoteClient("tRemoteClient", 40, rclientHandler, NULL);
    threads tCloud("tCloud", 30, cloudHandler, NULL);
    threads tDatabase("tDatabase", 20, databaseHandler, NULL);

    while(true){

    }

    pthread_exit(NULL);
}

void startScheduler(){

    // Creating and starting the main thread
    // Priority from 0-99
    threads tMain("tMain", 10, mainHandler, NULL);
}


int main(){

    // perror is thread-safe
    perror("[ARGOS] Starting");

    // Daemon Setup
    daemonSetup();
    perror("[ARGOS] Shutdown Daemon Up and Running");
    syslog (LOG_NOTICE, "[ARGOS] Shutdown Daemon Up and Running");
    startScheduler();

    // Daemon Main Thread
    //syslog (LOG_NOTICE, "Shutdown Daemon: Starting Main Thread.");
    closelog();

    while (1){

        // Check Button pressed and send SIGTERM

    }

    syslog (LOG_NOTICE, "[ARGOS] Shutdown Daemon Terminated");
    closelog();
    return EXIT_SUCCESS;
}

/* ps -xj | grep shutdownDaemon 
   grep shutdownDaemon /var/log/syslog */