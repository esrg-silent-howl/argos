#include "shutdown.hpp"

void daemonSetup(){

    pid_t pid;

    /* Fork off the parent process and create a new child process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
		//syslog(LOG_ERR, "%s\n", "Fork");
        exit(EXIT_FAILURE);

    /* Success: Let the parent process terminate */
    if (pid > 0){
        exit(EXIT_SUCCESS);
	}

	/*Start a new session ( setsid() )*/
    /* On success: The child process becomes session leader now detached 
	from its controlling terminal (CTTY)*/
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    //signal(SIGCHLD, SIG_IGN);
    //signal(SIGHUP, SIG_IGN);

	/* Redundancy Fork*/
    /* Fork off for the second time to ensure that the daemon doesn't
	have access to the TTY again*/
    pid = fork();

    /* An error occurred */
    if (pid < 0){
		//syslog(LOG_ERR, "%s\n", " Second Fork");
        exit(EXIT_FAILURE);
	}

    /* Success: Let the parent terminate */
    if (pid > 0){
		//printf("Deamon PID: %d\n", pid);
        exit(EXIT_SUCCESS);
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

int main()
{
    daemonSetup();

    while (1)
    {
        //TODO: Insert daemon code here.
        syslog (LOG_NOTICE, "Shutdown Daemon Up and Running.");
        sleep (20);
        //break;
    }

    syslog (LOG_NOTICE, "Shutdown Daemon Terminated.");
    closelog();

    return EXIT_SUCCESS;
}

/* ps -xj | grep shutdownDaemon 
   grep shutdownDaemon /var/log/syslog */