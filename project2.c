#include <stdio.h>
#include <stdlib.h>

typedef struct InstructionData
{
    unsigned int pid;
    unsigned int burst;
    unsigned int priority;
} InstructionData;

typedef struct InstructionTime
{
    int first;
    int last;
} InstructionTime;

int main(int argc, char *argv[])
{
    // input data, first two lines
    int numProcesses;
    int numExecutionElements;
    int numInstructions;
    
    // data to output
    int voluntary;
    int nonvoluntary;
    double utilization;
    double throughput;
    double turnaround;
    double waiting;
    double response;
    int totalTime;
    int totalWait;
    int totalTurnaround;
    int totalResponse;

    struct InstructionData* instructionList;

    FILE* file;

    // open if not stdin
    if (argc > 1)
    {
        file = fopen(argv[1], "r");
    } 

    // rean first two lines of the file in
    if (argc > 1)
    {
        fscanf(file, "%d\n", &numProcesses);
        fscanf(file, "%d %d\n", &numExecutionElements, &numInstructions);
    }
    else
    {
        fscanf(stdin, "%d\n", &numProcesses);
        fscanf(stdin, "%d %d\n", &numExecutionElements, &numInstructions);
    }

    // allocate memory for array
    instructionList = malloc(sizeof(InstructionData) * numInstructions);

    //initialize array
    for (int i = 0; i < numInstructions; ++i)
    {
        instructionList[i].pid = 0;
        instructionList[i].burst = 0;
        instructionList[i].priority = 0;
    }

    // read values of each process
    for (int i = 0; i < numInstructions; ++i)
    {
        if (argc > 1)
            fscanf(file, "%d %d %d\n", &instructionList[i].pid, &instructionList[i].burst, &instructionList[i].priority);
        else
            fscanf(stdin, "%d %d %d\n", &instructionList[i].pid, &instructionList[i].burst, &instructionList[i].priority);

    }

    // close if not stdin
    if (argc > 1)
        fclose(file);

    // count voluntary and non-voluntary switches
    voluntary = 1; // first switch skipped due to nothing else to compare to
    nonvoluntary = 0;
    int previous = instructionList[0].pid;
    int current;
    for (int i = 1; i < numInstructions; ++i)
    {
        current = instructionList[i].pid;

        // if same pid back to back then there is no switch
        if (previous != current)
        {
            ++voluntary; // assume voluntary until proven nonvoluntary

            // check if process is nonvoluntary rather than voluntary
            for (int j = i + 1; j < numInstructions; ++j)
            {
                if (previous == instructionList[j].pid)
                {
                    ++nonvoluntary;
                    --voluntary;
                    break;
                }
            }
        }
      
        previous = current;
    }

    // get total process time
    totalTime = 0;
    for (int i = 0; i < numInstructions; ++i)
    {
        totalTime += instructionList[i].burst;
    }

    // get total wait and turnaround time
    int wait[numExecutionElements];
    int turnaroundPerProcess[numExecutionElements];
    int responsePerProcess[numExecutionElements];
    int firstOccurance = 0;
    int lastOccurance = 0;

    struct InstructionTime* instructionTimeList;
    instructionTimeList = malloc(sizeof(InstructionTime) * numExecutionElements);
    
    //initialize array; using -1 to check if PID does not exist
    for (int i = 0; i < numExecutionElements; ++i)
    {
        instructionTimeList[i].first = -1;
        instructionTimeList[i].last = -1;
    }

    // check for first and last occurances of a PID
    int currPID;
    for (int i = 0; i < numInstructions; ++i)
    {
        currPID = instructionList[i].pid;

        // restart loop if PID DNE
        if (currPID == 0)
            continue;

        if (instructionTimeList[currPID - 1].first == -1)
        {
            instructionTimeList[currPID - 1].first = i;
            instructionTimeList[currPID - 1].last = i;
        }
        else
        {
            instructionTimeList[currPID - 1].last = i;
        }
    }

    for (int i = 0; i < numExecutionElements; ++i)
    {
        firstOccurance = instructionTimeList[i].first;
        lastOccurance = instructionTimeList[i].last;
        
        // check if PID Does not exist
        if (firstOccurance == -1)
            continue;

        // initialize values per process and calculate
        wait[i] = 0;
        turnaroundPerProcess[i] = 0;
        responsePerProcess[i] = 0;
        for (int j = 0; j < numInstructions; ++j)
        {
            currPID = instructionList[j].pid;

            if (j <= lastOccurance)
                turnaroundPerProcess[i] += instructionList[j].burst;

            if (j < lastOccurance && (currPID != i + 1))
                wait[i] += instructionList[j].burst;

            if (j < firstOccurance)
                responsePerProcess[i] += instructionList[j].burst;
        }
    }
    
    totalWait = 0;
    totalTurnaround = 0;
    totalResponse = 0;
    for (int i = 0; i < numExecutionElements; ++i)
    {
        totalWait += wait[i];
        totalTurnaround += turnaroundPerProcess[i];
        totalResponse += responsePerProcess[i];
    }

    // calculate cpu utilization
    // one process always occupied
    utilization = 100.0;

    // calculate throughput
    throughput = numExecutionElements * 1.0 / totalTime;

    // calculate turnaround time
    turnaround = totalTurnaround * 1.0 / numExecutionElements;

    // calculate waiting time
    waiting = totalWait * 1.0 / numExecutionElements;

    // calculate response time
    response = totalResponse * 1.0 / numExecutionElements;

    printf("%d\n", voluntary);
    printf("%d\n", nonvoluntary);
    printf("%.2f\n", utilization);
    printf("%.2f\n", throughput);
    printf("%.2f\n", turnaround);
    printf("%.2f\n", waiting);
    printf("%.2f\n", response);
}