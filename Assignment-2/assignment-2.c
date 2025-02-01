// This program performs large integer multiplication using pipes between a parent and child process.
// Written by Tazrin Khalid, Student# 251294394.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    // Check for exactly 2 integer arguments (4-digit numbers)
    if (argc != 3) {
        printf("Usage: %s <4-digit-integer> <4-digit-integer>\n", argv[0]);
        return 1;
    }

    // Convert input arguments to integers
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    // Check if inputs are valid 4-digit numbers
    if (a < 1000 || a > 9999 || b < 1000 || b > 9999) {
        printf("Error: Both numbers must be 4 digits.\n");
        return 1;
    }

    // Decompose the numbers into two 2-digit parts each
    int a1 = a / 100, a2 = a % 100;
    int b1 = b / 100, b2 = b % 100;

    int pipe1[2], pipe2[2]; // Two pipes for communication (parent->child and child->parent)

    // Create the pipes, checking for errors
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork(); // Create a child process

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid > 0) {  // Parent Process
        // Close unused ends of the pipes
        close(pipe1[0]); // Close unused read end of pipe1
        close(pipe2[1]); // Close unused write end of pipe2

        printf("Your integers are %d %d\n", a, b);
        printf("Parent (PID %d): created child (PID %d)\n\n", getpid(), pid);

        // Communicate for each stage: X, Y, and Z

        // 1. Calculate X = a1 * b1 * 10000
        printf("###\n# Calculating X\n###\n");
        int A;
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a1);
        write(pipe1[1], &a1, sizeof(a1));  // Send a1 to the child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b1);
        write(pipe1[1], &b1, sizeof(b1));  // Send b1 to the child
        read(pipe2[0], &A, sizeof(A));     // Read the result from the child
        printf("Parent (PID %d): Received %d from child\n\n", getpid(), A);
        int X = A * 10000;                 // Multiply result by 10000

        // 2. Calculate Y = (a1 * b2 + a2 * b1) * 100
        printf("###\n# Calculating Y\n###\n");
        int B, C;

        // First part of Y = a1 * b2
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a1);
        write(pipe1[1], &a1, sizeof(a1));  // Send a1 to the child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b2);
        write(pipe1[1], &b2, sizeof(b2));  // Send b2 to the child
        read(pipe2[0], &C, sizeof(C));     // Read the result from the child
        printf("Parent (PID %d): Received %d from child\n", getpid(), C);

        // Second part of Y = a2 * b1
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a2);
        write(pipe1[1], &a2, sizeof(a2));  // Send a2 to the child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b1);
        write(pipe1[1], &b1, sizeof(b1));  // Send b1 to the child
        read(pipe2[0], &B, sizeof(B));     // Read the result from the child
        printf("Parent (PID %d): Received %d from child\n\n", getpid(), B);

        int Y = (B + C) * 100;             // Add results and multiply by 100

        // 3. Calculate Z = a2 * b2
        printf("###\n# Calculating Z\n###\n");
        int D;
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a2);
        write(pipe1[1], &a2, sizeof(a2));  // Send a2 to the child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b2);
        write(pipe1[1], &b2, sizeof(b2));  // Send b2 to the child
        read(pipe2[0], &D, sizeof(D));     // Read the result from the child
        printf("Parent (PID %d): Received %d from child\n\n", getpid(), D);

        int Z = D;  // Final part of the multiplication

        // Final output: X + Y + Z
        printf("%d * %d == %d + %d + %d == %d\n", a, b, X, Y, Z, X + Y + Z);

        // Close the pipes
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2

    } else {  // Child Process
        // Close unused ends of the pipes
        close(pipe1[1]); // Close unused write end of pipe1
        close(pipe2[0]); // Close unused read end of pipe2

        int num1, num2, result;

        // Read two numbers from the parent, multiply, and send the result back
        for (; read(pipe1[0], &num1, sizeof(num1)) > 0 && read(pipe1[0], &num2, sizeof(num2)) > 0; ) {
            printf("Child (PID %d): Received %d from parent\n", getpid(), num1);
            printf("Child (PID %d): Received %d from parent\n", getpid(), num2);
            result = num1 * num2;
            printf("Child (PID %d): Sending %d to parent\n", getpid(), result);
            write(pipe2[1], &result, sizeof(result));
        }


        // Close the pipes
        close(pipe1[0]); // Close read end of pipe1
        close(pipe2[1]); // Close write end of pipe2
    }

    return 0;
}
