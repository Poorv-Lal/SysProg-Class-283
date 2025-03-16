//Name: Poorv Lal Bareli pbl37
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);
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
  		/*
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
  		
  		*/

  		if (build_cmd_buff(token, &clist->commands[count]) != OK) {
  			return ERR_CMD_ARGS_BAD;
  		}
  		count++;
 		//printf("cmd:  %s\n", token);
  		token = strtok(NULL, PIPE_STRING);

	}
	
	
	//store the firsnal lenth of the clist
	clist->num = count;
	
	
	//return statemts
	if (count == 0) {
		return WARN_NO_CMDS;
	}
	
	return OK;	
}

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

//there is only one command 
int exec_cmd(cmd_buff_t *cmd) {
	//if tehy want to leave
	if (strcmp(cmd->argv[0], "exit") == 0) {
		exit(0);
	} 
	
	//if they want to cd
	else if (strcmp(cmd->argv[0], "cd") == 0) {
		if (cmd->argc > 1) {
			if (chdir(cmd->argv[1]) != 0) {
			perror("cd failed");
			}
		}
		return 0;
	} 
	
	//fork stuff
	else {
		pid_t pid = fork();
		if (pid == 0) {
			execvp(cmd->argv[0], cmd->argv);
			perror("exec failed");
			exit(ERR_EXEC_CMD);
		} else if (pid > 0) {
			int status;
			waitpid(pid, &status, 0);
		} else {
			perror("fork failed");
			return -1;
		}
	}
	return 0;
}


//there are pipes that need to be proccesed
int execute_pipeline(command_list_t *clist) {
	int pipes[CMD_MAX - 1][2];
	pid_t pids[CMD_MAX];
	
	for (int i = 0; i < clist->num - 1; i++) {
		if (pipe(pipes[i]) == -1) {
			perror("pipe failed");
			return -1;
		}
	}
	
	//fork stuff
	for (int i = 0; i < clist->num; i++) {
		pids[i] = fork();
		if (pids[i] == 0) {
			if (i > 0) {
				dup2(pipes[i - 1][0], STDIN_FILENO);
			}
			if (i < clist->num - 1) {
				dup2(pipes[i][1], STDOUT_FILENO);
			}
			for (int j = 0; j < clist->num - 1; j++) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			execvp(clist->commands[i].argv[0], clist->commands[i].argv);
			perror("exec failed");
			exit(ERR_EXEC_CMD);
		}
	}
	
	for (int i = 0; i < clist->num - 1; i++) {
		close(pipes[i][0]);
		close(pipes[i][1]);
	}
	
	for (int i = 0; i < clist->num; i++) {
		waitpid(pids[i], NULL, 0);
		
	}
	
	return 0;
}

int exec_local_cmd_loop() {
	char cmd_buff[SH_CMD_MAX];
	command_list_t clist;
	int rc;
	
	while (1) {
		printf("%s", SH_PROMPT);
		if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
			printf("\n");
			break;
		}
		cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
		
		//they want to leave
		if (strcmp(cmd_buff, EXIT_CMD) == 0) {
			break;
		}
		
		//Parse the gieven command and store in cdm
		rc = build_cmd_list(cmd_buff, &clist);
		if (rc == OK) {
			
			//there are no pipes, just one command
			if (clist.num == 1) {
				exec_cmd(&clist.commands[0]);
			} 

			//there are pipes and we need to do extra stuff
			else {
				execute_pipeline(&clist);
			}
		} 

		//they gave us bad things :(
		else if (rc == WARN_NO_CMDS) {
			printf(CMD_WARN_NO_CMD);
		}
		else if (rc == ERR_TOO_MANY_COMMANDS) {
			printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
		}
	}
	return OK;
}
