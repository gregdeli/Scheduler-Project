//Γρηγόρης Δελημπαλταδάκης, ΑΜ: 1084647
//Δαμιανός Διασάκος, ΑΜ: 1084632
//Αλκιβιάδης Δασκαλάκης, ΑΜ: 1084673
//Ιάσων Ράικος, ΑΜ: 1084552

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
    double workload_rr; // workload time
    double workload_prio; // workload time
    struct timeval start_time, end_time; ////gia ne metrisw ton xrono ektelesis kathe diergasias
    char command[MAX_LEN_COMMAND];
    struct Work *next; // pointer gia to queue
    struct Work *prev;
    int status;
    bool exited;
    int index; //gia tin ektypwsi me tin seira pou oloklirwnontai oi diergasies
};

/* global variables and data structures */
struct Work *current_process; //global pointer se Work gia xrisi sto catch_sigchld()
struct timeval start_rr, end_rr; //gia ne metrisw workload time se RR kai PRIO
struct timeval start_prio, end_prio; 


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

//otan ena child process steilei shma SIGCHLD kai exei termatisei thelw na elegxw ean exei termatistei ontws i ektelesi tou
//gia na thesw to pedio exited = true kai na aferethei i diergasia apo tin oura ektelesis
void catch_sigchld(int sig)
{
    waitpid(current_process->pid, &current_process->status, WNOHANG);
    if(WIFEXITED(current_process->status))
    {
        current_process->exited = true;
        gettimeofday(&current_process->end_time, NULL);
        current_process->time = (current_process->end_time.tv_sec - current_process->start_time.tv_sec) + 
        (current_process->end_time.tv_usec - current_process->start_time.tv_usec) / 1000000.0;  
        current_process->workload_rr = (current_process->end_time.tv_sec-start_rr.tv_sec)+(current_process->end_time.tv_usec-start_rr.tv_usec)/ 1000000.0;  
        current_process->workload_prio = (current_process->end_time.tv_sec-start_prio.tv_sec)+(current_process->end_time.tv_usec-start_prio.tv_usec)/ 1000000.0;  
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

void SJF(int process_count, struct WorkQueue q, struct Work works[],int success[])
{
    struct Work temp;
    for (int i = 1; i < process_count; i++)
    {
        temp.priority = works[i].priority;
        temp.pid = works[i].pid;
        temp.time = works[i].time;
        temp.number = works[i].number;
        strcpy(temp.command, works[i].command);
        temp.next = works[i].next;
        temp.prev = works[i].prev;
        int j = i - 1;

        while (temp.number < works[j].number && j >= 0)
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

        works[j + 1].priority = temp.priority;
        works[j + 1].pid = temp.pid;
        works[j + 1].time = temp.time;
        works[j + 1].number = temp.number;
        strcpy(works[j + 1].command, temp.command);
        works[j + 1].next = temp.next;
        works[j + 1].prev = temp.prev;
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
            success[i]=i;
        }
    }
}

void RR(int quantum, struct WorkQueue *q, int success[],int index) 
{
    gettimeofday(&start_rr, NULL); 
    struct timespec sleep_time;
    if(quantum>=1000)
    {
        sleep_time.tv_sec = quantum/1000;
        sleep_time.tv_nsec = 0; 
    }
    else
    {
        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = quantum * 1000000; //quantum ms = quantum * 10^6 ns
    }
    
    //gia na pigainei sto catch_sigchld otan kanei mono exit() ena paidi
    struct sigaction sa;
    sa.sa_handler = catch_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP;

    sigaction(SIGCHLD, &sa, NULL); //kanw register tin catch_sigchld function ws handler for the SIGCHLD signal
    
    while (q->head != NULL)
    {
        current_process = dequeue(q);
        if(WIFSTOPPED(current_process->status))
        {
            kill(current_process->pid, SIGCONT);
        }
        else if(current_process->pid == -1)
        {
            current_process->pid = fork();
            gettimeofday(&current_process->start_time, NULL); 
        }

        if (current_process->pid == 0)
        {
            // child process
            char *args[] = {current_process->command, NULL};
            execvp(current_process->command, args);
            exit(0);
        }
        //an kanw dequeu process pou exei teleiwsei den prepei na ksana kanw sleep() 
        else if(current_process->exited){
            success[index] = current_process->index;
            index++;
            continue;
        }
        else
        {
            // parent process
            nanosleep(&sleep_time, NULL); // sleep gia xrono iso me to quantum

            if(current_process->exited) 
            {
                success[index] = current_process->index;
                index++;
                continue;
            }
            else
            {
                kill(current_process->pid, SIGSTOP);
                waitpid(current_process->pid, &current_process->status, WSTOPPED);
                enqueue(q,current_process);
            }
            
        }
    }
    gettimeofday(&end_rr, NULL);
}

void PRIO(int process_count, struct WorkQueue q, struct Work works[],int quantum,int success[])
{
    struct Work temp[process_count];

    for (int i = 1; i < process_count; i++)
    {
        temp[0].priority=works[i].priority;
        temp[0].pid=works[i].pid;
        temp[0].time=works[i].time;
        temp[0].number=works[i].number;
        strcpy(temp[0].command,works[i].command);
        temp[0].next=works[i].next;
        temp[0].prev=works[i].prev;
        int j=i-1;

        while (temp[0].priority < works[j].priority && j >= 0)
        {
            works[j + 1].priority=works[j].priority;
            works[j + 1].pid=works[j].pid;
            works[j + 1].time=works[j].time;
            works[j + 1].number=works[j].number;
            strcpy(works[j + 1].command,works[j].command);
            works[j + 1].next=works[j].next;
            works[j + 1].prev=works[j].prev;

            --j;
        }

        works[j + 1].priority=temp[0].priority;
        works[j + 1].pid=temp[0].pid;
        works[j + 1].time=temp[0].time;
        works[j + 1].number=temp[0].number;
        strcpy(works[j + 1].command,temp[0].command);
        works[j + 1].next=temp[0].next;
        works[j + 1].prev=temp[0].prev;
    }

    for (int i=0; i<process_count; i++)
    {
        enqueue(&q,&works[i]);
    }

    bool found_same_priority=false;
    int start_of_same_priority=0;
    struct Work *prev_work=NULL;
    struct Work *first_work=NULL;

    struct WorkQueue temp_q;
    init_queue(&temp_q);

    gettimeofday(&start_prio, NULL);

    first_work=dequeue(&q);
    for (int i=0; i<process_count; i++)
    {
        int pid=-1;
        int temp_process_position=0;

        prev_work=first_work;

        first_work=dequeue(&q);

        if(first_work==NULL && found_same_priority==false)
        {
            goto last_work_not_in_same_priority_queue;
        }
        else if(first_work==NULL && found_same_priority==true)
        {
            goto last_work_in_same_priority_queue;
        }
       
        if(first_work->priority==prev_work->priority)
        {
            found_same_priority=true;
                
            continue;
        }
        else
        {
            if(found_same_priority==true)
            {
                last_work_in_same_priority_queue:

                temp_process_position=start_of_same_priority;
                    
                for(int x=0; x<i-start_of_same_priority+1; x++)
                {
                    temp[x]=works[temp_process_position];
                        
                    temp_process_position++;
                }
                    
                for (int k=0; k<i-start_of_same_priority+1; k++)
                {
                    enqueue(&temp_q,&temp[k]);
                }
                    
                RR(quantum,&temp_q,success,start_of_same_priority);

                int process_position=start_of_same_priority;
                    
                for(int d=0; d<i-start_of_same_priority+1; d++)
                {
                    works[process_position]=temp[d];
                        
                    process_position++;
                }
                    
                found_same_priority=false;
                    
                start_of_same_priority=i+1;
                    
                continue;
            }
                
            start_of_same_priority=i;
        }

        last_work_not_in_same_priority_queue:
        
        pid=fork();
        if (pid==0)
        {
            // This is the child process
            char *args[]={prev_work->command,NULL};
            execvp(prev_work->command,args);
            exit(0);
        }
        else if (pid>0)
        {
            // This is the parent process
            struct timeval start,end;
            gettimeofday(&start,NULL);
            waitpid(pid, NULL,0);
            gettimeofday(&end,NULL);
            works[i].pid=pid;
            works[i].time=(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
            success[i]=i;
        }
    }
    gettimeofday(&end_prio, NULL);
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
        processes[i].index = i;
        processes[i].time = 0;
        processes[i].pid = -1;
        processes[i].status = -1;
        processes[i].exited = false;
        i++;
    }

    fclose(file);

    //initiate queue
    struct WorkQueue q;
    init_queue(&q);

    /* call selected scheduling policy */

    //se auton ton pinaka apothikeuoume ta indexes twn processes apo ton pinaka processes[] me tin seira pou termatizontai
    //etsi gnorizoume tin seira me tin opoia termatistikan oi diergasies
    int success[count];
    
    double workload_time; // gia metrisi workload time se RR kai PRIO

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
        SJF(count, q, processes,success);
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
        
        RR(quantum, &q, success, 0);
        workload_time = (end_rr.tv_sec - start_rr.tv_sec) + (end_rr.tv_usec - start_rr.tv_usec) / 1000000.0;  
        printf("\n\n# scheduler %s %d %s\n\n", algorithm, quantum, input_file);
    }
    else if (strcmp(algorithm,"PRIO")==0)
    {
        if (quantum==0)
        {
            printf("Error: Quantum not specified for PRIO algorithm\n");
            return 1;
        }

        PRIO(count,q,processes,quantum,success);
        workload_time = (end_prio.tv_sec - start_prio.tv_sec) + (end_prio.tv_usec - start_prio.tv_usec) / 1000000.0;  
        printf("\n\n# scheduler %s %d %s\n\n", algorithm, quantum, input_file);
    }
    else
    {
        printf("Error: Unknown algorithm %s\n", algorithm);
        return 1;
    }

    /* print information and statistics */

    double workload = 0;
    if(strcmp(algorithm, "BATCH") == 0 || strcmp(algorithm, "SJF") == 0)
    {
        double workload = 0;
        for (int i = 0; i < count; i++)
        {
            workload = workload + (processes[i].time);
            printf("Work: %d, Priority: %d, PID: %d, Elapsed Time: %.3lf, Workload Time: %.3lf\n",
                processes[i].number, processes[i].priority, processes[i].pid,
                processes[i].time, workload);
        }
        printf("\nWORKLOAD TIME: %.3lf\n\n", workload);
    }
    else if(strcmp(algorithm, "RR") == 0) 
    {
        for (int i = 0; i < count; i++)
        {
            
            printf("Work: %d, Priority: %d, PID: %d, Elapsed Time: %.3lf, Workload Time: %.3lf\n",
                processes[success[i]].number, processes[success[i]].priority, processes[success[i]].pid,
                processes[success[i]].time,processes[success[i]].workload_rr);

            
        }
        printf("\nWORKLOAD TIME: %.3lf\n\n", workload_time);
    }
    else{
        
        for (int i = 0; i < count; i++)
        {

            printf("Work: %d, Priority: %d, PID: %d, Elapsed Time: %.3lf, Workload Time: %.3lf\n",
                processes[success[i]].number, processes[success[i]].priority, processes[success[i]].pid,
                processes[success[i]].time,processes[success[i]].workload_prio);
        }

            
        
        printf("\nWORKLOAD TIME: %.3lf\n\n", workload_time);


    }
    
    return 0;
}