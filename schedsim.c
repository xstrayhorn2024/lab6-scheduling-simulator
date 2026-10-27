// C program for implementation of Simulation 
#include<stdio.h> 
#include<limits.h>
#include<stdlib.h>
#include "process.h"
#include "util.h"


// Function to find the waiting time for all  
// processes
// Add every process that has arrived by time t to the ready queue
// (a circular buffer holding process indexes) exactly once.
static void admitArrivals(ProcessType plist[], int n, int t, int admitted[],
                          int queue[], int *tail, int *count)
{
    // Admit in order of arrival time (ties: list order)
    for (;;) {
        int best = -1;
        for (int i = 0; i < n; i++)
            if (!admitted[i] && plist[i].art <= t &&
                (best < 0 || plist[i].art < plist[best].art))
                best = i;
        if (best < 0)
            break;
        admitted[best] = 1;
        queue[*tail] = best;
        *tail = (*tail + 1) % n;
        (*count)++;
    }
}

// Function to find the waiting time for all processes using Round Robin,
// taking arrival times into account with a ready queue.
void findWaitingTimeRR(ProcessType plist[], int n, int quantum)
{
    int *rem_bt = (int *)malloc(n * sizeof(int));   // remaining burst times
    int *admitted = (int *)calloc(n, sizeof(int));  // already queued once?
    int *queue = (int *)malloc(n * sizeof(int));    // ready queue
    int head = 0, tail = 0, count = 0;
    int t = 0, completed = 0;

    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    while (completed < n) {
        admitArrivals(plist, n, t, admitted, queue, &tail, &count);

        if (count == 0) {
            // CPU idle: jump ahead to the next arrival
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!admitted[i] && plist[i].art < next)
                    next = plist[i].art;
            t = next;
            continue;
        }

        int cur = queue[head];
        head = (head + 1) % n;
        count--;

        int slice = (rem_bt[cur] < quantum) ? rem_bt[cur] : quantum;
        t += slice;
        rem_bt[cur] -= slice;

        // Processes that arrived during this slice queue up before the
        // preempted process goes to the back of the line.
        admitArrivals(plist, n, t, admitted, queue, &tail, &count);

        if (rem_bt[cur] > 0) {
            queue[tail] = cur;
            tail = (tail + 1) % n;
            count++;
        } else {
            completed++;
            plist[cur].wt = t - plist[cur].art - plist[cur].bt;
        }
    }

    free(rem_bt);
    free(admitted);
    free(queue);
}

// Function to find the waiting time for all  
// processes 
void findWaitingTimeSJF(ProcessType plist[], int n)
{
    int *rt = (int *)malloc(n * sizeof(int));
    int complete = 0, t = 0, shortest = 0, minm, found;

    for (int i = 0; i < n; i++) {
        rt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    while (complete != n) {
        minm = INT_MAX;
        found = 0;
        for (int j = 0; j < n; j++) {
            if (plist[j].art <= t && rt[j] > 0 && rt[j] < minm) {
                minm = rt[j];
                shortest = j;
                found = 1;
            }
        }
        if (!found) {
            t++;
            continue;
        }
        rt[shortest]--;
        if (rt[shortest] == 0) {
            complete++;
            plist[shortest].wt = (t + 1) - plist[shortest].bt - plist[shortest].art;
        }
        t++;
    }

    free(rt);
}

// Function to find the waiting time for all  
// processes 
void findWaitingTime(ProcessType plist[], int n)
{
    // Processes are served in list order; the CPU may sit idle until a
    // process arrives, and waiting time = start time - arrival time.
    int t = 0;

    for (int i = 0; i < n; i++) {
        if (t < plist[i].art)
            t = plist[i].art;
        plist[i].wt = t - plist[i].art;
        t += plist[i].bt;
    }
}
  
// Function to calculate turn around time 
void findTurnAroundTime( ProcessType plist[], int n)
{ 
    // calculating turnaround time by adding bt[i] + wt[i] 
    for (int  i = 0; i < n ; i++) 
        plist[i].tat = plist[i].bt + plist[i].wt; 
} 
  
// Function to sort the Process acc. to priority
int my_comparer(const void *this, const void *that)
{
    const ProcessType *a = (const ProcessType *)this;
    const ProcessType *b = (const ProcessType *)that;

    // Higher priority value first
    if (a->pri < b->pri) return 1;
    if (a->pri > b->pri) return -1;
    // Same priority: first come, first served (then lower pid first)
    if (a->art != b->art) return (a->art > b->art) ? 1 : -1;
    return (a->pid > b->pid) ? 1 : ((a->pid < b->pid) ? -1 : 0);
}

//Function to calculate average time 
void findavgTimeFCFS( ProcessType plist[], int n) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTime(plist, n); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nFCFS\n");
}

//Function to calculate average time 
void findavgTimeSJF( ProcessType plist[], int n) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTimeSJF(plist, n); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nSJF\n");
}

//Function to calculate average time 
void findavgTimeRR( ProcessType plist[], int n, int quantum) 
{ 
    //Function to find waiting time of all processes 
    findWaitingTimeRR(plist, n, quantum); 
  
    //Function to find turn around time for all processes 
    findTurnAroundTime(plist, n); 
  
    //Display processes along with all details 
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

//Function to calculate average time 
void findavgTimePriority( ProcessType plist[], int n)
{
    // 1- Sort the processes by priority (highest first)
    qsort(plist, n, sizeof(ProcessType), my_comparer);

    // 2- Now simply apply FCFS algorithm.
    findWaitingTime(plist, n);
    findTurnAroundTime(plist, n);

    //Display processes along with all details
    printf("\n*********\nPriority\n");
}

void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 
  
    // Calculate total waiting time and total turn  
    // around time 
    for (int  i=0; i<n; i++) 
    { 
        total_wt = total_wt + plist[i].wt; 
        total_tat = total_tat + plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

ProcessType * initProc(char *filename, int *n)
{
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    // Copy only well-formed lines (pid bt art wt tat pri) into a temporary
    // file so malformed or blank lines never reach parse_file().
    FILE *clean = tmpfile();
    if (!clean) {
        fprintf(stderr, "Error: could not create temporary file\n");
        fclose(input_file);
        exit(1);
    }

    char line[512];
    int lineno = 0, valid = 0;
    while (fgets(line, sizeof(line), input_file)) {
        int pid, bt, art, wt, tat, pri;
        lineno++;
        if (sscanf(line, "%d %d %d %d %d %d", &pid, &bt, &art, &wt, &tat, &pri) == 6
            && bt >= 0 && art >= 0) {
            fprintf(clean, "%d %d %d %d %d %d\n", pid, bt, art, wt, tat, pri);
            valid++;
        } else {
            for (char *c = line; *c; c++)   // ignore pure blank lines silently
                if (*c != ' ' && *c != '\t' && *c != '\r' && *c != '\n') {
                    fprintf(stderr, "Warning: skipping malformed line %d\n", lineno);
                    break;
                }
        }
    }
    fclose(input_file);

    if (valid == 0) {
        fprintf(stderr, "Error: no valid process data in %s\n", filename);
        fclose(clean);
        exit(1);
    }

    rewind(clean);
    *n = 0;
    ProcessType *plist = parse_file(clean, n);
    fclose(clean);

    return plist;
}
  
// Driver code 
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = 2;

    ProcessType *proc_list;
  
    if (argc < 2) {
		   fprintf(stderr, "Usage: ./schedsim <input-file-path> [quantum]\n");
		   fflush(stdout);
		   return 1;
	   }
    
    // Optional second argument: Round Robin time quantum (default 2)
    if (argc >= 3) {
        char *end;
        long q = strtol(argv[2], &end, 10);
        if (*end != '\0' || q <= 0 || q > INT_MAX) {
            fprintf(stderr, "Error: quantum must be a positive integer\n");
            return 1;
        }
        quantum = (int)q;
    }

  // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
  
    findavgTimeFCFS(proc_list, n);
    
    printMetrics(proc_list, n);
    
    free(proc_list);
  
  // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
   
    findavgTimeSJF(proc_list, n); 
   
    printMetrics(proc_list, n);
   
    free(proc_list);
  
  // Priority
    n = 0; 
    proc_list = initProc(argv[1], &n);
    
    findavgTimePriority(proc_list, n); 
    
    printMetrics(proc_list, n);
    
    free(proc_list);
    
  // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    
    findavgTimeRR(proc_list, n, quantum); 
    
    printMetrics(proc_list, n);
    
    free(proc_list);
    
    return 0; 
} 