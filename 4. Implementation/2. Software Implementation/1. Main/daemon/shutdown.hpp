#ifndef _SHUTDOWN_HPP
#define _SHUTDOWN_HPP

#include  <sys/syslog.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include <iostream>
#include <vector>
#include <Python.h>

extern void daemonSetup();

#endif