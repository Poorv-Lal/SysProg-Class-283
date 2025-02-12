//Name: Poorv Lal - pbl37
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"


/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
  	memset(clist, 0, sizeof(command_list_t));
  	//initalize the variables 
	char *token = strtok(cmd_line,PIPE_STRING); // the first segment of the comand line before the pipe hits
	int count =0; //how many comands do we have???
	//char *temp_str;

	//go though each segemment of the input (token)
	while (token != NULL) 
	{
		//printf("%d\n", count);
		//check if we are past the number of allowed commands
		if (count >= CMD_MAX){
			//printf("HERE?");
			return ERR_TOO_MANY_COMMANDS;
		}
		
		//ignore any leading spaces
		while (*token == ' ' || *token == '\t'){
			token++;
		}
		
		size_t len = strlen(token) + 1;  // +1 for null terminator
		char *temp_str = (char *)malloc(len);
		int space_found=-1;
		int exe_count=0;
		int arg_count=0;

		if (!temp_str) {
		    return ERR_CMD_OR_ARGS_TOO_BIG;  // Handle memory allocation failure
		    }

		    // Copy the token into temp_str
		    strcpy(temp_str, token);

		    // Iterate over temp_str (character by character)
		    for (char *ptr = temp_str; *ptr != '\0'; ptr++) {
		        if(*ptr == ' '){
					if (space_found == -1) {
						space_found = 0;
					}
		        }

		        if (space_found == -1 )
		        {
					if(exe_count >= EXE_MAX)
					{
						return ERR_CMD_OR_ARGS_TOO_BIG;
					}
					else{
						clist->commands[count].exe[exe_count] = *ptr;
						exe_count = exe_count +1;
					}
		        }
		        else if (space_found == 0)
		        {
					space_found = 1;
		        }
		        else{
					if(arg_count >=ARG_MAX)
					{
						return ERR_CMD_OR_ARGS_TOO_BIG;
					}
					else{
						clist->commands[count].args[arg_count] = *ptr;
						arg_count = arg_count +1;
					}
		        }


		        	//printf("Character: %c\n", *ptr);  // Process each character
		    }

		    // Free allocated memory after use
		    free(temp_str);



		count++;
		//printf("cmd:  %s\n", token);
		token = strtok(NULL, PIPE_STRING);
	
	}

	
	//store the firsnal lenth of the clist
	clist->num = count;
	
	//if an argument is empty make it empty
	/*for(int i = 0; i<clist->num; i++ ){
		if(clist->commands[count].args == ' ' || clist->commands[count].args == "" )
		{
			clist->commands[count].args= '\0';
		}
	}*/

	//return statemts
	if (count == 0) {
	    return WARN_NO_CMDS;
	}

	return OK;
	
  	//printf(M_NOT_IMPL);
    //return EXIT_NOT_IMPL;
}
