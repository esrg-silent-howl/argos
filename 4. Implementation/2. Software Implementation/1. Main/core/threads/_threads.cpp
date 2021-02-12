#include "_threads.hpp"

threads::threads(std::string pt_name, unsigned int pt_prio, void* (*pt_fun)(void*), void* args){

    this->pt_name = pt_name;
    this->pt_id = pt_id;
    this->pt_fun = pt_fun;

    int s1 = pthread_attr_init(&(this->pt_attr));
    printf("attr init: %d\n", s1);
    int s2 = pthread_attr_getschedpolicy (&(this->pt_attr), &(this->pt_policy));
    printf("attr getshedpolicy: %d\n", s2);
    int s3 = pthread_attr_getschedparam (&(this->pt_attr), &(this->pt_param));
    printf("attr getshedparam: %d\n", s3);

    int rr_min_priority, rr_max_priority;

    int s4 = pthread_attr_setschedpolicy (&(this->pt_attr), SCHED_RR);
    printf("attr setshedpolicy: %d\n", s4);
    rr_min_priority = sched_get_priority_min (SCHED_RR);
	rr_max_priority = sched_get_priority_max (SCHED_RR);
    this->pt_param.sched_priority = pt_prio;

    printf ("SCHED_RR priority range is %d to %d: using %d\n",rr_min_priority,rr_max_priority,this->pt_param.sched_priority);

    int s5 = pthread_attr_setschedparam (&(this->pt_attr), &(this->pt_param));
    printf("attr setshedparam: %d\n", s5);

    printf ("Creating %s thread at RR/%d\n",this->pt_name.c_str() ,this->pt_param.sched_priority);
    //int s6 = pthread_attr_setinheritsched (&(this->pt_attr), PTHREAD_EXPLICIT_SCHED);
    //printf("attr setinheritshed: %d\n", s6);

    this->pt_status = pthread_create (&(this->pt_id), &(this->pt_attr), this->pt_fun, args);
    printf("pthread create: %d\n", this->pt_status);

	if(this->pt_status)
	{
		if(this->pt_status==EPERM)
			fprintf(stderr,"pthread_create() got EPERM\n");
		else if(this->pt_status==EINVAL)
      			fprintf(stderr,"pthread_create() got EINVAL\n");
    		else
      			fprintf(stderr,"pthread_create() got neither EPERM nor EINVAL\n");

    		fprintf(stderr,"pthread_create() got error %d\n",this->pt_status);
    		errno=this->pt_status;
    		perror("pthread_create()");
    		exit(1);
  	}
}

threads::~threads(){}

// // Static Condition Variables
// static pthread_cond_t cTerminate = PTHREAD_COND_INITIALIZER;
// static pthread_cond_t cCameraInit = PTHREAD_COND_INITIALIZER;
// static pthread_cond_t cInferenceStopped = PTHREAD_COND_INITIALIZER;
// static pthread_cond_t cThreatFound = PTHREAD_COND_INITIALIZER;

// // Static Mutexes
// static pthread_mutex_t mCamera = PTHREAD_MUTEX_INITIALIZER;
// static pthread_mutex_t mNotifyCache = PTHREAD_MUTEX_INITIALIZER;
// static pthread_mutex_t mFrameCache = PTHREAD_MUTEX_INITIALIZER;
// static pthread_mutex_t mMetaCache = PTHREAD_MUTEX_INITIALIZER;

// // dummy bool
// bool camera_init = false;

// static void* inferenceHandler(void* arg){
//     printf("Starting tMLInference\n");

//     // maybe not needed

//     int mtx_status_lock = pthread_mutex_lock(&mCamera);
//     while (!camera_init){
//         // Automaticlly unlocks mutex and locks it when signalled
//         int cond_wait_status = pthread_cond_wait(&cCameraInit, &mCamera);
//         if (!cond_wait_status)
//             printf("pthread_cond wait failed on tMLInference");    
//     }
//     int mtx_status_unlock = pthread_mutex_unlock(&mCamera);

//     while(true){

//     }
    
//     return (void*)0;
// }

// static void* cameraHandler(void* arg){
//     printf("Starting tCameraControl\n");

//     // Init I2C and light sensor

//     // Prepare camera 

//     int cond_sig_status = pthread_cond_signal(&cCameraInit);

//     while(true){

//     }

//     return (void*)0;
// }

// static void* rclientHandler(void* arg){
//     printf("Starting tRemoteClient\n");

//     while(true){

//     }

//     return (void*)0;
// }

// static void* cloudHandler(void* arg){
//     printf("Starting tCloud\n");

//     while(true){

//     }

//     return (void*)0;
// }

// static void* databaseHandler(void* arg){
//     printf("Starting tDatabase\n");

//     while(true){

//     }

//     return (void*)0;
// }

// static void* mainHandler(void* arg){
//     printf("Starting tMain\n");

//     threads tMLInference("tMLInference", 60, inferenceHandler, NULL);
// 	threads tCameraControl("tCameraControl", 50, cameraHandler, NULL);

//     // Attempt to connect the network

//     int mtx_status_lock = pthread_mutex_lock(&mCamera);
//     while (!camera_init){
//         // Automaticlly unlocks mutex and locks it when signalled
//         int cond_wait_status = pthread_cond_wait(&cCameraInit, &mCamera);
//         if (!cond_wait_status)
//             printf("pthread_cond wait failed on tMain");    
//     }
//     int mtx_status_unlock = pthread_mutex_unlock(&mCamera);

//     threads tRemoteClient("tRemoteClient", 40, rclientHandler, NULL);
//     threads tCloud("tCloud", 30, cloudHandler, NULL);
//     threads tDatabase("tDatabase", 20, databaseHandler, NULL);

//     while(true){

//     }

//     return (void*)0;
// }

// void startScheduler(){

//     // Creating and starting the main thread
//     // Priority from 0-99
//     threads tMain("tMain", 10, mainHandler, NULL);
// }



