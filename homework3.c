#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include<sys/wait.h>

#define TRUE 1
#define FALSE 0

//char * strtok(char str[], const char *delims);


/*

	# cd brenden

	argsToken

	argsTokenized = [cd brenden]

*/


int byebye(){
	exit(1);

	return 1;
}

void movetodir(char* dir){

	if (chdir(dir) != 0)
		perror("chdir() to /dir");
	else
		printf("Changed directory to: %s\n", dir);	

}

char*  whereami( ){

	char path [100];
	if (path == NULL){
		printf("Null pointer passed into: whereami\n");
		return NULL;
	}

	printf("%s\n", getcwd(path, 100));
	return getcwd(path,100);
}

void  history(char **commands){

	for(int i = 0; i < 10; i++){
		
		if (commands[i] == NULL)
			continue;	
		printf("[%d]: %s\n", i, commands[i]);
		
	}

}

void clearStringPtr(char** ptr, int len){

	for(int i = 0; i < len; i++)
		free(ptr[i]);

	free(ptr);

	if (ptr == NULL)
		printf("Ptr freed\n");
	else
		printf("Ptr not freed\n");


}

void trimNewLine(char buffer[]){

//	printf("strlen(%s): %ld\n",buffer,  strlen(buffer));
	for(int i = 0; i < strlen(buffer); i++){	
		if(buffer[i] == '\n')
			buffer[i] = ' ';
	}
}


/*void start(char *[] param){



}

void background(char *[] param){




}
 */






int main(int argc, char const *argv[]) {

	char cmdBuffer [100];
	char* tokenized = malloc(sizeof(char*) * 50);

	char **arguments = malloc(sizeof(char**) * 50);	
	int numArgs = 0;
	int cmdCount = 0;
	char delimiter [4] = " ";
	int j;
	int pid;


	

	// Stores 25 commands
	char** cmdHistory = malloc(sizeof(char**) * 30);

	char newLine [2];
	strcpy(newLine, "\n");

	printf("ChungusShell\n");
	while(TRUE)
	{

		printf("# ");

		fgets( cmdBuffer,sizeof(cmdBuffer), stdin);
	
		trimNewLine(cmdBuffer);	

/*		for(int i = 0; i < strlen(cmdBuffer); i++){
			printf("%d\n",cmdBuffer[i]);
		}
*/
//		gets(cmdBuffer);


		// stores commands of length 50 characters
		cmdHistory[cmdCount] = malloc(strlen(cmdBuffer)*sizeof(char*));

		strcpy(cmdHistory[cmdCount++], cmdBuffer);

//		printf("cmdHitsory[%d] = %s\n", cmdCount, cmdHistory[cmdCount - 1]);

		tokenized = strtok(cmdBuffer, delimiter);

//		for(int i = 0;i < 100; i++)
//			 printf("before tokenized: %s\n",tokenized );

		while(tokenized != NULL){
			arguments[numArgs] = malloc(sizeof(char*) * (strlen(tokenized) + 1));
//			arguments[numArgs] = malloc(sizeof(char*) * 50);
//			arguments[numArgs++] = tokenized;
//			 printf("after allocating space: %s\n",tokenized );
			strcpy(arguments[numArgs++] , tokenized);
//			printf("After strcpy: %s\n", arguments[numArgs - 1]);	
			arguments[numArgs] = malloc(sizeof(char*) * (strlen(tokenized) + 1));

			tokenized = strtok(NULL, delimiter);
			if(tokenized == NULL)
				break;
			strcpy(arguments[numArgs], tokenized);
//			printf("After tokenized with NULL: %s\n", tokenized);
		}
		
		printf("numArgs: %d\n", numArgs);

		for(int i = 0; i < (numArgs); i++)
			printf("arguments[%d] = %s\n", i, arguments[i]);

/*		for(int i = 0; i < (cmdCount); i++)
			printf("[%d]: %s\n", i, cmdHistory[i]);
*/

		
		if(strcmp(arguments[0], "byebye") == 0)
			byebye();
		else if(strcmp(arguments[0], "history") == 0){

			if(strcmp(arguments[1], "-c") == 0){
				printf("\n\nClearing history\n\n");
				clearStringPtr(cmdHistory, cmdCount);
				cmdCount = 0;
			}
		
//			history(cmdHistory);
			
			for(int i = 0; i < (cmdCount); i++)
				printf("[%d]: %s\n", i, cmdHistory[i]);
			
		}else if(strcmp(arguments[0], "whereami") == 0){

			whereami();
		}else if(strcmp(arguments[0], "movetodir") == 0){

			movetodir(arguments[1]);

		}else if((strcmp(arguments[0], "start") == 0)){
			
		
			pid_t pid;
			int status, died;
			
			char* param[50];
			char* program;
			int numParam = numArgs - 1;

			// allocating space
			for(int i = 0; i < 50; i++)
				param[i] = malloc(sizeof(char*) * 100);

			// getting the program
			program = arguments[1];

			printf("program: %s\n", program);


			printf("bennenene\n");

			// storing parameters from tokens
			for(int i = 1, j = 0; i < numArgs; i++, j++)
			{
			//	printf("i: %d\tj: %d\n", i ,j);
				strcpy(param[j], arguments[i]);
			}

			// appending the NULL
			param[numArgs - 1] = '\0';

			for(int i =0; i < (numArgs); i++)
				printf("param[%d]: %s\n", i, param[i]);
			

			pid = fork();

			if(pid == 0){
				printf("Program exec: %s\n", program);
				printf("execv...\n");
				execvp(program, param);
				printf("after execv\n");
			}
			else if(pid == -1){
				printf("pid == -1\n");
				printf("can't fork\n");
				exit(1);
			}else{

				printf("died\n");
				died = waitpid(pid, &status, 0);

			}

			


		/*	if(arguments[1][0] == '/'){
				// Full path

				// Start program
				// start(program, parameters[]);

				



			}else{

				
			// program = arguments[1];

			
			//	Any arugments[i] where i > 2, are the parameters for the program
			 
			
			}
			*/
	
			
			
		}else if(strcmp(arguments[0], "background") == 0){

			pid_t pid;
			int status, died;
			
			char* param[50];
			char* program;
			int numParam = numArgs - 1;

			// allocating space
			for(int i = 0; i < 50; i++)
				param[i] = malloc(sizeof(char*) * 100);

			// getting the program
			program = arguments[1];

			//printf("program: %s\n", program);


			//printf("bennenene\n");

			// storing parameters from tokens
			for(int i = 1, j = 0; i < numArgs; i++, j++)
			{
			//	printf("i: %d\tj: %d\n", i ,j);
				strcpy(param[j], arguments[i]);
			}

			// appending the NULL
			param[numArgs - 1] = '\0';

			for(int i =0; i < (numArgs); i++)
				printf("param[%d]: %s\n", i, param[i]);
			

			pid = fork();

			printf("pid: %d\n", pid);

			if(pid == 0){
				printf("Program exec: %s\n", program);
				printf("execv...\n");
				execvp(program, param);
				printf("after execv\n");
			}
			else if(pid == -1){
				printf("pid == -1\n");
				printf("can't fork\n");
				exit(1);
			}else{

				printf("died\n");
				died = waitpid(pid, &status, WCONTINUED);

				continue;

			}



		/*	if(arguments[1][0] == '/'){
				// Full path


			}else{
				// program = arguments[1];


			
			 //	Any arugments[i] where i > 2, are the parameters for the program
			 
			}
			*/

		}else if(strcmp(arguments[0], "exterminate") == 0){

			 pid = atoi(arguments[1]);
			// Kill process
			// Print :success or failure
			
			(kill(pid, SIGQUIT) == 0) ? printf("PID: %d\tKill confirmed\n", pid) : printf("PID: %d\tAlive\n", pid);
			
			

		}


		// Resetting some things
		
		// Clear arguments

//		clearStringPtr(char** ptr, int len)
		
		printf("numArgs: %d\n", numArgs);
		clearStringPtr(arguments, numArgs);

		//for(int i = 0; i < numArgs; i++)
		//	free(arguments[i]);
	//	free(arguments);

		numArgs = 0;

	}

	return 0;
}














