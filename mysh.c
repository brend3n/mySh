#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

#define NUM_TOKENS 100
#define TOKEN_LENGTH 100
#define COMMAND_LENGTH 100
#define NUM_ARGUMENTS 100
#define NUM_PARAMETERS 100
#define PARAMETER_LENGTH 100
#define BUFFER_LENGTH 100
#define NUM_PID 100

int sickCHILD = -1;

void exterminate(int pid);

// Linked list to store the process IDs (PIDs) for each process that runs in the background
typedef struct PID{
	// Process ID
	int pid;
	// Link to next node
	struct PID* next;
}pidList;

typedef struct CMD{

	char* cmd_str;

	struct CMD* next;
}cmdList;

pidList** parent = NULL;

// Returns a new pidList object
pidList* newNode_pid(){
		return calloc(1, sizeof(pidList));
}

// Inserts a new node to the head of the linked list
void add_pid(pidList** head, int pid){

	// Create node
	pidList *node = newNode_pid();

	// if(*head == NULL)
	// {
	// 	node = newNode_pid();
	// 	node->pid = pid;
	// 	node->next = NULL;
	// }
		// Initialize the node's fields
		node->pid = pid;
		// Link this node's next pointer to the head
		node->next = (*head);

		// This node is the new head
		(*head) = node;

}

// Traverses the linked list and prints all active PIDs
void print_pid(pidList* head){

	if(head == NULL)
		return;

	while(head != NULL){
		printf("%d\n", head->pid);
		head = head->next;
	}

}

// Removes the node from the linked list specified by its PID and kills the process.
// Returns if the PID is not found.
void delete_pid(pidList** head, int pid_to_delete){

	// Pointers used to relink nodes to ensure proper deletion of a node
	pidList* tmp = *head, *previous;

	// Found PID key at the head node
	if(tmp != NULL && tmp->pid == pid_to_delete){

		// The new head should be the tmp's next pointer
		*head = tmp->next;

		// Kill the process
		exterminate(pid_to_delete);
		// Free allocated space

		free(tmp);
		// printf("\n");
		return;
	}

	// Search for the node with the PID to delete
	while(tmp != NULL && tmp->pid != pid_to_delete){
		previous = tmp;
		tmp = tmp->next;
	}

	// No node with PID found
	if (tmp == NULL)
		return;

	// Link previous node to the node after the deleted node
	previous->next = tmp->next;

	// Kill the process
	exterminate(pid_to_delete);

	// Free allocated space
	free(tmp);

}

// Destroys the linked list
void freeHistory_pid(pidList* head){

	if (head == NULL)
		return;

	pidList* temp;
	while(head != NULL){
		temp = head;
		head = head->next;
		free(temp);
	}

}

// Returns a new pidList object
cmdList* newNode_cmd(){
		return calloc(1, sizeof(cmdList));
}

char* new_cmd_str(int len){
	return malloc((len + 1) * sizeof(char));
}

// Inserts a new node to the head of the linked list
void add_cmd(cmdList** head, char* cmd_str){

	// Create node
	cmdList *node = newNode_cmd();

	// Initialize the node's fields
	// node->cmd_str = malloc((strlen(cmd_str) + 1) * sizeof(char));
	node->cmd_str = new_cmd_str(strlen(cmd_str));
	strcpy(node->cmd_str, cmd_str);

	// Link this node's next pointer to the head
	node->next = (*head);

	// This node is the new head
	(*head) = node;

}
// Traverses the linked list and prints all active PIDs
void print_cmd(cmdList* head){

	int i = 0;
	if(head == NULL)
		return;

	while(head != NULL){
		printf("[%d]: %s\n",i++ ,head->cmd_str);
		head = head->next;
	}
}
// Destroys the linked list
void freeHistory_cmd(cmdList* head){

	if(head == NULL)
		return;

	cmdList* temp;

	while(head != NULL){
		temp = head;
		head = head->next;
		free(temp->cmd_str);
		free(temp);
	}

}


// Kills a single process specified by its PID
void exterminate(int pid){
	(kill(pid, SIGKILL) == 0) ? printf("PID: %d\tKill confirmed\n", pid) : printf("PID: %d\tAlive\n", pid);
}

// Kills are processes.
void exterminateAll(pidList** head){
	pidList* tmp = *head;
	pidList* next;

	if(tmp == NULL){
		printf("No one alive\n");
		return;
	}

	printf("Murdering: ");

	while(tmp != NULL){
		// Capture the next node
		next = tmp->next;
		printf("%d ", tmp->pid);
		// Kill the process that corresponds to the head node
		kill(tmp->pid, SIGKILL);
		// Remove the head from the list
		free(tmp);
		// Relink the head
		tmp = next;
	}

	// Destroy head reference
	*head = NULL;
	printf("\n");
	return;
}

// Runs a single instance of a program in the foreground
void start(char* param[], char* program){
	pid_t pid;
	int status;

	// Forks a child process from the parent process
	pid = fork();

	// New process runs in the child process
	if(pid == 0){
		execvp(program, param);
		exit(1);
	}

	waitpid(pid, 0, WUNTRACED);
}

void kill_child(){
	delete_pid(parent, sickCHILD);
}

void handler(){

	printf("sickCHILD: %d\n", sickCHILD);
	if(sickCHILD == -1)
		return;
	sickCHILD = wait(NULL);

	kill_child(parent, sickCHILD);
	sickCHILD = 0;
	parent = NULL;

}

void background(char* param[], char* program, pidList** head){
	signal(SIGCHLD, handler);
	parent = &(*head);
	int pid;
	printf("Parent: %d\n", getpid());
	pid = fork();
	if(pid == 0){
		execvp(program, param);
		exit(1);
	}

	add_pid(head, pid);
}




// Prints and returns the file path of the current directory
char* whereami(){

	char path [100];

	printf("%s\n", getcwd(path, 100));
	return getcwd(path,100);
}

// Terminates the program
int byebye(){
	exit(1);
	return 0;
}

// Changes directories to the specifed directory if there is one
void movetodir(char* dir){

	if (chdir(dir) != 0)
		perror("chdir() to /dir");
	else
		printf("Changed directory to: %s\n", dir);

}




// Runs a single instance of a program in the background
// void background(char* param[], char* program, pidList** head){
//
// 	pid_t pid;
// 	int status;
// 	int wtf;
//
// 	// printf("%d\n", getpid());
// 	// Forks a child process from the parent process
// 	pid = fork();
//
// 	printf("pid: %d\n", pid);
//
//
// 	if(pid == 0){
// 		execvp(program, param);
// 		exit(1);
// 	}
// 	// else{
// 		add(head, pid);
// 		status = waitpid(pid, 0, WNOHANG);
// 		// delete(head, status);
// 		return;
// 	// }
// 	// if(pid == 0){
// 	// 	// add(head, pid);
// 	// 	wtf = fork();
// 	// 	if(wtf == 0){
// 	// 		add(head, pid);
// 	// 		execvp(program, param);
// 	// 	}
// 	// 	exit(1);
// 	// }
//
// 		// add(head, pid);
// 		// add(head, wtf);
// 		// status = waitpid(pid, 0, WNOHANG);
// }

// // Deallocates memory in a 2D char array
// void clearStringPtr(char** ptr, int len){
//
// 	for(int i = 0; i <= len; i++){
// 		// printf("freeing: %s\n", ptr[i]);
// 		free(ptr[i]);
// 	}
//
// 	free(ptr);
// }

// Deallocates memory in a array of char pointers
void clearCharArr(char* ptr[], int len){

	for(int i = 0; i < len; i++){
		free(ptr[i]);
	}
}

// Replaces the newline character with a space
void replaceNewLine(char buffer[]){

	int len = strlen(buffer);
	for (int i = 0; i < len; i++){
		if(buffer[i] == '\n')
			buffer[i] = ' ';
	}

}

void begin(){

	pidList* pid_history = NULL;
	// argList* arg_history = NULL;
	cmdList* cmd_history = NULL;
	// history *historyP = NULL;
	// char** arguments = malloc(sizeof(char*) * NUM_ARGUMENTS);
	char* arguments[1024];
	// char** cmdHistory = malloc(sizeof(char*) * COMMAND_LENGTH);
	char* tokenized;
	// int pidHistory [NUM_PID];
	char cmdBuffer [BUFFER_LENGTH];
	int numArgs = 0;
	int total_args = 0;
	int cmdCount = 0;
	int pid;
	// int j;

	char delimiter [4] = " ";


	// Shell loop
	while(TRUE)
	{
		printf("# ");

		// Scan in command with possible arguments
		fgets(cmdBuffer,sizeof(cmdBuffer), stdin);



		replaceNewLine(cmdBuffer);

		// Allocating space for the command history
		// cmdHistory[cmdCount] = malloc((strlen(cmdBuffer) + 1) * sizeof(char));
		add_cmd(&cmd_history, cmdBuffer);
		// print_cmd(cmd_history);

		// cmd_history->cmd_str = calloc((strlen(cmdBuffer) + 1)	, sizeof(char));
		// cmdHistory[cmdCount] = calloc((strlen(cmdBuffer) + 1), sizeof(char));

		// Adding command to command history
		// strcpy(cmdHistory[cmdCount++], cmdBuffer);
		// strcpy(cmd_history->cmd_str, cmdBuffer);

		// Tokenizing the command
		tokenized = strtok(cmdBuffer, delimiter);
		// printf("Tokenized len: %ld", strlen(tokenized));
		// printf("cmdBuffer len: %ld", strlen(cmdBuffer));

		// Storing the tokens
		while(tokenized != NULL){
			// printf("<Beginning of loop> Tokenized: '%s'\n", tokenized);
				// Allocate space for the argument
				// arguments[numArgs] = malloc(sizeof(char) * (strlen(tokenized)+1));
				arguments[numArgs] = calloc((strlen(tokenized)+1), sizeof(char));

				// Store the argument
				strcpy(arguments[numArgs++] , tokenized);
				// printf("arg[%d]: %s", numArgs - 1, arguments[numArgs-1]);

				// Go to the next token
				tokenized = strtok('\0', delimiter);
				// tokenized = strtok(cmdBuffer, delimiter);
				// printf("arg[%d]: %s", numArgs - 1, arguments[numArgs-1]);
				// printf("<End of loop> Tokenized: '%s'\n", tokenized);

			}
			// for(int i = 0; i < numArgs; i++)
				// printf("arguments[%d]: %s\n", i , arguments[i]);


		// for(int i = 0; i <= numArgs; i++)
		// 	printf("poop: %s\n", arguments[i]);
		if(strcmp(arguments[0], "byebye") == 0){

			// for(int i = 0; i < numArgs; i++)
			// 	printf("arguments[%d]: %s\n", i , arguments[i]);
			free(tokenized);
			// printf("\narugments: ");
			// clearStringPtr(arguments, numArgs);
			// printf("Clearing arugmnets\n");
			clearCharArr(arguments, 1);
			freeHistory_cmd(cmd_history);
			freeHistory_pid(pid_history);
			// printf("\ncmdHistory: ");
			// clearStringPtr(cmdHistory, cmdCount);
			byebye();
		}
		else if(strcmp(arguments[0], "history") == 0){
			if(numArgs > 1 && strcmp(arguments[1], "-c") == 0){
				// clearStringPtr(cmdHistory, cmdCount);
				// printf("clearing the history\n");
				freeHistory_cmd(cmd_history);
				cmd_history = NULL;
				// cmdCount = 0;
			}else if(numArgs == 1){
				// history(cmdHistory, cmdCount);
				// printf("Printing the history\n");
				print_cmd(cmd_history);
				// printf("after printing\n");
			}
		}else if(strcmp(arguments[0], "whereami") == 0){
			whereami();
		}else if(strcmp(arguments[0], "movetodir") == 0){
			movetodir(arguments[1]);
		}else if(((strcmp(arguments[0], "start") == 0) || (strcmp(arguments[0], "background")) == 0) && numArgs > 1){

			char* param[numArgs - 1];

			// allocating space
			for(int i = 0; i < (numArgs - 1); i++)
				param[i] = malloc(sizeof(char) * 100);

			// storing parameters from tokens
			for(int i = 1, j = 0; i < numArgs; i++, j++)
				strcpy(param[j], arguments[i]);

			// appending the NULL charcater for execvp
			param[numArgs - 1] = '\0';

			if(strcmp(arguments[0], "start") == 0)
				start(param,arguments[1] );
			else if(strcmp(arguments[0], "background") == 0)
				// background(param, arguments[1], &historyP);
				background(param, arguments[1], &pid_history);

				clearCharArr(param, (numArgs));
		}else if(strcmp(arguments[0], "exterminate") == 0){
			// delete(&historyP, atoi(arguments[1]));
			printf("argumebts[1]: %d", atoi(arguments[1]));
			delete_pid(&pid_history, atoi(arguments[1]));
		}else if(strcmp(arguments[0], "pidhistory") == 0) {
			print_pid(pid_history);
		}else if(strcmp(arguments[0], "exterminateall") == 0) {
			exterminateAll(&pid_history);
		}else{
			printf("Not a valid command\n");
		}
		clearCharArr(arguments, numArgs);
		numArgs = 0;
	}

}

int main(int argc, char const *argv[]) {
	begin();
	return 0;
}
