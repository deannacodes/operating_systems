/*********************************************************************

Viraj, Tanner, and Deanna

Project 1 #4 - Fall 2018

This program forks two processes. One process reads a file and passes
it to the other, who writes it to a file.

**********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fstream>  


std::string compress_me(std::string to_compress);
int check_repeates(std::string input, int index);


int main(int argc, char **argv) {
	
	//prepare files from arguments
	FILE *fileLarge = fopen(argv[1], "r");
	FILE *fileSmall = fopen(argv[2], "r+");

	if (!fileLarge | !fileSmall) {
		printf("One or more files were not found. Please try again.\n");
		return 1;
	}



	fseek(fileLarge, 0L, SEEK_END);
	int fileLargeSize = (ftell(fileLarge) * sizeof(char)) + 1 ;
	fseek(fileLarge, 0, SEEK_SET);


	
	//create pipe
	int fd[2];
	if (pipe(fd) == -1) {
		exit(1);
	}



	//fork the process
	pid_t pid;
	pid = fork();



	if (pid < 0) {

		fprintf(stderr, "Failure with fork() command");
		return 1;
	


	} else if (pid == 0) {
		
		close(fd[0]);

		//read the process to the pipe
		char input[fileLargeSize];
		fscanf(fileLarge, "%[^\n]", input);
		write(fd[1], input, strlen(input) + 1 );
	
		close(fd[1]);



	} else {

		close(fd[1]);
		
		//get from pipe
		char readInput[fileLargeSize];
		read(fd[0], readInput, fileLargeSize);

		close(fd[0]);

		//compress file
		std::string compressedInput = compress_me(readInput);
		char* charCompressedInput = new char[compressedInput.length() + 1];	
		strcpy(charCompressedInput, compressedInput.c_str());

		//write to file
		fputs(charCompressedInput, fileSmall);

		wait(NULL);

		printf("Child Complete\n");
	}
	

	return 0;

}





//takes a string in put and compresses it. returns the compressed string
std::string compress_me(std::string to_compress){
	int temp = 0;
	int i = 0;

	std::string new_string = "";
	std::string temp_nums = "";

	while(to_compress[i] != '\0'){
		temp = check_repeates(to_compress, i);
		temp_nums[0] = '\0';

		temp_nums = std::to_string(temp);

		if(to_compress[i] == '1'){
			if(temp >= 16){
			    new_string = new_string + "+" + temp_nums + "+";
			}else {
				for(int j = 0; j < temp; j++){
					new_string.append("1");
				}
			}

		}else {
			if(temp >= 16){
			    new_string = new_string + "-" + temp_nums + "-";
			}else {
				for(int j = 0; j < temp; j++){
					new_string.append("0");
				}
			}

		}
		i = i + temp;
	}
	return new_string;
}





//counts the amount of repeating digits
int check_repeates(std::string input, int index){
	char test = input[index];
	char type = input[index];
	int marker = index;
	int count = 0;

	while(test == type){
		marker += 1;
		count += 1;
		test = input[marker];
	}
	return count;
}
