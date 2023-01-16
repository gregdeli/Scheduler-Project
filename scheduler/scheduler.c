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
// struct pou antiprosopeuei ena process
struct Work
{
    int number; //o arithmos tou executable workN 
    int priority;
    pid_t pid;
    double time; //elapsed time 
    double start_time;
    char *command; 
};
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
    // an den uparxei quantum
    if (argc == 3)
    {
        char *input_file = argv[2];
    }
    // an uparxei quantum
    int quantum = 0;
    if (argc == 4)
    {

        input_file = argv[3];
        quantum = atoi(argv[2]);
		printf("Input agrs: %s %d %s\n", algorithm,quantum,input_file); //delete this
    }

	/* read input file - populate queue */

    int line_count = 0; //arithmos ton grammwn sto input_file
    char row;
    char getname[5];

    int priority;

    FILE *file1 = fopen(input_file, "r");
    if (file1 == NULL)
    {
        printf("Could not open file\n");
        return -1;
    }

    // metrame ton arithmo twn grammwm tou input_file
    for (row = getc(file1); row != EOF; row = getc(file1))
        if (row == '\n') // Increment line_count if this character is newline
            line_count++;

    fclose(file1);

    // dimiourgw pinaka apo structs typou Work
    struct Work works[line_count];

    // open the file
    FILE *file = fopen(input_file, "r");
    if (file == NULL)
    {
        printf("Could not open file\n");
        return -1;
    }

    int n, p;

    char line[50];
    FILE *fp = fopen(input_file, "r");
    for (int i = 0; i < 7; i++)
    {
        fgets(line, sizeof(line), fp);
        sscanf(line, "../work/work%d", &n);
        sprintf(works[i].command[i], "../work/work%d", n);
        works[i].number = n;
    }
    fclose(fp);

    // read the file
    int j = 0;
    while (fscanf(file1, "%s %d", works[j].command[j], &priority) != EOF)
    {
        works[j].priority = priority;
        j++;
    }

	/* call selected scheduling policy */

	/* print information and statistics */

	printf("Scheduler exits\n");
	return 0;
}
