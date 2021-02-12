#include "shutdown.hpp"
#include "_threads.hpp"
#include "camera.hpp"

static std::string root_dir = "/home/silent/";
static bool camera_init = false;

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

    //pid_t pid;

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
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    /* Open the log file */
    openlog ("shutdownDaemon", LOG_PID, LOG_DAEMON);

}

static void* inferenceHandler(void* arg){
    printf("Starting tMLInference\n");

    int mtx_status_lock = pthread_mutex_lock(&mCamera);
    while (!camera_init){
        // Automaticlly unlocks mutex and locks it when signalled
        int cond_wait_status = pthread_cond_wait(&cCameraInit, &mCamera);
        if (!cond_wait_status)
            printf("pthread_cond wait failed on tMLInference");    
    }
    int mtx_status_unlock = pthread_mutex_unlock(&mCamera);

    std::string target_dir = root_dir + "Desktop/ObjectDetection/object_detection";
	std::string project_dir = root_dir + "Desktop/ARGOS/argos";
	std::string filename = root_dir + "Desktop/ObjectDetection/object_detection/argos_tflite_detection_image.py";

    while(true){

    }
    
    return (void*)0;
}

static void* cameraHandler(void* arg){
    printf("Starting tCameraControl\n");

    // Init I2C and light sensor

    // Prepare camera 
    Camera cam;

	std::string device = "/dev/video0";

	if (cam.openDevice(device, (Camera::Format){640, 480, \
			Camera::Encoding::MJPEG}) != Camera::Error::OK ) {
		cam.closeDevice();
		return (void*)1;
	}

    camera_init = true;
    // Signal the condition variable
    int cond_sig_status = pthread_cond_signal(&cCameraInit);

    // Start Camera
    cam.start();

	std::string capture_str = root_dir + "Desktop/out/outx.jpg";
	std::string capture_str_rect1 = root_dir + "Desktop/out/outx1.jpg";
	std::string capture_str_rect2 = root_dir + "Desktop/out/outx2.jpg";
	std::string capture_str_final = root_dir + "Desktop/out/outx3.jpg";

    for (uint32_t i = 0; i < 7; i++) {
		    sleep(1);
		    capture_str[28] = (char)(i+0x30);
		    capture_str_rect1[28] = (char)(i+0x30);
		    capture_str_rect2[28] = (char)(i+0x30);
		    capture_str_final[28] = (char)(i+0x30);

		    cam.capture(capture_str);
	}

    cam.stop();
	cam.closeDevice();

    while(true){

        // for (uint32_t i = 0; i < 7; i++) {
		//     sleep(1);
		//     capture_str[28] = (char)(i+0x30);
		//     capture_str_rect1[28] = (char)(i+0x30);
		//     capture_str_rect2[28] = (char)(i+0x30);
		//     capture_str_final[28] = (char)(i+0x30);

		//     cam.capture(capture_str);
	    // }

    }

    // cam.stop();
	// cam.closeDevice();

    return (void*)0;
}

static void* rclientHandler(void* arg){
    printf("Starting tRemoteClient\n");

    while(true){

    }

    return (void*)0;
}

static void* cloudHandler(void* arg){
    printf("Starting tCloud\n");

    while(true){

    }

    return (void*)0;
}

static void* databaseHandler(void* arg){
    printf("Starting tDatabase\n");

    while(true){

    }

    return (void*)0;
}

static void* mainHandler(void* arg){
    printf("Starting tMain\n");

    threads tMLInference("tMLInference", 60, inferenceHandler, NULL);
	threads tCameraControl("tCameraControl", 50, cameraHandler, NULL);

    // Attempt to connect the network

    int mtx_status_lock = pthread_mutex_lock(&mCamera);
    while (!camera_init){
        // Automaticlly unlocks mutex and locks it when signalled
        int cond_wait_status = pthread_cond_wait(&cCameraInit, &mCamera);
        if (!cond_wait_status)
            printf("pthread_cond wait failed on tMain");    
    }
    int mtx_status_unlock = pthread_mutex_unlock(&mCamera);

    threads tRemoteClient("tRemoteClient", 40, rclientHandler, NULL);
    threads tCloud("tCloud", 30, cloudHandler, NULL);
    threads tDatabase("tDatabase", 20, databaseHandler, NULL);

    while(true){

    }

    return (void*)0;
}

void startScheduler(){

    // Creating and starting the main thread
    // Priority from 0-99
    threads tMain("tMain", 10, mainHandler, NULL);
}


int main()
{   
    // Daemon Setup
    daemonSetup();
    syslog (LOG_NOTICE, "Shutdown Daemon: Up and Running.");
    startScheduler();

    // Daemon Main Thread
    syslog (LOG_NOTICE, "Shutdown Daemon: Starting Main Thread.");
    closelog();

    while (1){

        // Check Button pressed and send SIGTERM

    }

    //syslog (LOG_NOTICE, "Shutdown Daemon Terminated.");
    //closelog();
    return EXIT_SUCCESS;
}

/* ps -xj | grep shutdownDaemon 
   grep shutdownDaemon /var/log/syslog */