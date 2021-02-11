#ifndef _THREADS_H
#define _THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <string>
#include <sys/time.h>
#include <sys/resource.h>

typedef struct shed_param shed_param;

class threads{

    private:
        std::string pt_name;
        pthread_t pt_id;
        pthread_attr_t pt_attr;
        sched_param pt_param;
        int pt_policy;
        void* (*pt_fun)(void*);
        int pt_status;
    public:
        threads(std::string, unsigned int, void* (*pt_fun)(void*), void*);
        ~threads();
};

extern void startScheduler(void);

#endif