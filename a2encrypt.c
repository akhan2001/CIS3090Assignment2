/**
 * @file a2encrypt.c
 * @author Abdullah Khan (1101209) - mkhan94@uoguelph.ca
 * @brief Implements serial encryption given user input and stores into file
 * named "ciphertext.txt"
 * @version 0.1
 * @date 2021-10-29
 * @copyright Copyright (c) 2021
 * Execution: make
 * Compilation: ./a2encrypt "[word(s) to be encrypted]" eg. $:~ ./a2encrypt "the cat"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashtable.h"

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
 * @brief Rearranges the input dictionary by randomly swapping characters
 * @param str
 * @return char *rearrangedStr
 */
char *rearrange_chars(char *str) {
    srand(time(0));
    int length = strlen(str);
	char *rearrangedStr = malloc(sizeof(char) * (length + 1));
	strcpy(rearrangedStr, str);

    for (int i = 0; i < length; i++) {
        /*Goes for potentionally only 40% of randomness*/
        int randNum = (rand() % (5 - 1 + 1)) + 1;
        char temp = rearrangedStr[i];
        rearrangedStr[i] = rearrangedStr[randNum];
        rearrangedStr[randNum] = temp;
    }

	return rearrangedStr;
}

/********  MAIN  ********/
int main(int argc, char *argv[]) {
    /*Declarations of variables*/
    char *inputStr;
    char *inputDict;
    char *encryptDict;
    //char *encryptStr;

    /*Basic error-checking/initializing for command-line arguments - "the cat"*/
    if (argc == 1) {
        printf("~ Enter ./a2encrypt \"[word(s) to be encrypted]\" ~\n");
        exit(0);
    } else if (argc > 2) {
        printf("~ Enter ./a2encrypt \"[word(s) to be encrypted]\" (surrounded by \"...\")~\n");
        exit(0);
    } else {
        inputStr = argv[1];
    }

    //printf("(1) %s\n", inputStr);

    /*Removing any blank-spaces and/or repeating characters - "theca"*/
    inputDict = remove_redundancies(inputStr);
    //printf("(2) %s\n", inputDict);

    /*Rearranging characters in the input dictionary - "cehat"*/
    encryptDict = rearrange_chars(inputDict);
    //printf("(3) %s\n", encryptDict);

    /*Using hashmaps, match input dictionary to the encryption dict.*/
    sizeTable = strlen(inputDict);
    HashTable *ht = initialize_table();

    for (int i = 0; i < sizeTable; i++) {
        char *ptr = malloc(2 * sizeof(char));
        ptr[0] = inputDict[i];
        ptr[1] = '\0';
        char *ptr2 = malloc(2 * sizeof(char));
        ptr2[0] = encryptDict[i];
        ptr2[1] = '\0';
        insert_value(ht, ptr, ptr2);
    }

    char *value;
    char encryptStr[strlen(inputStr)][2];
    
    /*Swap letters*/
    for (int i = 0; i < strlen(inputStr); i++) {
        char *c = malloc(2);
        c[0] = inputStr[i];
        c[1] = '\0';
        c = receive_value(ht, c);

        if (c == NULL) {
            strcpy(encryptStr[i], " ");
        } else {
            strcpy(encryptStr[i], c);
        }
    }
    
    /*Store string into file called "ciphertext.txt"*/
    FILE *fp;
    /*Open file in append mode*/
    fp = fopen("ciphertext.txt", "a");
    if (fp == NULL)
    {
        /*If file pointer is null, exit*/
        printf("~ File not found ... exiting ~\n", "ciphertext.txt");
        exit(0);
    }
    /*Append the file with the ciphered text*/
    for (int k = 0; k < sizeof(encryptStr) / 2; k++) {
        fputs(encryptStr[k], fp);
    }
    fputs("\n", fp);
    /*Close file pointer*/
    fclose(fp);

    return 0;
}

