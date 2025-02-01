/* This program is written by Tazrin Khalid. It simulates three scheduling algorithms: First Come First Served, Shortest Job First,
 * and Round Robin. It reads data from an input file, computes waiting times, turnaround times, and outputs execution steps. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100

// Structure to represent a process with its scheduling attributes
typedef struct {
    char id[10];               // Process ID
    int burst_time;            // Burst time of the process
    int arrival_time;          // Arrival time of the process
    int wait_time;             // Waiting time for the process
    int turnaround_time;       // Turnaround time of the process
    int remaining_time;        // Remaining burst time (used in Round Robin)
} Process;

// First Come First Served (FCFS) scheduling
void fcfs(Process processes[], int count) {
    printf("First Come First Served\n");
    int current_time = 0;
    float total_wait = 0, total_turnaround = 0;

    // Process each job in order of arrival
    for (int i = 0; i < count; i++) {
        // If the CPU is idle, advance the current time to the process's arrival time
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        // Calculate waiting and turnaround times for the process
        processes[i].wait_time = current_time - processes[i].arrival_time;
        processes[i].turnaround_time = processes[i].wait_time + processes[i].burst_time;
        current_time += processes[i].burst_time;

        // Print burst details for the process
        for (int t = 0; t < processes[i].burst_time; t++) {
            printf("T%d : %s - Burst left %d, Wait time %d, Turnaround time %d\n",
                   current_time - processes[i].burst_time + t,
                   processes[i].id,
                   processes[i].burst_time - t - 1,
                   processes[i].wait_time,
                   t + processes[i].wait_time + 1);
        }
    }

    // Print summary and calculate total waiting and turnaround times
    for (int i = 0; i < count; i++) {
        printf("%s\n        Waiting time: %d\n        Turnaround time: %d\n",
               processes[i].id,
               processes[i].wait_time,
               processes[i].turnaround_time);
        total_wait += processes[i].wait_time;
        total_turnaround += processes[i].turnaround_time;
    }

    // Print average waiting and turnaround times
    printf("Total average waiting time: %.1f\n", total_wait / count);
    printf("Total average turnaround time: %.1f\n", total_turnaround / count);
}

// Shortest Job First (SJF) scheduling
void sjf(Process processes[], int count) {
    printf("Shortest Job First\n");
    int completed = 0, current_time = 0;
    float total_wait = 0, total_turnaround = 0;
    int is_completed[MAX_PROCESSES] = {0};

    // Continue until all processes are completed
    while (completed < count) {
        int idx = -1, min_burst = 1e9;

        // Find the shortest job that has arrived
        for (int i = 0; i < count; i++) {
            if (processes[i].arrival_time <= current_time && !is_completed[i] && processes[i].burst_time < min_burst) {
                min_burst = processes[i].burst_time;
                idx = i;
            }
        }

        // If no process is ready, increment the time
        if (idx == -1) {
            current_time++;
            continue;
        }

        // Execute the selected process and calculate its times
        processes[idx].wait_time = current_time - processes[idx].arrival_time;
        processes[idx].turnaround_time = processes[idx].wait_time + processes[idx].burst_time;

        // Print burst details for the selected process
        for (int t = 0; t < processes[idx].burst_time; t++) {
            printf("T%d : %s - Burst left %d, Wait time %d, Turnaround time %d\n",
                   current_time + t,
                   processes[idx].id,
                   processes[idx].burst_time - t - 1,
                   processes[idx].wait_time,
                   t + processes[idx].wait_time + 1);
        }

        // Update the current time and mark the process as completed
        current_time += processes[idx].burst_time;
        is_completed[idx] = 1;
        completed++;

        // Accumulate total waiting and turnaround times
        total_wait += processes[idx].wait_time;
        total_turnaround += processes[idx].turnaround_time;
    }

    // Print summary for all processes
    for (int i = 0; i < count; i++) {
        printf("%s\n        Waiting time: %d\n        Turnaround time: %d\n",
               processes[i].id,
               processes[i].wait_time,
               processes[i].turnaround_time);
    }

    // Print average waiting and turnaround times
    printf("Total average waiting time: %.1f\n", total_wait / count);
    printf("Total average turnaround time: %.1f\n", total_turnaround / count);
}

// Round Robin scheduling
void round_robin(Process processes[], int count, int quantum) {
    printf("Round Robin with Quantum %d\n", quantum);
    int current_time = 0, completed = 0;
    float total_wait = 0, total_turnaround = 0;

    // Continue until all processes are completed
    while (completed < count) {
        int active = 0;

        // Loop through all processes to allocate CPU time
        for (int i = 0; i < count; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                active = 1;
                int time_spent = (processes[i].remaining_time > quantum) ? quantum : processes[i].remaining_time;
                processes[i].remaining_time -= time_spent;
                current_time += time_spent;

                // Print burst details for the process
                for (int t = 0; t < time_spent; t++) {
                    printf("T%d : %s - Burst left %d, Wait time %d, Turnaround time %d\n",
                           current_time - time_spent + t,
                           processes[i].id,
                           processes[i].remaining_time,
                           processes[i].wait_time,
                           t + processes[i].wait_time + 1);
                }

                // Update waiting times for other processes
                for (int j = 0; j < count; j++) {
                    if (j != i && processes[j].remaining_time > 0 && processes[j].arrival_time <= current_time) {
                        processes[j].wait_time += time_spent;
                    }
                }

                // Mark process as completed if finished
                if (processes[i].remaining_time == 0) {
                    processes[i].turnaround_time = current_time - processes[i].arrival_time;
                    completed++;
                }
            }
        }

        // If no process is active, increment the current time
        if (!active) current_time++;
    }

    // Print summary for all processes
    for (int i = 0; i < count; i++) {
        total_wait += processes[i].wait_time;
        total_turnaround += processes[i].turnaround_time;

        printf("%s\n        Waiting time: %d\n        Turnaround time: %d\n",
               processes[i].id, processes[i].wait_time, processes[i].turnaround_time);
    }

    // Print average waiting and turnaround times
    printf("Total average waiting time: %.1f\n", total_wait / count);
    printf("Total average turnaround time: %.1f\n", total_turnaround / count);
}

// Function to simulate the scheduling algorithm
void simulate(char *algorithm, int quantum, char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    Process processes[MAX_PROCESSES];
    int count = 0;

    // Read process data from file
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0'; // Remove newline characters
        if (sscanf(line, "%[^,],%d", processes[count].id, &processes[count].burst_time) == 2) {
            if (strlen(processes[count].id) == 0) {
                fprintf(stderr, "Error: Process ID is missing at index %d\n", count);
                fclose(file);
                exit(EXIT_FAILURE);
            }

            processes[count].arrival_time = count; // Sequential arrival
            processes[count].remaining_time = processes[count].burst_time;
            processes[count].wait_time = 0;
            processes[count].turnaround_time = 0;
            count++;
        } else {
            fprintf(stderr, "Warning: Skipping invalid or blank line: %s\n", line);
        }
    }

    fclose(file);

    // Exit if no valid processes are found
    if (count == 0) {
        fprintf(stderr, "Error: No processes found in file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Call the appropriate scheduling algorithm
    if (strcmp(algorithm, "-f") == 0) {
        fcfs(processes, count);
    } else if (strcmp(algorithm, "-s") == 0) {
        sjf(processes, count);
    } else if (strcmp(algorithm, "-r") == 0) {
        if (quantum <= 0) {
            fprintf(stderr, "Error: Invalid time quantum\n");
            exit(EXIT_FAILURE);
        }
        round_robin(processes, count, quantum);
    } else {
        fprintf(stderr, "Error: Invalid scheduling algorithm %s\n", algorithm);
        exit(EXIT_FAILURE);
    }
}

// Main function to parse the given command-line arguments and execute the program
int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s -f|-s|-r [quantum] <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *algorithm = argv[1];
    int quantum = 0;

    if (strcmp(algorithm, "-r") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage for Round Robin: %s -r <quantum> <input_file>\n", argv[0]);
            return EXIT_FAILURE;
        }
        quantum = atoi(argv[2]);
    }

    char *filename = argv[argc - 1];
    simulate(algorithm, quantum, filename);

    return 0;
}
