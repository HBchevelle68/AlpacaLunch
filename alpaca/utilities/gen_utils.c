#define _POSIX_C_SOURCE 200809L 
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>

#include <interfaces/utility_interface.h>


#ifndef DEBUGENABLE
void AlpacaUtilities_daemonize(void){

	int fd;

    switch (fork())
    {
        case -1:
            // Error during fork
            exit(EXIT_FAILURE); 
            break;
        case 0:
            // Child falls through
            break;
        default:
            // Parent exits
            _exit(EXIT_SUCCESS);
            break;
    }

    /* On success: The child process becomes session leader */
    if (setsid() < 0){
        exit(EXIT_FAILURE);
    }

    /* Catch, ignore and handle signals */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    switch (fork())
    {
        case -1:
            // Error during fork
            exit(EXIT_FAILURE);
            break;
        case 0:
            // Child falls through
            break;
        default:
            // Parent exits
            _exit(EXIT_SUCCESS);
            break;
    }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    if(chdir("/")){
        exit(EXIT_FAILURE);
    }

    /* Close all open file descriptors */
    for (int tempFd = sysconf(_SC_OPEN_MAX); tempFd >= 0; tempFd--){
        close(tempFd);
    }

    // Reopen std file descs pointing to /dev/null
    fd = open("/dev/null", O_RDWR);
    if(fd != STDIN_FILENO){
        exit(EXIT_FAILURE);
    }
    if(dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO){
        exit(EXIT_FAILURE);
    }
    if(dup2(STDOUT_FILENO, STDERR_FILENO) != STDERR_FILENO){
        exit(EXIT_FAILURE);
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


