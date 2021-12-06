/**
 * @file a2decrypt.c
 * @author Abdullah Khan (mkhan94@uoguelph.ca) - 1101209
 * @brief Reads encrypted string from "ciphertext.txt" and the corresponding list
 * of letters that occur in the string. Then creates a decryption list and reorders
 * the decryption dictionary until correct text found
 * @version 0.1
 * @date 2021-10-29
 * @copyright Copyright (c) 2021
 * Compilation: mpicc a2decrypt -o a2decrypt.c
 * Run: mpiexec -n [# of processors] ./a2decrypt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <sys/time.h>

int sizeTable;

char *dictionaryFile = "/usr/share/dict/words";

/**
 * @brief Calculates the elapsed time from when it was called to when it ends
 * (Got function code from parallelSort.c provided by Prof. Denis)
 * @param start 
 * @return double time
 */
double calcTime(struct timeval start) {
    long long startusec, endusec;
    struct timeval	end;
    
    gettimeofday(&end, NULL);
    startusec = start.tv_sec * 1000000 + start.tv_usec;
    endusec = end.tv_sec * 1000000 + end.tv_usec;
    return (double)(endusec - startusec) / 1000000.0;
}

/**
 * @brief Searches for valid word in dictionary with given string
 * @param str 
 * @return int 1 or 0
 */
int search_words(char *str) {
    /*Reads words from file "/usr/share/dict/words"*/
    char line[256];
    FILE *fp;
    /*Open file in read mode*/
    fp = fopen(dictionaryFile, "r");
    if (fp == NULL)
    {
        printf("~ File not found ... exiting ~\n", "ciphertext.txt");
        exit(0);
    }

    /*Search and compare all words in dictionary*/
    while (fgets(line, 256, fp)) {
        int size = strlen(line);
        line[size - 1] = '\0';
        /*If given word is valid in dictionary, return 1*/
        if (strcmp(line, str) == 0) {
            fclose(fp);
            return 1;
        }
    }
    
    fclose(fp);
    /*Otherwise close file, and return 0*/
    return 0;
}

/**
 * @brief Removes any blank-spaces and/or repeating characters - "theca"
 * @param str
 * @return char *cleanStr
 */
char *remove_redundancies(char *str) {
    int length = strlen(str);
	char *cleanStr = malloc(sizeof(char) * (length + 1));
    strcpy(cleanStr, str);

    for (int i = 0; i < strlen(cleanStr); i++) {
        for (int j = i + 1; cleanStr[j] != '\0'; j++) {
            /*If repeating character, remove*/
            if (cleanStr[j] == cleanStr[i]) {
                for (int k = j; cleanStr[k] != '\0'; k++) {
					cleanStr[k] = cleanStr[k + 1];
				}
            }
        }
    }
    int k = 0;
    for (int i = 0; cleanStr[i]; i++) {
     	cleanStr[i] = cleanStr[i + k];
         /*If " " (space), remove*/
     	if (cleanStr[i] == ' ') {
		    k++;
		    i--;
	    }
    }
    return cleanStr;
}

/**
 * @brief Simple swap function
 * @param x 
 * @param y 
 */
void swap(char *x, char *y) {
    char temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

/**
 * @brief Finds all possible listings for the words in rearranged format (permutations)
 * @param str 
 * @param index 
 * @param strSize
 */
void functional_dictionary(char *str, int index, int strSize) {
    int counter;
    if (index == strSize) {
        counter++;
        if (search_words(str) == 1) {
            printf("%s\n", str);
        }
    } else {
        for (int i = index; i <= strSize; i++) {
            swap((str + index), (str + i));
            functional_dictionary(str, index + 1, strSize);
            swap((str + index), (str + i));
            //printf("%d %d\n", index, strSize);
        }
    }
}

int main(int argc, char *argv[]) {
    /*Declarations of variables*/
    char inputStr[256];
    char *inputDict;
    char *encryptDict;
    int nProcesses; //number of processes
    int my_rank; //My process rank
    double elapsed;
	struct timeval  start;

    /*Basic error-checking/initializing for command-line arguments - "the cat"*/
    if (argc > 1) {
        fprintf(stderr, "~ Enter mpiexec -n [# of processes] ./a2decrypt ~\n");
        exit(0);
    }

    /*Get the timing data*/
	gettimeofday(&start, NULL);
    
    /*Initialize MPI, number of processors and rank among all processes*/
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /*Rank 0 is producer process*/
    if (my_rank == 0) {
        /*Reads words from ciphertext.txt*/
        FILE *fp;
        /*Open file in read mode*/
        fp = fopen("ciphertext.txt", "r");
        if (fp == NULL) {
            /*If file pointer is null, exit*/
            printf("~ File not found ... exiting ~\n", "ciphertext.txt");
            exit(0);
        }
        /*Reads the line until \n is reached*/
        fscanf(fp, "%[^\n]", inputStr);
        printf("(1) %s\n", inputStr);
        fclose(fp);

        /*Checks if file words have space in between or not*/
        int hasSpace = 0;
        for (int k = 0; k < strlen(inputStr); k++) {
            if (inputStr[k] == ' ') {
                hasSpace = 1;
            }
        }

        /*Removing any blank-spaces and/or repeating characters - "theca"*/
        inputDict = remove_redundancies(inputStr);
        //printf("(2) %s - ", inputDict);
        /*Finds size of table for unique characters*/
        sizeTable = strlen(inputDict);
        //printf("Size: %d\n", strlen(inputDict));
        
        /*Assumes that there will only be one processor per each unique letter*/
        if (nProcesses > sizeTable) {
            printf("~ String length is greater than number of processors ~\n");
            exit(0);
        }

        // for(int i = 1; i < nProcesses; i++) {
        //     MPI_Send(inputDict, strlen(inputDict) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD); 
        // }
         MPI_Send(inputDict, strlen(inputDict) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        //int tempCounter = 0;
        printf("Process %d: ", my_rank);
        functional_dictionary(inputDict, 0, strlen(inputDict) - 1);
    } else {
        //MPI_Recv(inputDict, strlen(inputDict) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 1; i <= nProcesses; i++) {
            MPI_Recv(inputDict, strlen(inputDict) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        printf("Process %d: ", my_rank);
        functional_dictionary(inputDict, 0, strlen(inputDict) - 1);
    }

    /*Get the elapsed time is took for the processes to do the work*/
    elapsed = calcTime(start);
	printf("This program took %.4f seconds\n", elapsed);

    /*Shut down MPI*/
    MPI_Finalize();

    exit(0);
}