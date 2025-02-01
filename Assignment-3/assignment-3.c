/*
 * This program calculates the sum and count of prime numbers up to a specified maximum (N)
 * by dividing the work across multiple threads. 
 * This program is written by Tazrin Khalid (251294394)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define PRIME_BASE1 2
#define PRIME_BASE2 3

// Struct to hold data for each thread
typedef struct {
    int threadId;       
    int start;          
    int end;            
    int primeCount;    
    int primeSum;       
} ThreadData;

// Function to check if a number is prime
int isPrime(int num) {
    if (num <= 1) return 0;                 // Numbers <= 1 are not prime
    if (num <= PRIME_BASE2) return 1;       // 2 and 3 are prime numbers
    if (num % PRIME_BASE1 == 0 || num % PRIME_BASE2 == 0) return 0; // Eliminate even numbers and multiples of 3

    for (int i = 5; i * i <= num; i += 6) { // Check for factors of the form 6k ± 1
        if (num % i == 0 || num % (i + 2) == 0) return 0;
    }
    return 1; 
}

// Thread function to count and sum primes within specified range
void* countPrimes(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    data->primeCount = 0;
    data->primeSum = 0;

    // Iterate over range and check for primes
    for (int i = data->start; i < data->end; i++) {
        if (isPrime(i)) {
            data->primeCount++;        
            data->primeSum += i;       
        }
    }

    // Print results for the thread
    printf("Thread #%d is finding primes from low = %d to high = %d\n", data->threadId, data->start, data->end);
    printf("Thread #%d Sum is %d, Count is %d\n", data->threadId, data->primeSum, data->primeCount);
    
    pthread_exit(NULL);
}

// Function to create threads and assign ranges for each thread
void createThreads(pthread_t* threads, ThreadData* threadData, int numThreads, int range, int maxNumber) {
    for (int i = 0; i < numThreads; i++) {
        threadData[i].threadId = i;
        threadData[i].start = i * range;  
        threadData[i].end = (i + 1) * range < maxNumber ? (i + 1) * range : maxNumber; 
        pthread_create(&threads[i], NULL, countPrimes, &threadData[i]);
    }
}

// Function to join threads and aggregate the results from each thread
void joinThreads(pthread_t* threads, ThreadData* threadData, int numThreads, int* grandSum, int* grandCount) {
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);  // Wait for each thread to finish
        *grandSum += threadData[i].primeSum;   
        *grandCount += threadData[i].primeCount; 
    }
}

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <numberOfThreads> <maxNumber>\n", argv[0]);
        return 1;
    }

    // Parse and validate the command-line arguments
    int numThreads = atoi(argv[1]);
    int maxNumber = atoi(argv[2]);
    if (numThreads < 1 || maxNumber < 1) {
        fprintf(stderr, "Error: number of threads and max number must be positive integers.\n");
        return 1;
    }

    pthread_t threads[numThreads];          
    ThreadData threadData[numThreads];     
    int range = (maxNumber + numThreads - 1) / numThreads; // Calculate the range for each thread

    int grandSum = 0;   
    int grandCount = 0; 

    // Create threads and assign ranges
    createThreads(threads, threadData, numThreads, range, maxNumber);
    // Join threads and aggregate results
    joinThreads(threads, threadData, numThreads, &grandSum, &grandCount);

    // Print results
    printf("\n\tGRAND SUM IS %d, COUNT IS %d\n", grandSum, grandCount);
    return 0;
}
