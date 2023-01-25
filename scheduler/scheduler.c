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
#include <errno.h>
#include <stdbool.h>

/* global definitions */
#define MAX_QUEUE_SIZE 10
#define MAX_LEN_COMMAND 15 // max length of an executable string px ../work/work7

/* definition and implementation of process descriptor and queue(s) */

// struct pou antiprosopeuei ena process
struct Work
{
    int number; // o arithmos tou executable workN
    int priority;
    pid_t pid;
    double time; // elapsed time
    // double start_time;
    char command[MAX_LEN_COMMAND];
    struct Work *next; // pointer gia to queue
    struct Work *prev;
    int status;
    bool exited;
};

/* global variables and data structures */

struct Work *current_process; //global pointer se Work gia xrisi sto catch_sigchld()
struct timeval start, end; //gia ne metrisw ton xrono ektelesis kathe diergasias

// queue with doubly linked list
struct WorkQueue
{
    struct Work *head;
    struct Work *tail;
};


void init_queue(struct WorkQueue *q)
{
    q->head = NULL;
    q->tail = NULL;
}

void enqueue(struct WorkQueue *q, struct Work *new_process)
{

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

struct Work *dequeue(struct WorkQueue *q)
{
    if (q->head == NULL)
    {
        return NULL;
    }

    struct Work *first_process = q->head;
    q->head = first_process->next;

    // kanoniki katastasi
    // an meta tin afairesi tou prwtou process apo tin lista/queue o deiktis q->head deixnei kapou tote thetw ton deikti
    // prev tou kainouriou head process ws null
    if (q->head != NULL)
    {
        q->head->prev = NULL;
    }

    // an meta tin afairesi tou prwtou process apo tin lista o deiktis q->head ginei NULL simainei oti ekana dequeue to
    // telautaio process opote to q->tail edeixne ekei pou edeixne arxika kai o q->head opote prepei kai autos na ginei NULL
    if (q->head == NULL)
    {
        q->tail = NULL;
    }

    return first_process;
}

/* signal handler(s) */
void catch_sigchld(int sig)
{
    waitpid(current_process->pid, &current_process->status, WNOHANG);
    if(WIFEXITED(current_process->status))
    {
        current_process->exited = true;
        gettimeofday(&end, NULL);
        current_process->time = current_process->time + ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0);
        
    }
}

/* implementation of the scheduling policies, etc. batch(), rr() etc. */

void FCFS(int process_count, struct WorkQueue q, struct Work works[])
{
    for (int i = 0; i < process_count; i++)
    {
        struct Work *first_work = dequeue(&q);

        int pid = fork();
        if (pid == 0)
        {
            // This is the child process
            char *args[] = {first_work->command, NULL};
            execvp(first_work->command, args);
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
        }
    }
}

void SJF(int process_count, struct WorkQueue q, struct Work works[])
{
    struct Work temp[process_count];
    for (int i = 1; i < process_count; i++)
    {
        temp[0].priority = works[i].priority;
        temp[0].pid = works[i].pid;
        temp[0].time = works[i].time;
        temp[0].number = works[i].number;
        strcpy(temp[0].command, works[i].command);
        temp[0].next = works[i].next;
        temp[0].prev = works[i].prev;
        int j = i - 1;

        while (temp[0].priority < works[j].priority && j >= 0)
        {
            works[j + 1].priority = works[j].priority;
            works[j + 1].pid = works[j].pid;
            works[j + 1].time = works[j].time;
            works[j + 1].number = works[j].number;
            strcpy(works[j + 1].command, works[j].command);
            works[j + 1].next = works[j].next;
            works[j + 1].prev = works[j].prev;
            --j;
        }
        works[j + 1].priority = temp[0].priority;
        works[j + 1].pid = temp[0].pid;
        works[j + 1].time = temp[0].time;
        works[j + 1].number = temp[0].number;
        strcpy(works[j + 1].command, temp[0].command);
        works[j + 1].next = temp[0].next;
        works[j + 1].prev = temp[0].prev;
    }

    for (int i = 0; i < process_count; i++)
    {
        enqueue(&q, &works[i]);
    }

    for (int i = 0; i < process_count; i++)
    {
        struct Work *first_work = dequeue(&q);

        int pid = fork();
        if (pid == 0)
        {
            // This is the child process
            char *args[] = {first_work->command, NULL};
            execvp(first_work->command, args);
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
        }
    }
}

void RR(int quantum, struct WorkQueue *q)
{
    struct timespec sleep_time;
    sleep_time.tv_sec = quantum/1000;
    sleep_time.tv_nsec = 0;

    signal(SIGCHLD,catch_sigchld); //kanw register tin catch_sigchld function ws handler for the SIGCHLD signal

    while (q->head != NULL)
    {
        //int ret_val = 0;
        current_process = dequeue(q);
        gettimeofday(&start, NULL); //start timer

        if(WIFSTOPPED(current_process->status)){
            kill(current_process->pid, SIGCONT);
        }
        else if(current_process->pid == -1){
            current_process->pid = fork();
        }

        if (current_process->pid == 0)
        {
            // child process
            char *args[] = {current_process->command, NULL};
            execvp(current_process->command, args);
            exit(0);
        }

        else
        {
            // parent process
            int ret_val = nanosleep(&sleep_time, NULL); // sleep gia xrono iso me to quantum
            if(current_process->exited) {
                continue;
            }
            else{
                kill(current_process->pid, SIGSTOP);
                waitpid(current_process->pid, &current_process->status, WSTOPPED);
                enqueue(q,current_process);
                gettimeofday(&end, NULL);
                current_process->time = current_process->time + ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0);
            }
            
        }
    }
}

void PRIO()
{
    // algorithmos tou jj edw
}

int main(int argc, char **argv)
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
        input_file = argv[2];
    }
    // an uparxei quantum
    int quantum = 0;
    if (argc == 4)
    {

        input_file = argv[3];
        quantum = atoi(argv[2]);
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

    // init work 
    for (int i = 0; i < count; i++)
    {
        processes[i].time = 0;
        processes[i].pid = -1;
        processes[i].status = -1;
        processes[i].exited = false;
    }

    char *token;

    int i = 0;
    int n; // apo to workn
    while (fgets(line, 50, file) != NULL)
    {
        token = strtok(line, "\t");
        strcpy(processes[i].command, token);
        sscanf(token, "../work/work%d", &n); // diavazw ton arithmo tis diergasias
        processes[i].number = n;
        token = strtok(NULL, "\t");
        processes[i].priority = atoi(token);
        i++;
    }

    fclose(file);

    //initiate queue
    struct WorkQueue q;
    init_queue(&q);

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
    else if (strcmp(algorithm, "SJF") == 0)
    {

        SJF(count, q, processes);
        printf("\n\n# scheduler %s %s\n\n", algorithm, input_file);
    }
    else if (strcmp(algorithm, "RR") == 0)
    {
        if (quantum == 0)
        {
            printf("Error: Quantum not specified for RR algorithm\n");
            return 1;
        }
        for (int i = 0; i < count; i++)
        {
            enqueue(&q, &processes[i]);
        }
        RR(quantum, &q);
        printf("\n\n# scheduler %s %d %s\n\n", algorithm, quantum, input_file);
    }

    else if (strcmp(algorithm, "PRIO") == 0)
    {
        if (quantum == 0)
        {
            printf("Error: Quantum not specified for RR algorithm\n");
            return 1;
        }
        // klisi synartisis jj edw

        printf("\n\n# scheduler %s %d %s\n\n", algorithm, quantum, input_file);
    }
    else
    {
        printf("Error: Unknown algorithm %s\n", algorithm);
        return 1;
    }

    /* print information and statistics */

    double workload = 0;
    for (int i = 0; i < count; i++)
    {
        workload = workload + (processes[i].time);
        printf("Work: %d, Priority: %d, PID: %d, Elapsed Time: %.3lf, Workload Time: %.3lf\n",
               processes[i].number, processes[i].priority, processes[i].pid,
               processes[i].time, workload);
    }
    printf("\nWORKLOAD TIME: %.3lf\n\n", workload);

    return 0;
}