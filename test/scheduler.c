#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void catch_sigchld(int signo)
{
    printf("Child process has exited!\n");
}

int main()
{
    pid_t pid;
    int status;

    // Register a signal handler for SIGCHLD
    signal(SIGCHLD, catch_sigchld);

    pid = fork();
    if (pid == 0) {
        // Child process
        printf("I am the child process\n");
        sleep(3);
        exit(0);
    } else {
        // Parent process
        printf("I am the parent process\n");
        sleep(10);
    }

    return 0;
}
