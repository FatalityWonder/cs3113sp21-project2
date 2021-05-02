#include <stdio.h>
#include <stdlib.h>

typedef struct Command
{
    char pid[16];
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
    
    // initialize array
    for (int i = 0; i < allocate; ++i)
    {
        strcpy(commandList[i].pid, "empty");
        commandList[i].size = 0;
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
            {
                returned = firstFit(commandList, allocatePID, allocate);
            }
            else if (strcmp(algorithm, "BESTFIT") == 0)
            {
                returned = nextFit(commandList, allocatePID, allocate, nextHold);
            }
            else if (strcmp(algorithm, "WORSTFIT") == 0)
            {
                returned = bestFit(commandList, allocatePID, allocate);
            }
            else if (strcmp(algorithm, "NEXTFIT") == 0)
            {
                returned = worstFit(commandList, allocatePID, allocate);
            }

            // no valid space to put element
            if (returned == -1)
                printf("FAIL REQUEST %s %d\n", processID, allocatePID);
            else
            {
                // valid space to put element
                for (int i = returned; i < allocatePID + returned; ++i)
                {
                    strcpy(commandList[i].pid, processID);
                    commandList[i].size = allocatePID;
                }   

                if (strcmp(algorithm, "NEXTFIT") == 0)
                {   
                    nextHold = allocatePID + returned;
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
                    strcpy(commandList[i].pid, "empty");
                    commandList[i].size = 0;
                }
                printf("FREE %s %d %d\n", processID, size, returned);
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
                int i = 0; // iterator

                // search for available space
                for (i; i <= allocate; ++i)
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
                        if (i == allocate)
                        {
                            printf("(%d, %d) ", end - start, start);
                            ++printed;
                            start = -1;
                            end = -1;
                        }
                    }
                    // occupied space in array
                    else
                    {
                        // check if previously went through empty block in array
                        if (start != end)
                        {
                            printf("(%d, %d) ", end - start + 1, start);
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

                    if (strcmp(algorithm, "NEXTFIT") == 0)
                    {
                        nextHold = 0;
                    }
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
                char currPID[16];  // check if multiple PID connected
                strcpy(currPID, "empty"); //initialize as empty

                // search for occupied space
                for (int i = 0; i <= allocate; ++i)
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
                        if (i == allocate)
                        {
                            printf("(%s, %d, %d) ", commandList[start].pid, end - start, start);
                            ++printed;
                            start = -1;
                            end = -1;
                        }
                        // check if PID changed
                        else if (strcmp(currPID, "empty") != 0 && strcmp(currPID, commandList[i].pid) != 0)
                        {
                            printf("(%s, %d, %d) ", commandList[start].pid, end - start, start);
                            ++printed;
                            start = i;
                            end = i;
                        }
                    }
                    // blank space in array
                    else
                    {
                        // check if previously went through occupied block in array
                        if (start != end)
                        {
                            printf("(%s, %d, %d) ", commandList[start].pid, end - start + 1, start);
                            ++printed;
                            start = -1;
                            end = -1;
                        }
                    }

                    strcpy(currPID, commandList[i].pid);
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
                printf("(%s, %d, %d)\n", processID, size, returned);
            }
        }
        //printf(command);
        //("\n");

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

    // porocess does not fit at all in array
    if (processSize > totalSize)
        return -1;

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
            if ((end - start + 1) >= processSize)
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
                if ((end - start + 1) >= processSize)
                    return start;

                // process does not fit
                start = -1;
                end = -1;
            }
        }
    }   

    return -1;
}

int nextFit(Command *array, int processSize, int totalSize, int currentLocation)
{
    int start = -1;
    int end = -1;

    // porocess does not fit at all in array
    if (processSize > totalSize)
        return -1;

    for (int i = currentLocation; i < totalSize; ++i)
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
            if ((end - start + 1) >= processSize)
                return start;

            // check if entire array is empty
            if (i + 1 == totalSize - currentLocation && start == 0)
                return currentLocation + 1;  
        }         
        // occupied space in array
        else
        {
            // check if previously went through empty block in array
            if (start != end || (start > -1 && end > -1))
            {
                // check if array fits in current location, if so return
                if ((end - start + 1) >= processSize)
                    return start;

                // process does not fit
                start = -1;
                end = -1;
            }
        }
    } 

    for (int i = 0; i < currentLocation; ++i)
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
            if ((end - start + 1) >= processSize)
                return start;

            // check if entire array is empty
            if (i + 1 == totalSize - currentLocation && start == 0)
                return currentLocation + 1;  
        }         
        // occupied space in array
        else
        {
            // check if previously went through empty block in array
            if (start != end || (start > -1 && end > -1))
            {
                // check if array fits in current location, if so return
                if ((end - start + 1) >= processSize)
                    return start;

                // process does not fit
                start = -1;
                end = -1;
            }
        }
    } 
    
    return -1;
}

int bestFit(Command *array, int processSize, int totalSize)
{
    // start and end of blank area
    int start = -1;
    int end = -1;
    int currentSize = 0;    // size of current found block
    int smallestSize = totalSize;    // size of array is minimum
    int smallestLocation = -1 ; // location of smallest free block, -1 if no valid locations found

    // porocess does not fit at all in array
    if (processSize > totalSize)
        return -1;

    for (int i = 0; i < totalSize; ++i)
    {    
        // start is blank and area found
        if (array[i].size == 0 && start == -1)
        {
            start = i;
            end = i;
            ++currentSize;
        }
        // in empty block of array
        else if (array[i].size == 0)
        {
            end = i;
            ++currentSize;

            // check if array fits in current location
            if ((end - start + 1) >= processSize && currentSize < smallestSize)
            {
                if (i + 1 < totalSize && array[i + 1].size != 0)
                {
                    smallestLocation = start;
                    smallestSize = currentSize;
                }
            }

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
                // check if array fits in current location
                if ((end - start + 1) >= processSize && currentSize < smallestSize)
                {
                    smallestLocation = start;
                    smallestSize = currentSize;
                }

                // process does not fit
                start = -1;
                end = -1;
                currentSize = 0;
            }
        }
    }   

    return smallestLocation;
}

int worstFit(Command *array, int processSize, int totalSize)
{
    // start and end of blank area
    int start = -1;
    int end = -1;
    int currentSize = 0;    // size of current found block
    int largestSize = 0;    // size of largest found block  
    int largestLocation = -1 ; // location of largest free block, -1 if no valid locations found

    // porocess does not fit at all in array
    if (processSize > totalSize)
        return -1;

    for (int i = 0; i < totalSize; ++i)
    {    
        // start is blank and area found
        if (array[i].size == 0 && start == -1)
        {
            start = i;
            end = i;
            ++currentSize;
        }
        // in empty block of array
        else if (array[i].size == 0)
        {
            end = i;
            ++currentSize;

            // check if array fits in current location
            if ((end - start + 1) >= processSize && currentSize > largestSize)
            {
                largestLocation = start;
                largestSize = currentSize;
            }

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
                // check if array fits in current location
                if ((end - start + 1) >= processSize && currentSize > largestSize)
                {
                    largestLocation = start;
                    largestSize = currentSize;
                }

                // process does not fit
                start = -1;
                end = -1;
                currentSize = 0;
            }
        }
    }   

    return largestLocation;
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
