#ifndef _THREADS_HPP
#define _THREADS_HPP

#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

typedef struct shed_param shed_param;

typedef enum Priority_t {
    IDLE = 0,
    LOW = 15,
    BELOW_NORMAL = 30,
    NORMAL = 45,
    ABOVE_NORMAL = 60,
    HIGH = 75,
    REAL_TIME = 90
} Priority;

struct Thread{

    public:
        std::string name;
        pthread_t native;
        pthread_attr_t native_attr;
        
        sched_param thread_param;
        int thread_policy;
        int thread_status;

    public:
    	struct Config {

		std::string name;
		
		uint32_t priority;
		void* args;
		void* (*start_routine) (void *);
	    };

    // public:
        //Thread(std::string, unsigned int, void* (*pt_fun)(void*), void*);
        // Thread();
        // ~Thread();
};

const Thread* createThread(const Thread::Config& config);

int32_t deleteThread (const Thread& thread);

#endif