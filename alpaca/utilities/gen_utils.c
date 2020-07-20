#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h> 

//#define _POSIX_C_SOURCE      199309L    

/*
int msleep(long msec) {

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
*/

void AlpacaUtilities_daemonize(){

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
    chdir("/");

    /* Close all open file descriptors */
    for (int tempFd = sysconf(_SC_OPEN_MAX); tempFd >= 0; tempFd--){
        close (tempFd);
    }
}






