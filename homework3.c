#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

#define NUM_TOKENS 99
#define TOKEN_LENGTH 100
#define COMMAND_LENGTH 100
#define NUM_ARGUMENTS 100
#define NUM_PARAMETERS 100
#define PARAMETER_LENGTH 100
#define BUFFER_LENGTH 100
#define NUM_PID 100



typedef struct PidList{
	int pid;
	struct PidList* next;


} pidList;

pidList* newNode(void){
	return calloc(1, sizeof(pidList));
}

pidList* add(pidList** head, int pid){
	pidList* node = newNode();
	node->pid = pid;
	node->next = (*head);
	(*head) = node;
}

void printPID(pidList* head){

	while(head != NULL){
		printf("%d ", head->pid);
		head = head->next;
	}

}

void delete(pidList** head, int pid_to_delete){
	pidList* tmp = *head, *previous;

	if(tmp != NULL && tmp->pid == pid_to_delete){
		*head = tmp->next;
		free(tmp);
		return;
	}

	while(tmp != NULL && tmp->pid != pid_to_delete){
		previous = tmp;
		tmp = tmp->next;
	}

	if (tmp == NULL)
		return;

	previous->next = tmp->next;

	free(tmp);


}
void freeHistory(pidList* head){
	pidList* temp;
	while(head != NULL){
		temp = head;
		head = head->next;
		free(temp);
	}
}

void test(){

	pidList* head = NULL;
	// pidList* node1 = add(head, 60);
	// pidList* node2 = add(head, 90);
	// pidList* node3 = add(head, 80);
	add(&head, 50);
	add(&head, 60);
	add(&head, 90);
	add(&head, 80);


	// free(temp);



	printPID(head);
	printf("\n");
	delete(&head, 30);
printPID(head);

freeHistory(head);
	// printf("%d\n", head->pid);

}



int byebye(){
	exit(1);
	return 0;
}
void movetodir(char* dir){

	if (chdir(dir) != 0)
		perror("chdir() to /dir");
	else
		printf("Changed directory to: %s\n", dir);

}
char* whereami( ){

	char path [100];
	if (path == NULL){
		printf("Null pointer passed into: whereami\n");
		return NULL;
	}

	printf("%s\n", getcwd(path, 100));
	return getcwd(path,100);
}
void  history(char **commands, int cmdCount){

	for(int i = 0; i < cmdCount; i++){
		// if (commands[i] == NULL)
		// 	continue;
		printf("[%d]: %s\n", i, commands[i]);
	}
}
void clearStringPtr(char** ptr, int len){

	for(int i = 0; i < len; i++){
		// printf("freeing: [%d]\n", i);
		free(ptr[i]);
	}

	// printf("freeong the pointer\n");
	free(ptr);
}
void clearCharArr(char **ptr, int len){

	for(int i = 0; i < len; i++){
		// printf("freeing: [%d]\n", i);
		free(ptr[i]);
	}

}
void clearDynArr(char* arr){
	free(arr);
}
void trimNewLine(char buffer[]){

	for(int i = 0; i < strlen(buffer); i++){
		if(buffer[i] == '\n')
			buffer[i] = ' ';
	}
}
void start(char* param[], char* program){
	pid_t pid;
	int status, died;

	pid = fork();


	if(pid == 0){
		execvp(program, param);
		exit(1);
	}else{
		// wait(NULL);

		// insert pid
		waitpid(pid, &status, 0);
	}
}
void background(char* param[], char* program){

	pid_t pid;
	int status, died;

	pid = fork();

	printf("pid: %d\n", pid);

	if(pid == 0){
		execvp(program, param);
		exit(1);
	}
	else{

		waitpid(pid, 0, WNOHANG);
		return;
	}
}
void exterminate(char* arg){
	int pid = atoi(arg);
	(kill(pid, SIGKILL) == 0) ? printf("PID: %d\tKill confirmed\n", pid) : printf("PID: %d\tAlive\n", pid);

}



void begin(){

	pidList *historyP = NULL;

	char** arguments = malloc(sizeof(char*) * NUM_ARGUMENTS);
	char** cmdHistory = malloc(sizeof(char*) * COMMAND_LENGTH);
	int pidHistory [NUM_PID];
	char* tokenized;
	char cmdBuffer [BUFFER_LENGTH];
	int numArgs = 0;
	int cmdCount = 0;
	int pid;
	int j;

	char delimiter [4] = " ";

	while(TRUE)
	{
		printf("# ");

		// Scan in command with possible arguments
		fgets(cmdBuffer,sizeof(cmdBuffer), stdin);
		trimNewLine(cmdBuffer);

		// Capturing the current command with its arguments and adding it to the command history
		cmdHistory[cmdCount] = malloc((strlen(cmdBuffer) + 1) * sizeof(char));

		strcpy(cmdHistory[cmdCount++], cmdBuffer);

		// Tokenizing the command
		tokenized = strtok(cmdBuffer, delimiter);

		while(tokenized != NULL){
			arguments[numArgs] = malloc(sizeof(char) * (strlen(tokenized) + 1));
			strcpy(arguments[numArgs++] , tokenized);
			tokenized = strtok(NULL, delimiter);
		}
		if(strcmp(arguments[0], "byebye") == 0){

			clearDynArr(tokenized);
			clearStringPtr(cmdHistory, cmdCount);
			clearStringPtr(arguments, numArgs);
			byebye();
		}
		else if(strcmp(arguments[0], "history") == 0){
			if(numArgs > 1 && strcmp(arguments[1], "-c") == 0){
				clearStringPtr(cmdHistory, cmdCount);
				cmdCount = 0;
			}else{
				history(cmdHistory, cmdCount);
			}
		}else if(strcmp(arguments[0], "whereami") == 0){
			whereami();
		}else if(strcmp(arguments[0], "movetodir") == 0){
			movetodir(arguments[1]);
		}else if((strcmp(arguments[0], "start") == 0) || (strcmp(arguments[0], "background")) == 0){

			char* function;
			function = arguments[0];

			char* program;
			int numParam = numArgs - 1;
			char* param[numParam];

			// allocating space
			for(int i = 0; i < numParam; i++)
				param[i] = malloc(sizeof(char) * 50);

			// getting the program
			program = arguments[1];

			// storing parameters from tokens
			for(int i = 1, j = 0; i < numArgs; i++, j++)
				strcpy(param[j], arguments[i]);

			// appending the NULL
			param[numArgs - 1] = '\0';

			if(strcmp(function, "start") == 0)
				start(param, program);
			else if(strcmp(function, "background") == 0)
				background(param, program);

				clearCharArr(param, numParam);
				free(function);
				free(program);

		}else if(strcmp(arguments[0], "exterminate") == 0){
			exterminate(arguments[1]);
		}else{
			printf("Not a valid command stupid\n");
		}
		numArgs = 0;
	}
}

int main(int argc, char const *argv[]) {

	// printf("before\n");
	// test();
	// printf("after\n");
	begin();

	/* MAIN-OLD
	// char cmdBuffer [BUFFER_LENGTH];
	//
	// char* tokenized;
	// char** arguments = malloc(sizeof(char*) * NUM_ARGUMENTS);
	//
	// // Stores 25 commands
	// char** cmdHistory = malloc(sizeof(char*) * COMMAND_LENGTH);
	//
	// int numArgs = 0;
	//
	// int cmdCount = 0;
	//
	// char delimiter [4] = " ";
	//
	// int j;
	//
	// int pid;
	//
	// printf("ChungusShell\n");
	//
	// while(TRUE)
	// {
	// 	waitpid(pid, 0, WNOHANG);
	//
	// 	printf("# ");
	//
	// 	// Scan in command with possible arguments
	// 	fgets(cmdBuffer,sizeof(cmdBuffer), stdin);
	// 	trimNewLine(cmdBuffer);
	//
	// 	// Capturing the current command with its arguments and adding it to the command history
	// 	cmdHistory[cmdCount] = malloc((strlen(cmdBuffer) + 1) * sizeof(char));
	//
	// 	strcpy(cmdHistory[cmdCount++], cmdBuffer);
	//
	// 	// Tokenizing the command
	// 	tokenized = strtok(cmdBuffer, delimiter);
	//
	// 	while(tokenized != NULL){
	// 		arguments[numArgs] = malloc(sizeof(char) * (strlen(tokenized) + 1));
	// 		strcpy(arguments[numArgs++] , tokenized);
	// 		tokenized = strtok(NULL, delimiter);
	// 	}
	//
	// 	if(strcmp(arguments[0], "byebye") == 0){
	//
	// 		// free everything
	// 		clearDynArr(tokenized);
	// 		// for(int i = 0; i < NUM_TOKENS; i++)
	// 			// free(tokenized[i]);
	// 		// free(tokenized);
	//
	// 		clearStringPtr(cmdHistory, cmdCount);
	// 		clearStringPtr(arguments, numArgs);
	// 		byebye();
	// 	}
	// 	else if(strcmp(arguments[0], "history") == 0){
	//
	// 		if(numArgs > 1 && strcmp(arguments[1], "-c") == 0){
	// 			// clearStringPtr(cmdHistory, cmdCount);
	// 			cmdCount = 0;
	// 		}else{
	// 			history(cmdHistory, cmdCount);
	// 		}
	// 	}else if(strcmp(arguments[0], "whereami") == 0){
	// 		whereami();
	// 	}else if(strcmp(arguments[0], "movetodir") == 0){
	// 		movetodir(arguments[1]);
	// 	}else if((strcmp(arguments[0], "start") == 0) || (strcmp(arguments[0], "background")) == 0){
	//
	// 		// printf("num args: %d\n", numArgs);
	// 		char* function;
	// 		function = arguments[0];
	// 		// pid_t pid;
	// 		// int status, died;
	// 		char* program;
	// 		int numParam = numArgs - 1;
	// 		char* param[numParam];
	//
	// 		// allocating space
	// 		for(int i = 0; i < numParam; i++)
	// 			param[i] = malloc(sizeof(char) * 50);
	//
	// 		// getting the program
	// 		program = arguments[1];
	//
	// 		// storing parameters from tokens
	// 		for(int i = 1, j = 0; i < numArgs; i++, j++)
	// 			strcpy(param[j], arguments[i]);
	//
	// 		// appending the NULL
	// 		param[numArgs - 1] = '\0';
	//
	// 		if(strcmp(function, "start") == 0)
	// 			start(param, program);
	// 		else if(strcmp(function, "background") == 0)
	// 			background(param, program);
	//
	//
	// 			clearCharArr(param, numParam);
	// 			free(function);
	// 			free(program);
	//
	// 	}else if(strcmp(arguments[0], "exterminate") == 0){
	//
	// 		 pid = atoi(arguments[1]);
	//
	// 		(kill(pid, SIGKILL) == 0) ? printf("PID: %d\tKill confirmed\n", pid) : printf("PID: %d\tAlive\n", pid);
	// 	}else{
	// 		printf("Not a valid command stupid\n");
	// 	}
	// 	numArgs = 0;
	// }

	*/
	return 0;
}
