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
#define MAX_QUEUE_SIZE 10
#define MAX_LEN_COMMAND 15 // max length of an executable string px ../work/work7

/* definition and implementation of process descriptor and queue(s) */

// struct pou antiprosopeuei ena process
struct Work
{
    int number; //o arithmos tou executable workN 
    int priority;
    pid_t pid;
    double time; //elapsed time 
    double start_time;
    char command[MAX_LEN_COMMAND]; 
};

/* global variables and data structures */

//queue
struct WorkQueue
{
    struct Work processes[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int size;
};

// initialize head, tail and size values
void init_queue(struct WorkQueue *q)
{
    q->head = 0;
    q->tail = -1;
    q->size = 0;
}

void enqueue(struct WorkQueue *q, struct Work item)
{
    if(q->size == MAX_QUEUE_SIZE)
    {
        printf("Error: Queue is full\n");
        return;
    }
    /*me to % MAX_QUEUE_SIZE otan kanw enqueue kai to 
    tail vriskotan stin teleutaia thesi tou pinaka anti na auksithei kata ena, midenizetai*/
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE; 
    q->processes[q->tail] = item;
    q->size++;
}

struct Work dequeue(struct WorkQueue *q)
{
    if(q->size == 0)
    {
        printf("Error: Queue is empty\n");
        return;
    }
    struct Work process = q->processes[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE; 
    q->size--;
}

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
    
    int count = 0;
    char row;
    FILE *file = fopen(input_file, "r");
    if (file == NULL)
    {
        printf("Could not open file\n");
        return -1;
    }

    // metrame ton arithmo twn grammwn tou input_file
    for (row = getc(file); row != EOF; row = getc(file))
        if (row == '\n') // Increment count if this character is newline
            count++;

    rewind(file); // pigainoume ton file pointer stin arxi tou arxeiou

    char line[50];
    struct Work processes[count];
    char *token;

    int i = 0;
    int n; //workn
    while (fgets(line, 50, file) != NULL)
    {
        token = strtok(line, "\t");
        strcpy(processes[i].command, token);
        sscanf(token, "../work/work%d", &n); //diavazw ton arithmo tis diergasias
        processes[i].number = n;
        token = strtok(NULL, "\t");
        processes[i].priority = atoi(token);
        i++;
    }
    
    fclose(file);

	/* call selected scheduling policy */

	/* print information and statistics */

	return 0;
}
