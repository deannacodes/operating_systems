/*********************************************************************

Viraj, Tanner, and Deanna

Project 1 #9 - Fall 2018

This program creates n threads. It divides a file into n pieces and 
compresses each piece concurrently. It prints the processing time 
in ms to the console.

**********************************************************************/


#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <pthread.h>  
#include <ctime>


std::string compress_me(std::string to_compress);
int check_repeates(std::string input, int index);
void* runCompress(void *arg);

struct argStruct {
	FILE *fileL;
	FILE *fileS;
	int filePiece;
};


int main(int argc, char **argv) {


	if (argc != 4) {
		printf("Wrong arguments: file-to-compress.txt file-to-copy-to.txt #-of-forks\n");
		return 1;
	}

	//start timing
	clock_t start = clock();



	//prepare files and n processes
	FILE *fileLarge = fopen(argv[1], "r");
	FILE *fileSmall = fopen(argv[2], "r+");
	int divider = atoi(argv[3]);

	if (!fileLarge | !fileSmall) {
		printf("One or more files were not found. Please try again.\n");
		return 1;
	}



	
	fseek(fileLarge, 0L, SEEK_END);
	long fileLargeSize = (ftell(fileLarge) - 1);
	fseek(fileLarge, 0, SEEK_SET);	
	long filePiece = (fileLargeSize / divider);
	
	long remainder = filePiece - ((filePiece * divider) - fileLargeSize);




	struct argStruct args;
	args.fileL = fileLarge;
	args.fileS = fileSmall;
	args.filePiece = filePiece;
	


	
	pthread_t tid;
	pthread_attr_t attr;





	for ( int i = 0 ; i < divider ; i++ ) {
		if (i == (divider - 1)) {
			args.filePiece = remainder;
		}

		
		//create the thread and compress the input
		pthread_attr_init(&attr);
		pthread_create(&tid, &attr, &runCompress, (void*)&args);
 		


		//close thread
		pthread_join(tid, NULL);
		printf("Thread Complete\n");

	}



	//display time to run
	clock_t stop = clock();
        double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        printf("Time elapsed in ms: %f\n", elapsed);

	return 0;

}




//takes a struct of type arg with command line arguments provided. 
//Compresses a piece of the file provided and places it into another
//file.
void *runCompress(void* arg) {
	
	struct argStruct *args = (struct argStruct *)arg;

	char input[(args -> filePiece) + 1] = "";			
	fread(input, sizeof(char), args -> filePiece, args -> fileL);
	
	std::string compressedInput = compress_me(input);
	char* charCompressedInput = new char[compressedInput.length() + 1];	
	strcpy(charCompressedInput, compressedInput.c_str());
	fputs(charCompressedInput, args -> fileS);	

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
