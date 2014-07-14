/*  CISC 4335 - Parallel and Distributed Computing
*            Sieve of Eratosthenes
*               Michael Figueroa
*                   Homework #1
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define NUMTHRDS 8
#define VECLEN 5000001 //Number of primes to test VECLEN - 1 (Add a one to account for 0)
#define LOOP true

pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;
double sum = 0; //Global sum
int comp[VECLEN]; //Array of primes/composites

void *findPrimes(void *arg) {

    int     i, start, end, len ;
    long    offset;
    double  mysum;
    offset  = (long)arg;
    printf  ("I am thread number  %d \n", offset);
    len     =  VECLEN / NUMTHRDS;
    start   =  offset * len;
    end     =  start + len;
    mysum   = 0;

    // Initialize array to all 0's (prime), except 0,1 & 2
    for(i = start; i <= end; i++) {
        if(i == 0 || i == 1)
            comp[i] = 1;
        else if(i%2 == 0 && i != 2)
            comp[i] = 1;
    }

    // Go through each threads range from start to end
    for (i = start; i <= end; i++) {
        // Check for non-composites
        if(comp[i] != 1) {
            //Test divisibility by ever odd less than i
            for(int j = 3; j * j <= i; j+=2) {
                // If i is divisible by j with remainder 0, mark as composite
                if(i%j == 0) {
                    comp[i] = 1;
                    break;
                }
            }
        }
    }

    // Count remaining primes
    for(i = start; i <= end; i++)
        if(comp[i] == 0)
            mysum++;


    pthread_mutex_lock (&mutexsum);
    sum += mysum;
    pthread_mutex_unlock (&mutexsum);

    pthread_exit((void*) 0);

}


int main (int argc, char *argv[]) {

    long     i;
    int      findNum = -1;
    void     *status;

    pthread_attr_t attr;

    pthread_mutex_init(&mutexsum, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i = 0; i < NUMTHRDS; i++) {
        pthread_create(&callThd[i], &attr, findPrimes, (void *)i);
    }

    pthread_attr_destroy(&attr);

    for(i=0; i<NUMTHRDS; i++) {
        pthread_join(callThd[i], &status);
    }

    std::cout << "\nNumber of primes from 0 to " << VECLEN - 1 << ": " << sum << std::endl;
    pthread_mutex_destroy(&mutexsum);

    // Loop to check if a number is prime or not
    while(LOOP && findNum != 0) {
        std::cout << "\nEnter number to check if prime (Enter 0 to terminate): ";
        std::cin >> findNum;

        if( comp[findNum] == 0 && findNum < VECLEN)
            std::cout << "The number " << findNum << " is a prime!\n";
        else if( comp[findNum] == 1 && findNum < VECLEN)
            std::cout << "The number " << findNum << " is a composite!\n";
        else if( findNum >= VECLEN )
            std::cout << "Number not part of array, please enter a number in range!\n";
    }

    pthread_exit(NULL);

}
