/* header files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <string.h>
/* global definitions */

/* definition and implementation of process descriptor and queue(s) */

/* global variables and data structures */

/* signal handler(s) */

/* implementation of the scheduling policies, etc. batch(), rr() etc. */

int main(int argc,char **argv)
{
	/* local variables */

	/* parse input arguments (policy, quantum (if required), input filename */
	// check if the number of arguments is correct
    if (argc != 3 && argc != 4)
    {
        printf("Invalid number of arguments.\nUsage: ./scheduler <algorithm> [quantum] <input_file>\n");
        return -1;
    }
    // read input arguments
    char *algorithm = argv[1];
    char *input_file;
    if (argc == 3)
    {
        char *input_file = argv[2];
    }
    int quantum = 0;
    if (argc == 4)
    {

        input_file = argv[3];
        quantum = atoi(argv[2]);
		printf("Input agrs: %s %d %s\n", algorithm,quantum,input_file); //delete this
    }

	/* read input file - populate queue */

	/* call selected scheduling policy */

	/* print information and statistics */

	printf("Scheduler exits\n");
	return 0;
}
