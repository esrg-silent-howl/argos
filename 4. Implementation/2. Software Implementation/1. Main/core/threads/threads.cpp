#include "threads.hpp"

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

static void* inferenceFunc(void* arg){
    printf("Starting Inference");

    return (void*)0;
}

static void* cameraFunc(void* arg){
    printf("Starting Camera");

    return (void*)0;
}

static void* rclientFunc(void* arg){
    printf("Starting Remote Client");

    return (void*)0;
}

static void* cloudFunc(void* arg){
    printf("Starting Cloud");

    return (void*)0;
}

static void* databaseFunc(void* arg){
    printf("Starting Database");

    return (void*)0;
}

static void* mainFunc(void* arg){
    printf("Starting Main");

    return (void*)0;
}

void startScheduler(){

    //Creating and starting all the threads
    //Priority from 0-99
    threads tMLInference("tCamera", 60, inferenceFunc, NULL);
	threads tCameraControl("tCamera", 50, cameraFunc, NULL);
    threads tRemoteClient("tCamera", 40, rclientFunc, NULL);
    threads tCloud("tCamera", 30, cloudFunc, NULL);
    threads tDatabase("tCamera", 20, databaseFunc, NULL);
    threads tMain("tCamera", 10, mainFunc, NULL);

}



