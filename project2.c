#include <stdio.h>
#include <stdlib.h>

typedef struct Command
{
    char *pid;
    unsigned int size;
} Command;

int main(int argc, char *argv[])
{
    char *algorithm = argv[1];  // algorithm
    char *temp;
    int allocate = strtol(argv[2], &temp, 10);     // total memory allocation

    // open file in read mode
    FILE* file;
    file = fopen(argv[3], "r");

    char command[9]; // longest command is AVAILABLE
    char processID[16]; // largest process label is 16 characters
    int allocatePID;

    struct Command* commandList; 
    commandList = malloc(sizeof(Command) * allocate); // / max size of the array requested
    
    // initialize array, 0 means empty, 1 means occupied
    for (int i = 0; i < allocate; ++i)
    {
        commandList[i].pid = "empty";
        commandList[i].size = 0;
        printf("%d\n", i);
    }

    int nextHold = 0;
    int returned; // value returned from algorithms and findPID

    fscanf(file, "%s", &command); // priming read
    while (!feof(file))
    {
        if (strcmp(command, "REQUEST") == 0)
        {
            fscanf(file, "%s %d\n", &processID, &allocatePID);
         
            // run correct algorithm
            if (strcmp(algorithm, "FIRSTFIT") == 0)
                returned = firstFit(commandList, allocatePID, allocate);
            else if (strcmp(algorithm, "BESTFIT") == 0)
                returned = nextFit(commandList, allocatePID, allocate, nextHold);
            else if (strcmp(algorithm, "WORSTFIT") == 0)
                returned = bestFit(commandList, allocatePID, allocate);
            else if (strcmp(algorithm, "NEXTFIT") == 0)
                returned = worstFit(commandList, allocatePID, allocate);

            // no valid space to put element
            if (returned == -1)
                printf("FAIL REQUEST %s %d\n", processID, allocatePID);
            else
            {
                // valid space to put element
                for (int i = returned; i < allocatePID && i < allocate; ++i)
                {
                    commandList[i].pid = processID;
                    commandList[i].size = allocatePID;
                }   

                printf("ALLOCATED %s %d\n", processID, returned);
            }
        }
        else if (strcmp(command, "RELEASE") == 0)
        {
            fscanf(file, "%s\n", &processID);

            // check if PID exists to be released 
            returned = findPID(commandList, processID, allocate);

            // pid not found
            if (returned == -1)
                printf("FAIL RELEASE %s\n", processID);
            // pid found
            else
            {
                // get size of process
                int size = getSize(commandList, returned);

                // reset process data
                for (int i = returned; i < (returned + size); ++i)
                {
                    commandList[i].pid = "empty";
                    commandList[i].size = 0;
                }
                printf("FREE %s,%d,%d\n", processID, size, returned);
            }
        }
        else if (strcmp(command, "LIST") == 0)
        {
            fscanf(file, "%s\n", &command);

            if (strcmp(command, "AVAILABLE") == 0)
            {   
                // start and end of values to print
                int start = -1;
                int end = -1;
                int printed = 0;

                // search for available space
                for (int i = 0; i < allocate; ++i)
                {   
                    // start is blank and area found
                    if (commandList[i].size == 0 && start == -1)
                    {
                        start = i;
                        end = i;
                    }
                    // in empty block of array
                    else if (commandList[i].size == 0)
                    {
                        end = i;

                        // check if entire array is empty
                        if (i + 1 == allocate)
                        {
                            printf("(%d,%d)", start, end - start + 1);
                            ++printed;
                            start = -1;
                            end = -1;
                        }
                    }
                    // occupied space in array
                    else
                    {
                        // check if previously went through empty block in array
                        if (start != end || (start > -1 && end > -1))
                        {
                            printf("(%d,%d)", start, end - start + 1);
                            ++printed;
                            start = -1;
                            end = -1;
                        }
                    }
                }

                // no available space
                if (printed == 0)
                {
                    printf("FULL");
                }

                // end with new line
                printf("\n");
            }
            else if (strcmp(command, "ASSIGNED") == 0)
            {
                // start and end of values to print
                int start = -1;
                int end = -1;
                int printed = 0;

                // search for occupied space
                for (int i = 0; i < allocate; ++i)
                {   
                    // start is blank and occupied area found
                    if (commandList[i].size != 0 && start == -1)
                    {
                        start = i;
                        end = i;
                    }
                    // in occupied block of array
                    else if (commandList[i].size != 0)
                    {
                        end = i;

                        // check if entire array is occupied
                        if (i + 1 == allocate)
                        {
                            printf("(%s,%d,%d)", commandList[start].pid, start, end - start + 1);
                            ++printed;
                            start = -1;
                            end = -1;
                        }
                    }
                    // blank space in array
                    else
                    {
                        // check if previously went through occupied block in array
                        if (start != end || (start > -1 && end > -1))
                        {
                            printf("(%s,%d,%d)", commandList[start].pid, start, end - start + 1);
                            ++printed;
                            start = -1;
                            end = -1;
                        }
                    }
                }

                // no occupied space in array
                if (printed == 0)
                {
                    printf("NONE");
                }

                // end with new line
                printf("\n");
            }
        }
        else if (strcmp(command, "FIND") == 0)
        {
            fscanf(file, "%s\n", &processID);
            
            // find if PID exists
            returned = findPID(commandList, processID, allocate);

            // pid not found
            if (returned == -1)
                printf("FAULT\n");
            // pid found
            else
            {
                int size = getSize(commandList, returned);
                printf("(%s,%d,%d\n)", processID, size, returned);
            }
        }

        fscanf(file, "%s", &command); // priming read
    }
    
    // close file
    fclose(file);
}

int firstFit(Command *array, int processSize, int totalSize)
{   
    // start and end of blank area
    int start = -1;
    int end = -1;
    int printed = 0;

    for (int i = 0; i < totalSize; ++i)
    {    
        // start is blank and area found
        if (array[i].size == 0 && start == -1)
        {
            start = i;
            end = i;
        }
        // in empty block of array
        else if (array[i].size == 0)
        {
            end = i;

            // check if array fits in current location, if so return
            if (end - start + 1 >= processSize)
                    return start;

            // check if entire array is empty
            if (i + 1 == totalSize && start == 0)
                return 0;  
        }         
        // occupied space in array
        else
        {
            // check if previously went through empty block in array
            if (start != end || (start > -1 && end > -1))
            {
                // check if array fits in current location, if so return
                if (end - start + 1 >= processSize)
                    return start;

                start = -1;
                end = -1;
            }
        }
    }   

    return -1;
}

int nextFit(Command *array, int processSize, int totalSize, int currentLocation)
{
    for (int i = currentLocation; i < totalSize; ++i)
    {

    } 

    for (int i = 0; i < currentLocation; ++i)
    {

    } 
    
    return -1;
}

int bestFit(Command *array, int processSize, int totalSize)
{
    for (int i = 0; i < totalSize; ++i)
    {

    } 

    return -1;
}

int worstFit(Command *array, int processSize, int totalSize)
{
    for (int i = 0; i < totalSize; ++i)
    {

    } 

    return -1;
}

// Searches for location of processID and returns it. Returns -1 if not found
int findPID(Command *array, char processID[], int totalSize)
{
    int found;

    for (int i = 0; i < totalSize; ++i)
    {
        if (strcmp(processID, array[i].pid) == 0)
            return i;
    }

    return -1;
}

int getSize(Command *array, int location)
{
    return array[location].size;
}
