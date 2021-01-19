/* File: ArchA2
 * Author: Andrew Mercer
 * School: Dalhousie University 
 * Dept: Electrical and Computer Eng 
 * 
 * Purpose: To emeulate loading an dumping data from memory. 
 * 
 * Note: Main.c
 * 
 * Last Modified: 2020.06.25
 * 
 */

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include"header.h"
#include<stdlib.h>


/* Function: main.c
 * 
 * Description: Gets command line arguments, determines if debug file was included in the 
 * command line arguments and if so, calls load function. Issues warnings if incorrect 
 * user input.
 * 
 * Arguments: argc,argv
 * 
 */

int main(int argc, char **argv)
{
    
    
    FILE * fin;
    
    if(argc == NO_ARGS)
    {
        debugger_flag = true;
        DEBUGGER(fin);
        fclose(fin);
    }
    else if(argc == TWO_ARGS)
    {
        if(strcmp(argv[1],"-d") == 0)
        {
            if((fin = fopen(argv[FIRST_ARG],"r")) != NULL)
            { 
                LOADER(fin);
                rewind(fin);
                DEBUGGER(fin);
                fclose(fin);
            }
            else
            {
                printf("File Not found %s\n",argv[FIRST_ARG]);
            }
        }
        else
        {
            printf("Invalid option: %s\n",argv[1]);
        }
    }
    else
    {
        printf("Invalid number of arguments\n");
        getchar();
    }
    

    return 0;
}



