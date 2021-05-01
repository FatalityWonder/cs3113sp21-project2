#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *algorithm = argv[1];  // algorithm
    int allocate = argv[2];     // total memory allocation

    // open file in read mode
    FILE* file;
    file = fopen(argv[3], "r");

    char command[9]; // longest command is AVAILABLE
    char processID[16]; // largest process label is 16 characters
    int allocatePID;

    fscanf(file, "%s", &command); // priming read
    while (!feof(file))
    {
        if (strcmp(command, "REQUEST") == 0)
        {
            fscanf(file, "%s %d\n", &processID, &allocatePID);
            
            if (strcmp(algorithm, "FIRSTFIT") == 0)
            {

            }
            else if (strcmp(algorithm, "BESTFIT") == 0)
            {

            }
            else if (strcmp(algorithm, "WORSTFIT") == 0)
            {

            }
            else if (strcmp(algorithm, "NEXTFIT") == 0)
            {

            }
        }
        else if (strcmp(command, "RELEASE") == 0)
        {
            fscanf(file, "%s\n", &processID);

        }
        else if (strcmp(command, "LIST") == 0)
        {
            fscanf(file, "%s\n", &command);

            if (strcmp(command, "AVAILABLE") == 0)
            {

            }
            else if (strcmp(command, "ASSIGNED") == 0)
            {
                
            }
        }
        else if (strcmp(command, "FIND") == 0)
        {
            fscanf(file, "%s\n", &processID);

        }
        printf(command);
        printf("\n");

        fscanf(file, "%s", &command); // priming read
    }
    
    // close file
    fclose(file);
}
