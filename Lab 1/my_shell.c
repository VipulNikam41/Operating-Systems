#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

int bg_processes = 0;
int parallel_processes = 0;

pid_t process_pid[64];
pid_t bg_pid[64];

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line) 
{
	char **tokens = (char **)malloc(64 * sizeof(char *));
	char *token = (char *)malloc(64 * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	i =0; 
	while(i < strlen(line)) 
	{

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t') 
		{
			token[tokenIndex] = '\0';
			if (tokenIndex != 0) 
			{
				tokens[tokenNo] = (char*)malloc(64*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
			}
		}
		else 
		{
			token[tokenIndex++] = readChar;
		}


		i++;
	}

	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}
int t_p=0;

void backgroundHandler()
{
	int wstat;
	pid_t pid;
	if (bg_processes != 0)
	{
		pid = wait3(&wstat, WNOHANG, (struct rusage *)NULL);

		size_t i = 0; 
		while(i < bg_processes)
		{
			if (bg_pid[i] == pid)
			{  
				if(t_p==1){
					printf("Shell: Background process finished \n");
				}
				
				t_p++;
				bg_processes--;
				bg_pid[i] = bg_pid[bg_processes];
				break;
			}

			i++;
		}
	}
}

int execute(char **commands, int bg, int parallel, int argCount) 
{
	char *comm_args[argCount+1];
	int status;
	int i=0;
	while(i < argCount) 
	{
		comm_args[i] = commands[i];
		// printf("%s ", comm_args[i]);
		i++;
	}
	comm_args[argCount] = NULL;
	// printf("%s", comm_args[0]);

	int pid = fork();
	if(pid == 0) {
		signal(SIGINT, SIG_DFL);
		// setpgrp();

		if (execvp(comm_args[0], comm_args) < 0) {
			printf("Shell: Incorrect Command \n");
			exit(EXIT_FAILURE);
		};
	} else if(pid) {
		// setpgid(pid, pid);
		if (parallel == 1) { // if parallel
			process_pid[parallel_processes] = pid;
			parallel_processes++;
		}
		else if (bg == 1)
		{ // if background
			bg_pid[bg_processes] = pid;
			bg_processes++;
		}
		else
		{ // if blocking, reap it before going ahead
			waitpid(pid, &status, 0);
			// wait(NULL);
		}
	}
}


int main(int argc, char* argv[]) {
	signal(SIGINT, SIG_IGN);
	signal(SIGCHLD, backgroundHandler);

	char  line[1024];            
	char  **tokens;              
	int i;

	FILE* fp;
	if(argc == 2) 
	{
		fp = fopen(argv[1],"r");
		if(fp < 0) 
		{
			printf("File doesn't exists.");
			return -1;
		}
	}

	while(1) 
	{			
		/* BEGIN: TAKING INPUT */
		parallel_processes = 0;

		bzero(line, sizeof(line));
		if(argc == 2) 
		{ // batch mode
			if(fgets(line, sizeof(line), fp) == NULL) 
			{ // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} 
		else 
		{ // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		// printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		char *linuxCommand[100];
		int argCount = 0;
		int bg = 0, parallel = 0;
   
        // do whatever you want with the commands, here we just print them

		for(i=0; tokens[i] != NULL; i++){
			if(strcmp(tokens[i], "cd") == 0) 
			{
				chdir(tokens[++i]);
				continue;
			}
			else if(strcmp(tokens[i], "&") == 0) 
			{
				bg = 1;
				execute(linuxCommand, bg, parallel, argCount);
				argCount = 0;
			}
			else if(strcmp(tokens[i], "&&") == 0) 
			{
				execute(linuxCommand, bg, parallel, argCount);
				argCount = 0;
			}
			else if(strcmp(tokens[i], "&&&") == 0) 
			{
				parallel = 1;
				execute(linuxCommand, bg, parallel, argCount);
				argCount = 0;
			}
			else if(strcmp(tokens[i], "exit") == 0) 
			{
				// killProcesses==>process_pid / parallel_processes
				for (int it = 0; it < parallel_processes; it++)
				{
					kill(process_pid[i], SIGKILL);
					wait(0);
				}
				// killProcesses==>bg_pid / bg_processes
				for (int it = 0; it < bg_processes; it++)
				{
					kill(bg_pid[i], SIGKILL);
					wait(0);
				}
				printf("Shell: Goodbye. \n");
				exit(EXIT_SUCCESS);
				break;
			}
			else {
				linuxCommand[argCount] = tokens[i];
				argCount++;
			}
			// printf("found token %s (remove this debug output later)\n", tokens[i]);
		}
		
		execute(linuxCommand, bg, parallel, argCount);
       
	   	size_t i = 0; 
	   	while(i < parallel_processes) 
	   	{
			int status;
			pid_t wpid = waitpid(process_pid[i], &status, 0);
			parallel_processes--;
			process_pid[i] = 0;

			i++;
		}
		waitpid(-1, 0, WNOHANG);

		// Freeing the allocated memory	
		i=0; 
		while(tokens[i]!=NULL)
		{
			free(tokens[i]);
			i++;
		}
		free(tokens);
	}
	return 0;
}
