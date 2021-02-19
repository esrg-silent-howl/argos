#include "shutdown.hpp"

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

int main(){

    // Daemon Setup
    daemonSetup();
    perror("[ARGOS] Shutdown Daemon Up and Running");
    syslog (LOG_NOTICE, "[ARGOS] Shutdown Daemon Up and Running");

    // Daemon Main Thread
    while (1){

        // Check Button pressed and send SIGTERM
        break;
    }

    syslog (LOG_NOTICE, "[ARGOS] Shutdown Daemon Terminated");
    closelog();
    return EXIT_SUCCESS;
}

/* ps -xj | grep shutdownDaemon 
   grep shutdownDaemon /var/log/syslog */