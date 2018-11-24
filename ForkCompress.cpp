/*********************************************************************

Viraj, Tanner, and Deanna

Project 1 #3 - Fall 2018

This program forks a child process and executes the program MyCompress, 
compressing the compress.txt file into compressed.txt.

**********************************************************************/


#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	
	//create child process
	pid_t pid;
	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Failure with fork() command");
		return 1;
	} else if (pid == 0) {
		//child process
		execl("./MyCompress", "./MyCompress", "compress.txt", "compressed.txt", (char*) NULL);
	} else {
		//parent process
		wait(NULL);
		printf("Child Complete\n");
	}
	
	return 0;
}
