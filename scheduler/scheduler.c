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
#define MAX_LEN_COMMAND 15 //max length of an executable string px ../work/work7

/* definition and implementation of process descriptor and queue(s) */

//struct pou antiprosopeuei ena process
struct Work
{
    int number; //o arithmos tou executable workN 
    int priority;
    pid_t pid;
    double time; //elapsed time 
    //double start_time;
    char command[MAX_LEN_COMMAND]; 
    struct Work *next; //pointer gia to queue
    struct Work *prev;
};

/* global variables and data structures */

//queue with doubly linked list 
struct WorkQueue
{
    struct Work *head;
    struct Work *tail;
};

void init_queue(struct WorkQueue *q) {
    q->head = NULL;
    q->tail = NULL;
}

void enqueue(struct WorkQueue *q, struct Work *new_process) {

    new_process->prev = NULL;
    new_process->next = NULL;

    if (q->tail == NULL) 
    {
        q->head = new_process;
        q->tail = new_process;
    }
    else 
    {
        q->tail->next = new_process;
        new_process->prev = q->tail;
        q->tail = new_process;
    }
}

struct Work* dequeue(struct WorkQueue *q) {
    if (q->head == NULL) 
    {
        return NULL;
    }
    
    struct Work *first_process = q->head;
    q->head = first_process->next;

    //kanoniki katastasi
    //an meta tin afairesi tou prwtou process apo tin lista/queue o deiktis q->head deixnei kapou tote thetw ton deikti 
    //prev tou kainouriou head process ws null
    if (q->head != NULL) 
    {
        q->head->prev = NULL;
    }

    //an meta tin afairesi tou prwtou process apo tin lista o deiktis q->head ginei NULL simainei oti ekana dequeue to 
    //telautaio process opote to q->tail edeixne ekei pou edeixne arxika kai o q->head opote prepei kai autos na ginei NULL 
    if (q->head == NULL) 
    {
        q->tail = NULL;
    }

    return first_process;
}

//synartisi pou epistrefei to work struct ston opoio deixnei to head tou queue
//whats the point of this???
//den to xreiazomaste
/*struct Work front(struct WorkQueue *q)
{
    if (q->head == NULL)
    {
        printf("The queue is empty\n");
    }
    return *(q->head);
}*/
  

/* signal handler(s) */

/* implementation of the scheduling policies, etc. batch(), rr() etc. */

int success[7] = {};
void FCFS(int process_count, struct WorkQueue q, struct Work works[])
{
    for (int i = 0; i < process_count; i++)
    {
        struct Work *work_p = dequeue(&q);
        struct Work current_work = *work_p;
        struct timeval start, end;
        gettimeofday(&start, NULL);
        int pid = fork();
        if (pid == 0)
        {
            // This is the child process
            char *args[] = {current_work.command, NULL};
            execvp(current_work.command, args);   
            exit(0);
        }
        else if (pid > 0)
        {
            // This is the parent process
            struct timeval start, end;
            gettimeofday(&start, NULL);
            waitpid(pid, NULL, 0);
            gettimeofday(&end, NULL);
            works[i].pid = pid;
            works[i].time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            success[i] = i;
            
        }
    }
}

/*void round_robin(struct WorkQueue *q, int quantum)
{
    struct Work current_process;
    double start_time, end_time;
    for (int i = 0; i < q->size; i++)
    {
        current_process = dequeue(q);
        start_time = clock();
        current_process.pid = fork();
        if (current_process.pid == 0)
        {
            // child process
            execl(current_process.command, current_process.command, NULL);
            exit(1);
        }
        else
        {
            // parent process
            sleep(quantum);
            kill(current_process.pid, SIGSTOP);
            end_time = clock();
            current_process.time = (end_time - start_time) / CLOCKS_PER_SEC;
            enqueue(q, current_process);
        }
    }
}*/


int main(int argc,char **argv)
{
	/* local variables */

	/* parse input arguments (policy, quantum (if required), input filename */

	//check if the number of arguments is correct
    if (argc != 3 && argc != 4)
    {
        printf("Invalid number of arguments.\nUsage: ./scheduler <algorithm> [quantum] <input_file>\n");
        return -1;
    }
    //read input arguments
    char *algorithm = argv[1];
    char *input_file;
    //an den uparxei quantum
    if (argc == 3)
    {
        char *input_file = argv[2];
    }
    //an uparxei quantum
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

    //metrame ton arithmo twn grammwn tou input_file
    for (row = getc(file); row != EOF; row = getc(file))
        if (row == '\n') // Increment count if this character is newline
            count++;

    rewind(file); //pigainoume ton file pointer stin arxi tou arxeiou

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

    //populate queue
    struct WorkQueue q;
    init_queue(&q);

    for (int i = 0; i < count; i++)
    {
        enqueue(&q, &processes[i]);
    }
    //test print queue
    //delete this
    struct Work *current = q.head;
    while (current != NULL) {
        printf("Work%d\n", current->number);
        current = current->next;
    }

	/* call selected scheduling policy */

    if (strcmp(algorithm, "BATCH") == 0)
    {
        for (int i = 0; i < count; i++)
        {
            enqueue(&q, &processes[i]);
        }

        FCFS(count, q, processes);
        printf("\n\n# scheduler %s %s\n\n", algorithm, input_file);
    }

    if (strcmp(algorithm, "RR") == 0)
    {
        if (quantum == 0)
        {
            printf("Error: Quantum not specified for RR algorithm\n");
            return 1;
        }
        //scheduleRR(count, quantum, processes);
    }

	/* print information and statistics */
    
    double workload = processes[success[0]].time;
    // Print the structs
    printf("Work: %d, Priority: %d, PID: %d, Elapsed Time: %.3lf, Workload Time: %.3lf\n", processes[success[0]].number, processes[success[0]].priority, processes[success[0]].pid, processes[success[0]].time, workload);
    for (int j = 1; j < count; j++)
    {
        workload = workload + (processes[success[j]].time);
        printf("Work: %d, Priority: %d, PID: %d, Elapsed Time: %.3lf, Workload Time: %.3lf\n", processes[success[j]].number, processes[success[j]].priority, processes[success[j]].pid, processes[success[j]].time, workload);
    }
    printf("WORKLOAD TIME: %.3lf\n\n", workload);

	return 0;
}
