//Poorv Lal (Bareli) pbl37
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>

//INCLUDES for extra credit
//#include <signal.h>
//#include <pthread.h>
//-------------------------

#include "dshlib.h"
#include "rshlib.h"


/*
 * start_server(ifaces, port, is_threaded)
 *      ifaces:  a string in ip address format, indicating the interface
 *              where the server will bind.  In almost all cases it will
 *              be the default "0.0.0.0" which binds to all interfaces.
 *              note the constant RDSH_DEF_SVR_INTFACE in rshlib.h
 * 
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -s implemented in dsh_cli.c
 *              For example ./dsh -s 0.0.0.0:5678 where 5678 is the new port  
 * 
 *      is_threded:  Used for extra credit to indicate the server should implement
 *                   per thread connections for clients  
 * 
 *      This function basically runs the server by: 
 *          1. Booting up the server
 *          2. Processing client requests until the client requests the
 *             server to stop by running the `stop-server` command
 *          3. Stopping the server. 
 * 
 *      This function is fully implemented for you and should not require
 *      any changes for basic functionality.  
 * 
 *      IF YOU IMPLEMENT THE MULTI-THREADED SERVER FOR EXTRA CREDIT YOU NEED
 *      TO DO SOMETHING WITH THE is_threaded ARGUMENT HOWEVER.  
 */
int start_server(char *ifaces, int port, int is_threaded){
    int svr_socket;
    int rc;

    //
    //TODO:  If you are implementing the extra credit, please add logic
    //       to keep track of is_threaded to handle this feature
    //
	is_threaded = is_threaded;

    svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0){
        int err_code = svr_socket;  //server socket will carry error code
        return err_code;
    }

    rc = process_cli_requests(svr_socket);

    stop_server(svr_socket);


    return rc;
}

/*
 * stop_server(svr_socket)
 *      svr_socket: The socket that was created in the boot_server()
 *                  function. 
 * 
 *      This function simply returns the value of close() when closing
 *      the socket.  
 */
int stop_server(int svr_socket){
    return close(svr_socket);
}

/*
 * boot_server(ifaces, port)
 *      ifaces & port:  see start_server for description.  They are passed
 *                      as is to this function.   
 * 
 *      This function "boots" the rsh server.  It is responsible for all
 *      socket operations prior to accepting client connections.  Specifically: 
 * 
 *      1. Create the server socket using the socket() function. 
 *      2. Calling bind to "bind" the server to the interface and port
 *      3. Calling listen to get the server ready to listen for connections.
 * 
 *      after creating the socket and prior to calling bind you might want to 
 *      include the following code:
 * 
 *      int enable=1;
 *      setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
 * 
 *      when doing development you often run into issues where you hold onto
 *      the port and then need to wait for linux to detect this issue and free
 *      the port up.  The code above tells linux to force allowing this process
 *      to use the specified port making your life a lot easier.
 * 
 *  Returns:
 * 
 *      server_socket:  Sockets are just file descriptors, if this function is
 *                      successful, it returns the server socket descriptor, 
 *                      which is just an integer.
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code is returned if the socket(),
 *                               bind(), or listen() call fails. 
 * 
 */
int boot_server(char *ifaces, int port){
   // return WARN_RDSH_NOT_IMPL;
   int server_socket;
   struct sockaddr_in server_addr;

   //1 Create the server socket using socket() function
   server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		perror("Socket creation failed");
		return ERR_RDSH_SERVER;
	}
	
	//server_addr.sin_family = AF_INET;
	//server_addr.sin_port = htons(port);
	//server_addr.sin_addr.s_addr = inet_addr(ifaces);
	
	int enable=1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
		perror("set socket failed");
		close(server_socket);
		return ERR_RDSH_COMMUNICATION;

	}
	
	//memset(&adder, 0, sizeof(addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ifaces);

	//2 Calling bind to "bind" the server to the interface and port
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Bind failed");
		close(server_socket);
		return ERR_RDSH_SERVER;
	}
	
	//3 Calling listen to get the server ready to listen for connections.
	if (listen(server_socket, 5) < 0) {
		perror("Listen failed");
		close(server_socket);
		return ERR_RDSH_SERVER;
	}
	
	printf("Server listening on %s:%d\n", ifaces, port);
	return server_socket;
}

/*
 * process_cli_requests(svr_socket)
 *      svr_socket:  The server socket that was obtained from boot_server()
 *   
 *  This function handles managing client connectionsi.  It does this using
 *  the following logic
 * 
 *      1.  Starts a while(1) loop:
 *  
 *          a. Calls accept() to wait for a client connection. Recall that 
 *             the accept() function returns another socket specifically
 *             bound to a client connection. 
 *          b. Calls exec_client_requests() to handle executing commands
 *             sent by the client. It will use the socket returned from
 *             accept().
 *          c. Loops back to the top (step 2) to accept connecting another
 *             client.  
 * 
 *          note that the exec_client_requests() return code should be
 *          negative if the client requested the server to stop by sending
 *          the `stop-server` command.  If this is the case step 2b breaks
 *          out of the while(1) loop. 
 * 
 *      2.  After we exit the loop, we need to cleanup.  Dont forget to 
 *          free the buffer you allocated in step #1.  Then call stop_server()
 *          to close the server socket. 
 * 
 *  Returns:
 * 
 *      OK_EXIT:  When the client sends the `stop-server` command this function
 *                should return OK_EXIT. 
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code terminates the loop and is
 *                returned from this function in the case of the accept() 
 *                function failing. 
 * 
 *      OTHERS:   See exec_client_requests() for return codes.  Note that positive
 *                values will keep the loop running to accept additional client
 *                connections, and negative values terminate the server. 
 * 
 */
int process_cli_requests(int svr_socket){
   // return WARN_RDSH_NOT_IMPL;
   struct sockaddr_in cli_addr;
   socklen_t cli_len = sizeof(cli_addr);
   int cli_socket;
   //1.  Starts a while(1) loop:   
   while (1) {
		//a. Calls accept() to wait for a client connection. Recall that
		// *             the accept() function returns another socket specifically
		//  *             bound to a client connection.
		cli_socket = accept(svr_socket, (struct sockaddr *)&cli_addr, &cli_len);
		if (cli_socket < 0) {
			perror("Accept failed");
			return ERR_RDSH_COMMUNICATION;
		}
		
		printf("Client connected.\n");
		
		//b. Calls exec_client_requests() to handle executing commands
		// *             sent by the client. It will use the socket returned from
		//  *             accept().
		int rc = exec_client_requests(cli_socket);
		//close(cli_socket);
		
		//note that the exec_client_requests() return code should be
		// *          negative if the client requested the server to stop by sending
		 // *          the `stop-server` command.  If this is the case step 2b breaks
		 //  *          out of the while(1) loop.
		if (rc == STOP_SERVER_SC) {
			printf(RCMD_MSG_SVR_STOP_REQ);
			break;
		}
		printf(RCMD_MSG_CLIENT_EXITED);
	}
	//2. After we exit the loop, we need to cleanup.  Dont forget to
	// *          free the buffer you allocated in step #1.  Then call stop_server()
	//  *          to close the server socket.
	//stop_server(svr_socket);

	return OK;
}

int rsh_exec_cmd(cmd_buff_t *cmd);
/*
 * exec_client_requests(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 *   
 *  This function handles accepting remote client commands. The function will
 *  loop and continue to accept and execute client commands.  There are 2 ways
 *  that this ongoing loop accepting client commands ends:
 * 
 *      1.  When the client executes the `exit` command, this function returns
 *          to process_cli_requests() so that we can accept another client
 *          connection. 
 *      2.  When the client executes the `stop-server` command this function
 *          returns to process_cli_requests() with a return code of OK_EXIT
 *          indicating that the server should stop. 
 * 
 *  Note that this function largely follows the implementation of the
 *  exec_local_cmd_loop() function that you implemented in the last 
 *  shell program deliverable. The main difference is that the command will
 *  arrive over the recv() socket call rather than reading a string from the
 *  keyboard. 
 * 
 *  This function also must send the EOF character after a command is
 *  successfully executed to let the client know that the output from the
 *  command it sent is finished.  Use the send_message_eof() to accomplish 
 *  this. 
 * 
 *  Of final note, this function must allocate a buffer for storage to 
 *  store the data received by the client. For example:
 *     io_buff = malloc(RDSH_COMM_BUFF_SZ);
 *  And since it is allocating storage, it must also properly clean it up
 *  prior to exiting.
 * 
 *  Returns:
 * 
 *      OK:       The client sent the `exit` command.  Get ready to connect
 *                another client. 
 *      OK_EXIT:  The client sent `stop-server` command to terminate the server
 * 
 *      ERR_RDSH_COMMUNICATION:  A catch all for any socket() related send
 *                or receive errors. 
 */
int exec_client_requests(int cli_socket) {
   // return WARN_RDSH_NOT_IMPL;
   char* io_buff= malloc(RDSH_COMM_BUFF_SZ);
   command_list_t clist;
   int rc;
   int  recv_size;         //the +1 includes the NULL byte
   int  is_last_chunk;     //boolean to see if this is the last chunk
   char eof_char = '\0';
   //send(cli_socket,"Hello?\n",8,0);
   //printf("Heloo?");
   while (1) {
		recv_size = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0);
		if (recv_size < 0) {
			free(io_buff);
			return ERR_RDSH_COMMUNICATION;
		}
		else if(recv_size == 0){
			free(io_buff);
			return OK;
		}

		is_last_chunk = ((char)io_buff[recv_size - 1] == eof_char)? 1 : 0 ;
		if (is_last_chunk) {
			io_buff[recv_size - 1] = '\0';
		}
		
		printf("%.*s", recv_size, io_buff);
	

   		//they want to leave
   		//if (strcmp(io_buff, EXIT_CMD) == 0 || strcmp(io_buff,"stop-server" ) == 0 ) {
   		//	free(io_buff);
   		//	return OK_EXIT;
   		//}
   		/*
   		if (strcmp(io_buff, "exit") == 0) {
   			send(cli_socket, "Goodbye!\n", 9, 0);
   			free(io_buff);
   			return OK_EXIT;  // Close connection
   		} else if (strcmp(io_buff, "stop-server") == 0) {
   			send(cli_socket, "Server shutting down.\n", 22, 0);
   			close(cli_socket);
   			//close(server_socket);
   			free(io_buff);
   			return STOP_SERVER_SC;
   		}*/


   		//Parse the gieven command and store in cdm
   		rc = build_cmd_list(io_buff, &clist);
   		if (rc == OK) {
   			// Redirect output to client
   			//int saved_stdout = dup(STDOUT_FILENO);
   			//int saved_stderr = dup(STDERR_FILENO);
   			//dup2(cli_socket, STDOUT_FILENO);
   			//dup2(cli_socket, STDERR_FILENO);

   			//there are no pipes, just one command
   			if (clist.num == 1) {
   				// Redirect output to client
   				int saved_stdout = dup(STDOUT_FILENO);
   				int saved_stderr = dup(STDERR_FILENO);
   				dup2(cli_socket, STDOUT_FILENO);
   				dup2(cli_socket, STDERR_FILENO);

   				rsh_exec_cmd(&clist.commands[0]);
				
				// Restore original output
				dup2(saved_stdout, STDOUT_FILENO);
				dup2(saved_stderr, STDERR_FILENO);
				close(saved_stdout);
				close(saved_stderr);	
   			}
   			
   			//there are pipes and we need to do extra stuff
   			else {
   				//execute_pipeline(&clist);
   				rsh_execute_pipeline(cli_socket,&clist);
   			}
			
			// Restore original output
			//dup2(saved_stdout, STDOUT_FILENO);
			//dup2(saved_stderr, STDERR_FILENO);
			//close(saved_stdout);
			//close(saved_stderr);

   			//close(saved_stdout);
   			//close(saved_stderr);
   			
   			// Send EOF to signal end of output
   			send_message_eof(cli_socket);
   		}
   		if (rc == STOP_SERVER_SC || rc == OK_EXIT){
   			break;
   		}
   		
   		//they gave us bad things :(
   		else if (rc == WARN_NO_CMDS) {
   			//printf(CMD_WARN_NO_CMD);
   			send(cli_socket, CMD_WARN_NO_CMD, strlen(CMD_WARN_NO_CMD), 0);
   		}
   		else if (rc == ERR_TOO_MANY_COMMANDS) {
   			//printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
   			char err_msg[128];
   			snprintf(err_msg, sizeof(err_msg), CMD_ERR_PIPE_LIMIT, CMD_MAX);
   			send(cli_socket, err_msg, strlen(err_msg), 0);
   		}
   	}
   	free(io_buff);
   	return OK;
}

/*
 * send_message_eof(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client

 *  Sends the EOF character to the client to indicate that the server is
 *  finished executing the command that it sent. 
 * 
 *  Returns:
 * 
 *      OK:  The EOF character was sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the EOF character. 
 */
int send_message_eof(int cli_socket){
   // return WARN_RDSH_NOT_IMPL;
   int bytes_sent;

   bytes_sent = send(cli_socket, &RDSH_EOF_CHAR, 1, 0);
   if (bytes_sent == 1){
		//this is what is expected, we sent a single character,
		//the EOF character, so we can return a good error code.
		//we use OK for this as defined in dshlib.h
		return OK;
	}


   return ERR_RDSH_COMMUNICATION;
}

/*
 * send_message_string(cli_socket, char *buff)
 *      cli_socket:  The server-side socket that is connected to the client
 *      buff:        A C string (aka null terminated) of a message we want
 *                   to send to the client. 
 *   
 *  Sends a message to the client.  Note this command executes both a send()
 *  to send the message and a send_message_eof() to send the EOF character to
 *  the client to indicate command execution terminated. 
 * 
 *  Returns:
 * 
 *      OK:  The message in buff followed by the EOF character was 
 *           sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the message followed by the EOF character. 
 */
int send_message_string(int cli_socket, char *buff){
   // return WARN_RDSH_NOT_IMPL;
	int len = strlen(buff);
	if (send(cli_socket, buff, len, 0) == 1) {
		return OK;
	}
	
	return ERR_RDSH_COMMUNICATION;
}

//there was only one command 
int rsh_exec_cmd(cmd_buff_t *cmd){
	if (strcmp(cmd->argv[0], "exit") == 0) {
		exit(0);
	}
	else if (strcmp(cmd->argv[0], "stop-server") == 0) {
		exit(0);
	} 

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

/*
 * rsh_execute_pipeline(int cli_sock, command_list_t *clist)
 *      cli_sock:    The server-side socket that is connected to the client
 *      clist:       The command_list_t structure that we implemented in
 *                   the last shell. 
 *   
 *  This function executes the command pipeline.  It should basically be a
 *  replica of the execute_pipeline() function from the last deliverable. 
 *  The only thing different is that you will be using the cli_sock as the
 *  main file descriptor on the first executable in the pipeline for STDIN,
 *  and the cli_sock for the file descriptor for STDOUT, and STDERR for the
 *  last executable in the pipeline.  See picture below:  
 * 
 *      
 *┌───────────┐                                                    ┌───────────┐
 *│ cli_sock  │                                                    │ cli_sock  │
 *└─────┬─────┘                                                    └────▲──▲───┘
 *      │   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐  │  │    
 *      │   │   Process 1  │     │   Process 2  │     │   Process N  │  │  │    
 *      │   │              │     │              │     │              │  │  │    
 *      └───▶stdin   stdout├─┬──▶│stdin   stdout├─┬──▶│stdin   stdout├──┘  │    
 *          │              │ │   │              │ │   │              │     │    
 *          │        stderr├─┘   │        stderr├─┘   │        stderr├─────┘    
 *          └──────────────┘     └──────────────┘     └──────────────┘   
 *                                                      WEXITSTATUS()
 *                                                      of this last
 *                                                      process to get
 *                                                      the return code
 *                                                      for this function       
 * 
 *  Returns:
 * 
 *      EXIT_CODE:  This function returns the exit code of the last command
 *                  executed in the pipeline.  If only one command is executed
 *                  that value is returned.  Remember, use the WEXITSTATUS()
 *                  macro that we discussed during our fork/exec lecture to
 *                  get this value. 
 */
int rsh_execute_pipeline(int cli_sock, command_list_t *clist) {
   // return WARN_RDSH_NOT_IMPL;
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

		if (pids[i] < 0){
			perror("Fork Error");

			return ERR_EXEC_CMD;			
		}
		if (pids[i] == 0) {
			if (i > 0) {
				dup2(pipes[i - 1][0], STDIN_FILENO);
			}
			if (i < clist->num - 1) {
				dup2(pipes[i][1], STDOUT_FILENO);
			}
			if(i == clist->num){
				dup2(cli_sock,STDIN_FILENO);
			}

			for (int j = 0; j < clist->num - 1; j++) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			//int rc = rsh_exec_cmd(clist->commands[i]);
			//if(rc!= 0 ){
			execvp(clist->commands[i].argv[0], clist->commands[i].argv);
			perror("exec failed");
			exit(ERR_EXEC_CMD);
			//}
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

/**************   OPTIONAL STUFF  ***************/
/****
 **** NOTE THAT THE FUNCTIONS BELOW ALIGN TO HOW WE CRAFTED THE SOLUTION
 **** TO SEE IF A COMMAND WAS BUILT IN OR NOT.  YOU CAN USE A DIFFERENT
 **** STRATEGY IF YOU WANT.  IF YOU CHOOSE TO DO SO PLEASE REMOVE THESE
 **** FUNCTIONS AND THE PROTOTYPES FROM rshlib.h
 **** 
 */

/*
 * rsh_match_command(const char *input)
 *      cli_socket:  The string command for a built-in command, e.g., dragon,
 *                   cd, exit-server
 *   
 *  This optional function accepts a command string as input and returns
 *  one of the enumerated values from the BuiltInCmds enum as output. For
 *  example:
 * 
 *      Input             Output
 *      exit              BI_CMD_EXIT
 *      dragon            BI_CMD_DRAGON
 * 
 *  This function is entirely optional to implement if you want to handle
 *  processing built-in commands differently in your implementation. 
 * 
 *  Returns:
 * 
 *      BI_CMD_*:   If the command is built-in returns one of the enumeration
 *                  options, for example "cd" returns BI_CMD_CD
 * 
 *      BI_NOT_BI:  If the command is not "built-in" the BI_NOT_BI value is
 *                  returned. 
 */
Built_In_Cmds rsh_match_command(const char *input)
{
    input = input;
    return BI_NOT_IMPLEMENTED;
}

/*
 * rsh_built_in_cmd(cmd_buff_t *cmd)
 *      cmd:  The cmd_buff_t of the command, remember, this is the 
 *            parsed version fo the command
 *   
 *  This optional function accepts a parsed cmd and then checks to see if
 *  the cmd is built in or not.  It calls rsh_match_command to see if the 
 *  cmd is built in or not.  Note that rsh_match_command returns BI_NOT_BI
 *  if the command is not built in. If the command is built in this function
 *  uses a switch statement to handle execution if appropriate.   
 * 
 *  Again, using this function is entirely optional if you are using a different
 *  strategy to handle built-in commands.  
 * 
 *  Returns:
 * 
 *      BI_NOT_BI:   Indicates that the cmd provided as input is not built
 *                   in so it should be sent to your fork/exec logic
 *      BI_EXECUTED: Indicates that this function handled the direct execution
 *                   of the command and there is nothing else to do, consider
 *                   it executed.  For example the cmd of "cd" gets the value of
 *                   BI_CMD_CD from rsh_match_command().  It then makes the libc
 *                   call to chdir(cmd->argv[1]); and finally returns BI_EXECUTED
 *      BI_CMD_*     Indicates that a built-in command was matched and the caller
 *                   is responsible for executing it.  For example if this function
 *                   returns BI_CMD_STOP_SVR the caller of this function is
 *                   responsible for stopping the server.  If BI_CMD_EXIT is returned
 *                   the caller is responsible for closing the client connection.
 * 
 *   AGAIN - THIS IS TOTALLY OPTIONAL IF YOU HAVE OR WANT TO HANDLE BUILT-IN
 *   COMMANDS DIFFERENTLY. 
 */
Built_In_Cmds rsh_built_in_cmd(cmd_buff_t *cmd)
{
    cmd = cmd; 
    return BI_NOT_IMPLEMENTED;
}
