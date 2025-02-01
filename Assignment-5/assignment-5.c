#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_CUSTOMERS 50
#define MAX_AISLES 5
#define MAX_SEATS 12

typedef struct {
    int customer_id;
    int aisle_seats[MAX_AISLES][MAX_SEATS];
    pthread_mutex_t seat_locks[MAX_AISLES][MAX_SEATS];
} Theater;

typedef struct {
    int customer_id;
    int aisle;
    int seats[60];
    int seat_count;
} BookingRequest;

Theater theater;

pthread_mutex_t log_lock;

void initialize_theater() {
    for (int i = 0; i < MAX_AISLES; i++) {
        for (int j = 0; j < MAX_SEATS; j++) {
            theater.aisle_seats[i][j] = 0;
            pthread_mutex_init(&theater.seat_locks[i][j], NULL);
        }
    }
}

void* process_booking(void* arg) {
    BookingRequest* request = (BookingRequest*)arg;
    int aisle = request->aisle - 1; // Convert to 0-based index
    int locks_acquired = 0;
    int success = 0;

    // Attempt to book seats
    for (int i = 0; i < request->seat_count; i++) {
        int seat = request->seats[i] - 1; // Convert to 0-based index
        if (pthread_mutex_trylock(&theater.seat_locks[aisle][seat]) == 0) {
            locks_acquired++;
        } else {
            break; // Seat already booked
        }
    }

    if (locks_acquired == request->seat_count) {
        // Successful booking
        sleep(1 + rand() % 3); // Simulate delay
        for (int i = 0; i < request->seat_count; i++) {
            int seat = request->seats[i] - 1;
            theater.aisle_seats[aisle][seat] = request->customer_id;
        }
        success = 1;
    }

    // Log booking result
    pthread_mutex_lock(&log_lock);
    printf("Customer %d - %s - ", request->customer_id, success ? "Successful" : "Fail");
    for (int i = 0; i < request->seat_count; i++) {
        printf("Aisle %d, Seat %d", request->aisle, request->seats[i]);
        if (i < request->seat_count - 1) printf(", ");
    }
    printf("\n");
    pthread_mutex_unlock(&log_lock);

    // Release all locks
    for (int i = 0; i < locks_acquired; i++) {
        int seat = request->seats[i] - 1;
        pthread_mutex_unlock(&theater.seat_locks[aisle][seat]);
    }

    free(request);
    pthread_exit(NULL);
}

void print_theater_layout() {
    printf("          ");
    for (int i = 1; i <= MAX_SEATS; i++) {
        printf("%2d  ", i);
    }
    printf("\n");

    for (int i = 0; i < MAX_AISLES; i++) {
        printf("Aisle %d ", i + 1);
        for (int j = 0; j < MAX_SEATS; j++) {
            printf("%3d ", theater.aisle_seats[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize theater
    initialize_theater();
    pthread_mutex_init(&log_lock, NULL);

    // Open file
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[MAX_CUSTOMERS];
    int thread_count = 0;

    char line[256];
    int is_header = 1; // Flag to indicate the first line (header)
    while (fgets(line, sizeof(line), file)) {
        if (is_header) {
            is_header = 0; // Skip the first line
            continue;
        }

        // Skip comments
        char* comment = strchr(line, '#');
        if (comment) *comment = '\0';

        if (strlen(line) > 1) {
            BookingRequest* request = malloc(sizeof(BookingRequest));
            char* token = strtok(line, ",");
            request->customer_id = atoi(token);

            request->seat_count = 0;
            int index = 0;
            while ((token = strtok(NULL, ",")) != NULL) {
                if (index % 2 == 0) { // Aisle number
                    request->aisle = atoi(token);
                } else { // Seat number
                    request->seats[request->seat_count++] = atoi(token);
                }
                index++;
            }

            pthread_create(&threads[thread_count++], NULL, process_booking, request);
        }
    }

    fclose(file);

    // Wait for all threads to complete
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print final theater layout
    print_theater_layout();

    // Clean up
    pthread_mutex_destroy(&log_lock);
    for (int i = 0; i < MAX_AISLES; i++) {
        for (int j = 0; j < MAX_SEATS; j++) {
            pthread_mutex_destroy(&theater.seat_locks[i][j]);
        }
    }

    return 0;
}
