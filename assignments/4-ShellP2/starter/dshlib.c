//Name: Poorv Lal Bareli - pbl37
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */


int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
	//imeadiatly store the given string
	cmd_buff->_cmd_buffer = strdup(cmd_line);
		
	//initalize variables
	int arg_index = 0;  // Tracks the number of arguments
    int in_quotes = 0;  // Flag to track whether inside quotes
	char *ptr = cmd_line;  // Pointer to iterate through the input
    char *start_arg = NULL;  // Pointer to start of the current argument

	//skip any inital leading spaces
	while (*ptr == ' ' || *ptr == '\t') {
		ptr++;
	}
	
	//go though each segemment of the input (token)
	while (*ptr != '\0'){
		//if you find a quote, everything inside is suposed to be there
		if (*ptr == '"') {
			//in_quotes = !in_quotes;
			if (in_quotes == 0 ) {
			// End of the quoted argument, move to the next character
				in_quotes = 1;
			} else {
			// Start of the quoted argument
				in_quotes = 0;
			}
			ptr++;
			continue;  // Skip the quote character
		}
		
		//if we are inside a quote or just have a regular character, we want to store this information
		//take note of this ptr for later
		else if (in_quotes==1 ||( *ptr != ' ' && *ptr != '\t')) {
			// Start a new argument when we encounter the beginning of an argument
			if (start_arg == NULL) {
				start_arg = ptr;
			}
			ptr++;
		}

		//if we are at the end of the arguemt we can store the information amd reset (or end if needed)
		else{
			if (start_arg != NULL) {
				*ptr = '\0';  
				
				//if the last character in the string is a quote, remove it too
				int len = strlen(start_arg);
				if (start_arg[len-1] =='"' ){
					start_arg[len-1] ='\0';
				}

				cmd_buff->argv[arg_index] = start_arg;  
				start_arg = NULL;
				arg_index++;
				ptr++;
			}
			
			//skip whitespace until next argumetnt
			while (*ptr == ' ' || *ptr == '\t') {
				ptr++;
			}
		}


	}
	
	// If there's an argument being processed at the end of the string, add it
	if (start_arg != NULL) {
		//if the last character in the string is a quote, remove it too
		int len = strlen(start_arg);
		if (start_arg[len-1] =='"' ){
			start_arg[len-1] ='\0';
		}
		
		cmd_buff->argv[arg_index] = start_arg;
		arg_index++;
	}
	cmd_buff->argc = arg_index;

	cmd_buff ->argv[arg_index] = NULL; // the array of arguments end will a null

	//return 
	if (arg_index > 0) {
	    return OK;
	} else {
	    return WARN_NO_CMDS; 
	}

}



int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    int rc = 0;
    cmd_buff_t cmd;

    // TODO IMPLEMENT MAIN LOOP
    while(1){
    	printf("%s", SH_PROMPT);
    	if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
    		printf("\n");
    		break;
    	}
    	//remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
    	
		//they want to leave
		if(strcmp(cmd_buff,EXIT_CMD) == 0 ){
			break;
		}
		
		// TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff
		//Parse the gieven command and store in cdm
		rc = build_cmd_buff(cmd_buff,&cmd);
		
		//go though what tehy gave us back
		//they gave us the correct good thing that we want
		if(rc == OK )
		{
			//printf(CMD_OK_HEADER, cmd.argc);
			//for (int i = 0; i < cmd.argc; i++)
			//{
			//	printf("Arguemnt %d: %s\n", cmd.argc,cmd.argv[i]);	
			//}

			// TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
			// the cd command should chdir to the provided directory; if no directory is provided, do nothing

			if (strcmp(cmd.argv[0], "exit") == 0) {
			    return 0; 
			 }
			 else if (strcmp(cmd.argv[0], "cd") == 0) {
			 	if (cmd.argc > 1) {
			 		if (chdir(cmd.argv[1]) != 0) {
			 			perror("cd failed");
			 		}
			 	}
			}
			else {
			// TODO IMPLEMENT if not built-in command, fork/exec as an external command
			// for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
				pid_t pid = fork();
				if (pid == 0) { // Child process
					execvp(cmd.argv[0], cmd.argv);
					perror("Error: exec failed\n"); // If exec fails
					exit(ERR_EXEC_CMD);
				} else if (pid > 0) { // Parent process
					int status;
					waitpid(pid, &status, 0);
				} else {
					perror("Error: fork failed\n");
				}
			}

		}
		
		//they gave us bad things :(
		else if (rc == WARN_NO_CMDS)
		{
			printf(CMD_WARN_NO_CMD);
		}
		else if (rc == ERR_TOO_MANY_COMMANDS)
		{
			printf(CMD_ERR_PIPE_LIMIT,CMD_MAX);
		}
	
	}
    return OK;
}
