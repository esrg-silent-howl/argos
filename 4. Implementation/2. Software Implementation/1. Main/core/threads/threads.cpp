#include "threads.hpp"
#include "log.hpp"

typedef struct shed_param shed_param;

// Thread::Thread(std::string pt_name, unsigned int pt_prio, void* (*pt_fun)(void*), void* args){

//     this->pt_name = pt_name;
//     this->pt_id = pt_id;
//     this->pt_fun = pt_fun;

//     int s1 = pthread_attr_init(&(this->pt_attr));
//     perror("attr init: %d\n", s1);
//     int s2 = pthread_attr_getschedpolicy (&(this->pt_attr), &(this->pt_policy));
//     perror("attr getshedpolicy: %d\n", s2);
//     int s3 = pthread_attr_getschedparam (&(this->pt_attr), &(this->pt_param));
//     perror("attr getshedparam: %d\n", s3);

//     int rr_min_priority, rr_max_priority;

//     int s4 = pthread_attr_setschedpolicy (&(this->pt_attr), SCHED_RR);
//     perror("attr setshedpolicy: %d\n", s4);
//     rr_min_priority = sched_get_priority_min (SCHED_RR);
// 	rr_max_priority = sched_get_priority_max (SCHED_RR);
//     this->pt_param.sched_priority = pt_prio;

//     perror ("SCHED_RR priority range is %d to %d: using %d\n",rr_min_priority,rr_max_priority,this->pt_param.sched_priority);

//     int s5 = pthread_attr_setschedparam (&(this->pt_attr), &(this->pt_param));
//     perror("attr setshedparam: %d\n", s5);

//     perror ("Creating %s thread at RR/%d\n",this->pt_name.c_str() ,this->pt_param.sched_priority);
//     //int s6 = pthread_attr_setinheritsched (&(this->pt_attr), PTHREAD_EXPLICIT_SCHED);
//     //perror("attr setinheritshed: %d\n", s6);

//     this->pt_status = pthread_create (&(this->pt_id), &(this->pt_attr), this->pt_fun, args);
//     perror("pthread create: %d\n", this->pt_status);

// 	if(this->pt_status)
// 	{
// 		if(this->pt_status==EPERM)
// 			fprintf(stderr,"pthread_create() got EPERM\n");
// 		else if(this->pt_status==EINVAL)
//       			fprintf(stderr,"pthread_create() got EINVAL\n");
//     		else
//       			fprintf(stderr,"pthread_create() got neither EPERM nor EINVAL\n");

//     		fprintf(stderr,"pthread_create() got error %d\n",this->pt_status);
//     		errno=this->pt_status;
//     		perror("pthread_create()");
//     		exit(1);
//   	}
// }

// Thread::~Thread(){}


const Thread* createThread (const Thread::Config& config) {

	/* Dinamically allocate memory for the Thread object */
	Thread* thread = new Thread;

	log::info << "Creating thread " << config.name << std::endl;

	thread->name = config.name;
	
	/* Initialize attr object with the default attributes */
	pthread_attr_init(&(thread->native_attr));

    /* Get current scheduler's policy */
    pthread_attr_getschedpolicy (&(thread->native_attr), &(thread->thread_policy));

    /* Get current scheduler's parameters */
    pthread_attr_getschedparam (&(thread->native_attr), &(thread->thread_param));

    /* Set current scheduler's policy */
    pthread_attr_setschedpolicy (&(thread->native_attr), SCHED_RR);

    /* Get current scheduler's max and min priorities */
    int rr_min_priority = sched_get_priority_min (SCHED_RR);
	int rr_max_priority = sched_get_priority_max (SCHED_RR);

    /* Define the real priority as the desired priority */
    thread->thread_param.sched_priority = config.priority;
    //printf ("SCHED_RR priority range is %d to %d: using %d\n",rr_min_priority,rr_max_priority,thread->thread_param.sched_priority);

    /* Request to attribute the desired priority value to the attributes 
	structure and then get the real value */
    pthread_attr_setschedparam (&(thread->native_attr), &(thread->thread_param));
    pthread_attr_getschedparam(&(thread->native_attr), &(thread->thread_param));

    /* Check if priority change request failed */
	if (thread->thread_param.sched_priority != config.priority) {
		log::warning << "Prority change request failed" << std::endl;
	}

    thread->thread_status = pthread_create(&(thread->native), &(thread->native_attr), \
			config.start_routine, (void*)config.args);

    if(thread->thread_status)
	{
		if(thread->thread_status==EPERM)
            log::error << "Thread creation returned EPERM" << std::endl;
		else if(thread->thread_status==EINVAL)
            log::error << "Thread creation returned EINVAL" << std::endl;  
    	else
            log::error << "Thread creation returned neither EPERM nor EINVAL" << std::endl;
    		exit(1);
  	}

	log::ok << "Created thread " << thread->name << std::endl;
	log::details << "Priority: " << thread->thread_param.sched_priority << std::endl;

	return thread;
}

int32_t deleteThread (Thread& thread) {

	int32_t status;
	
	/* Destroy attr object */
	status = pthread_attr_destroy(&(thread.native_attr));

	/* Free objects memory */
	delete &thread;

    return status;
}



