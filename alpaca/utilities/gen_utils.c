#define _POSIX_C_SOURCE 200809L 
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>

#include <interfaces/utility_interface.h>


#if DEBUGENABLE
void AlpacaUtilities_daemonize(void){

	pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0){
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0){
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
    if (setsid() < 0){
        exit(EXIT_FAILURE);
    }

    /* Catch, ignore and handle signals */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0){
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0){
        exit(EXIT_SUCCESS);
    }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    if(chdir("/")){
        exit(1);
    }

    /* Close all open file descriptors */
    for (int tempFd = sysconf(_SC_OPEN_MAX); tempFd >= 0; tempFd--){
        close (tempFd);
    }
}
#else
void AlpacaUtilities_daemonize(void){return;}
#endif



/* 
 * msleep(): Sleep for the requested number of milliseconds. 
 */
int AlpacaUtilities_mSleep(long msec){

    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {

        // SLEEP
        res = nanosleep(&ts, &ts);

    } while (res && errno == EINTR);

    return res;
}


