#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For fork(), getpid(), getppid()
#include <sys/wait.h>  // For wait()
#include <string.h>
#include "checkPassword.h"  // For checkPassword function

void crackPass(int start) {
    char guess[4];  // 3 characters + null terminator
    guess[3] = '\0';  // Null terminate the string

    for (char ch1 = 33; ch1 <= 126; ch1++) {
        for (char ch2 = 33; ch2 <= 126; ch2++) {
            for (char ch3 = 33; ch3 <= 126; ch3++) {
                // Form the guess string
                guess[0] = ch1;
                guess[1] = ch2;
                guess[2] = ch3;

                // Test this guess
                if (checkPassword(guess, start) == 0) {
                    printf("Process %d with parent %d finds %d-%d: %s\n", getpid(), getppid(), start, start+2, guess);
                    return;  // If correct, stop searching
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int forkEnabled = 0;

    // Check if the -f flag is passed
    if (argc > 1 && strcmp(argv[1], "-f") == 0) {
        forkEnabled = 1;
    }

    printf("Process %d with parent %d cracking the password...\n", getpid(), getppid());

    if (forkEnabled) {
        // Fork into 4 child processes sequentially
        pid_t pid1 = fork();
        if (pid1 == 0) {
            crackPass(0);  // Child 1 cracks 0-2
            exit(0);
        }
 
        pid_t pid2 = fork();
        if (pid2 == 0) {
            crackPass(3);  // Child 2 cracks 3-5
            exit(0);
        }

        pid_t pid3 = fork();
        if (pid3 == 0) {
            crackPass(6);  // Child 3 cracks 6-8
            exit(0);
        }

        pid_t pid4 = fork();
        if (pid4 == 0) {
            crackPass(9);  // Child 4 cracks 9-11
            exit(0);
        }
        
// Parent waits for all children to finish
        int status;
        waitpid(pid1, &status, 0);
        waitpid(pid2, &status, 0);
        waitpid(pid3, &status, 0);
        waitpid(pid4, &status, 0);
              
    } else {
        // Run the password cracking sequentially
        crackPass(0);
        crackPass(3);
        crackPass(6);
        crackPass(9);
    }

    return 0;
}
