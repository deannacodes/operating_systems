/*********************************************************************

Viraj, Tanner, and Deanna

Project 2 #3 - Fall 2018

This program simulates a queue at an airline. Threads for passengers 
in the queue, baggage handlers, security screeners, and flight attendants 
all run simultaneously. Takes arguments:[# passengers] [# baggage handlers] 
[# security screeners] [# flight attendants]

**********************************************************************/


#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <pthread.h> 
#include <semaphore.h>
#include <vector>


sem_t semBag, semScreen, semFlight, semPass; //semaphores
pthread_mutex_t lockPassengers, baggers, screeners, attendants; //mutexes
int stepsLeft[4]; //keeps track of how many passengers are left for each step

//this'll store the passenger IDs waiting at each step
struct argStruct {
	std::vector<int> bagsWaiting;
	std::vector<int> screenWaiting;
	std::vector<int> flightWaiting;
	int passengerLimit;
};

//takes a person waiting for baggage and sends them to screening phase
void* checkBaggage(void *arg);

//takes a person waiting for screening and sends them to seating phase
void* screenPassenger(void *arg);

//takes a person waiting for seating and seats them
void* seatPassenger(void *arg);

//queues up a passenger to be ready for baggage
void* passengerQueue(void *arg);


int main(int argc, char **argv) {

	//verify and store commandline args
	if (argc != 5) {
		printf("Wrong arguments: [passengers] [baggage handlers] [security screeners] [flight attendants]\n");
		return 1;
	}


	int bagHandlers = atoi(argv[2]);
	int secScreeners = atoi(argv[3]);
	int flightAttendants = atoi(argv[4]);
	int passengers = atoi(argv[1]);
	
	for ( int i = 0 ; i < 4 ; i++) {
		stepsLeft[i] = passengers;
	}



	sem_init(&semBag, 0, 0);
	sem_init(&semScreen, 0, 0);
	sem_init(&semFlight, 0, 0);
	sem_init(&semPass, 0, 1);

	pthread_mutex_init(&baggers, NULL);
	pthread_mutex_init(&screeners, NULL);
	pthread_mutex_init(&attendants, NULL);
	pthread_mutex_init(&lockPassengers, NULL);


	struct argStruct args;
	args.passengerLimit = passengers;

	
	pthread_t p;

	
	//create threads for each passenger 
	for ( int i = 0 ; i < passengers ; i++ ) {

		pthread_create(&p, NULL, &passengerQueue, &args);
	}

	

	//create threads for each bag handler
	for ( int i = 0 ; i < bagHandlers ; i++ ) {

		pthread_create(&p, NULL, &checkBaggage, &args);	

	}



	//create threads for each screener
	for ( int i = 0 ; i < secScreeners ; i++ ) {

		pthread_create(&p, NULL, &screenPassenger, &args);
	}



	
	//create threads for each flight attendant
	for ( int i = 0 ; i < flightAttendants ; i++ ) {

		pthread_create(&p, NULL, &seatPassenger, &args);
	}
	
	
	

	//join all threads
	for ( int i = 0 ; i < (bagHandlers + secScreeners + flightAttendants) ; i++) {

		pthread_join(p, NULL);

	}

	

	return 0;

}





void *checkBaggage(void *arg) {

	struct argStruct *args = (struct argStruct *)arg;	

	while (true) {
		pthread_mutex_lock(&baggers);
		
		if (stepsLeft[1] > 0) {		
			sem_wait(&semBag);
			sem_post(&semPass);


				stepsLeft[1]--;
				args -> screenWaiting.push_back(args -> bagsWaiting.front());
				args -> bagsWaiting.erase(args -> bagsWaiting.begin());
				
				printf("Passenger #%i is waiting to be screened by a screener.\n", args -> screenWaiting.back());

			pthread_mutex_unlock(&baggers);
			sem_post(&semScreen);
		} else { 
			pthread_mutex_unlock(&baggers);
			break; 
		}	

	}


}





void *screenPassenger(void *arg) {
	struct argStruct *args = (struct argStruct *)arg;
	
	while (true) {
		pthread_mutex_lock(&screeners);
		if (stepsLeft[2] > 0) {			

			sem_wait(&semScreen);		

				stepsLeft[2]--;			
				args -> flightWaiting.push_back(args -> screenWaiting.front());
				args -> screenWaiting.erase(args -> screenWaiting.begin());

				printf("Passenger #%i is waiting to board the plane by an attendant.\n", args -> flightWaiting.back());
			
			pthread_mutex_unlock(&screeners);
			sem_post(&semFlight);

		} else { 
			pthread_mutex_unlock(&screeners);
			break; 
		}

	}

}




void *seatPassenger(void *arg) {
	struct argStruct *args = (struct argStruct *)arg;

	while (true) {
		
		pthread_mutex_lock(&attendants);

		if (stepsLeft[3] > 0) {	

			sem_wait(&semFlight);				

				stepsLeft[3]--;
				printf("Passenger #%i has been seated and relaxes.\n", args -> flightWaiting.front());
				
				args -> flightWaiting.erase(args -> flightWaiting.begin());
			
			pthread_mutex_unlock(&attendants);

		} else { 
			pthread_mutex_unlock(&attendants);
			break; 
		}

	}
}





void *passengerQueue(void *arg) {
	struct argStruct *args = (struct argStruct *)arg;	

	sem_wait(&semPass);
	pthread_mutex_lock(&lockPassengers);
		
		int passenger = ((args -> passengerLimit) - stepsLeft[0] + 1); 

		printf("Passenger #%i arrived at the terminal.\n", passenger);
		args -> bagsWaiting.push_back(passenger);
				
		stepsLeft[0]--;
		
		printf("Passenger #%i is waiting at baggage processing for a handler.\n", args -> bagsWaiting.back());		

	pthread_mutex_unlock(&lockPassengers);
	sem_post(&semBag);

}


