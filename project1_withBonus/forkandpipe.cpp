/** 
 *  Author: Elliot Thompson
 *  Assignment: Project 1 CSCI 474: OS Concepts
 *  Date 10-28-2021
 * 
 *  code was written with help from the following sources:
 *  fileio:       https://www.cprogramming.com/tutorial/lesson10.html
 *  forkpipe:     https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
 *  printf c_str: http://www.cplusplus.com/forum/beginner/100593/
 */
#include <stdlib.h> // normal stuff
#include <stdio.h> // io streams
#include <unistd.h> // not sure
#include <string.h> // for strings
#include <iostream> // reading from stdin,out
#include <fstream> // file stream help
#include <sys/time.h> // for timer stuff

using namespace std;

/**
 * sum all lines of the file and write total to pipe 
 */
void processFile(string fileName, int fds[]);

float time_diff(struct timeval *start, struct timeval *end);

int main() {
    
    // return pipes fds and other vars
    int childResultReturn[4][2];
    string fileNames[4] = {"./2021_Fall_project_data_Files/block1.dat", 
        "./2021_Fall_project_data_Files/block2.dat", 
        "./2021_Fall_project_data_Files/block3.dat", 
        "./2021_Fall_project_data_Files/block4.dat"};
    int total = 0;
    int result = 0;
    pid_t p;
    struct timeval start{};
    struct timeval end{};

    gettimeofday(&start, nullptr);

    for(int i = 0; i< 4; i++) {
        // instantiate the file descriptors for each pipe and error check
        if (pipe(childResultReturn[i]) == -1){
            printf("pipe failed on loop %d\n", i);
            exit(1);
        }

        // create the children and process, the parent will continue through the loop 
        p = fork(); 
        
        if (p < 0) {
            printf("Fork failed");
            exit(1);
        } else if (p == 0) { //child process
            // child i will process file i and return through pipe i
            string fileName = fileNames[i];
            // sum ints from each line and write to pipe
            processFile(fileName, childResultReturn[i]);
            exit(0);
        }
    }
    // loop only the parent goes through since child all exit
    for(int i = 0; i< 4; i++) {
        // parent will read from each pipe and add to result
        close(childResultReturn[i][1]);  // Close writing end of the pipe, to make sure

        // Read string from child, print it and close, will wait until stuff is there
        read(childResultReturn[i][0], &result, sizeof(int));
        total += result;

        // close reading end
        close(childResultReturn[i][0]);

        printf("Total for child %d is: %d\n", i+1, result);
    }
    
    gettimeofday(&end, nullptr);

    printf("Total overall is: %d\n", total);
    printf("Time taken is: %0.8f ms\n", time_diff(&start, &end));
    exit(0);
}

void processFile(string fileName, int fds[]) {
    ifstream file (fileName.c_str());
    int inputline ;
    int sum = 0;

    if ( !file.is_open() ) {
        printf("file failed to open");
        exit(1); //error
    } else {
        // sum the lines of file
        while (file >> inputline) { 
            sum += inputline;
        }
    }
    write(fds[1], &sum, sizeof(int));
    close(fds[1]);
}

float time_diff(struct timeval *start, struct timeval *end){
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}