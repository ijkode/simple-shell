//Simple Shell
//author: Liran Libster
//description: this program is a simple shell that runs given commands

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/types.h>

#include <pwd.h>

#include <unistd.h>

#include <wait.h>

#define MAX 512

void printDir();

char ** buildArr(char * word, int wordsCounter);

int countWords(char * word);

void freeArray(char ** array, int numberofWords);

int pipeCount(char * word);

void onePipe(char ** array1, char ** array2);

void twoPipes(char ** array1, char ** array2, char ** array3);

void sigtstpHandler(int num);

void sigchldHandler(int num);

pid_t childpid;
pid_t contChild;

int main() {

    char str[MAX];
    char strin[MAX];
    char strout[MAX];
    char strout2[MAX];
    char ** array1;
    char ** array2;
    char ** array3;
    int commandCounter = 0; //counter to know how many commands
    int strLength = 0; //counter to know how many character in the string
    int numberofWords;
    int numofPipes = 0;

    while (1) {

        printDir();
        signal(SIGTSTP, sigtstpHandler);
        signal(SIGCHLD, sigchldHandler);
        fgets(str, MAX, stdin);
        strLength += (int) strlen(str) - 1; //every time the user type new string add the number of characters to the integer
        commandCounter++;
        str[strlen(str) - 1] = '\0';

        numberofWords = countWords(str);

        if (numberofWords == 0) { //if the input is enter or spaces dont exit
            continue;
        }

        int pipes = pipeCount(str);

        if (pipes == 0) {

            array1 = buildArr(str, numberofWords);

            if (strcmp(array1[0], "done") == 0 && numberofWords == 1) { //if the user input is "done" and the string contains only 1 word print the statistics and exit the program

                printf("Number of commands: %d\n", commandCounter);
                printf("Number of pipes: %d\n", numofPipes);
                printf("See you Next time !\n");
                freeArray(array1, numberofWords); //final free before exit
                return 0;
            }

            if (strcmp(array1[0], "cd") == 0) {
                printf("Command not supported (Yet)\n");
                wait(NULL);
                continue;
            }

            pid_t pid = fork();
            childpid = pid;

            if (strcmp(str, "fg") == 0) {
                kill(contChild, SIGCONT);
                pause();
            }

            if (pid < 0) {
                perror("Fork Failed");
                freeArray(array1, numberofWords);
                exit(1);
            }
            if (pid == 0) {
                signal(SIGTSTP, SIG_DFL);
                if (execvp(array1[0], array1) < 0 && strcmp(array1[0], "cd") != 0) {
                    if (strcmp(str, "fg") != 0)
                        printf("command not found\n");
                    exit(1);

                }
            }
            waitpid(pid, NULL, WUNTRACED);
            freeArray(array1, numberofWords);
        }

        if (pipes > 0 && pipes < 3) {

            if (str[0] == '|' || str[strlen(str) - 1] == '|') {
                printf("command not supported\n");
                continue;
            }

            if (pipes == 1) {
                numofPipes++;
                for (int i = 0; i < strlen(str); i++) { //separate the string into 2 new strings (before the pipe and after)
                    if (str[i] == '|') {
                        strin[i] = '\0';
                        for (int j = 0; j < strlen(str); j++) {
                            strout[j] = str[i + 1];
                            i++;
                            strout[j + 1] = '\0';
                        }
                        break;
                    }
                    strin[i] = str[i];
                }
                int strinWords = countWords(strin);
                int stroutWords = countWords(strout);

                array1 = buildArr(strin, strinWords);
                array2 = buildArr(strout, stroutWords);
                if (strcmp(array1[0], "cd") == 0 || strcmp(array2[0], "cd") == 0) {
                    printf("Command not supported (Yet)\n");
                    wait(NULL);
                    continue;
                }

                if (strcmp(array1[0], "done") == 0 || strcmp(array2[0], "done") == 0) {
                    printf("Command not found\n");
                    wait(NULL);
                    continue;
                }

                onePipe(array1, array2);
                freeArray(array1, strinWords);
                freeArray(array2, stroutWords);
            }

            if (pipes == 2) {
                numofPipes += 2;
                int lastPipe = 0; //flag that will tell the position of the last pipe in the string
                for (int i = 0; i < strlen(str); i++) { //separate the string into 2 new strings (before the pipe and after)
                    if (str[i] == '|') {
                        strin[i] = '\0';
                        for (int j = 0; j < strlen(str); j++) {
                            strout[j] = str[i + 1];
                            i++;
                            strout[j + 1] = '\0';
                            lastPipe++;
                            if (str[i] == '|') {
                                strout[j] = '\0';
                                break;
                            }
                        }
                        break;
                    }
                    lastPipe++;
                    strin[i] = str[i];
                }

                int constPipe = lastPipe;

                for (int l = 0; l < strlen(str) - constPipe; l++) { //create the last string using the position of the last pipe we saved
                    lastPipe++;
                    strout2[l] = str[lastPipe];
                    strout2[l + 1] = '\0';
                }

                int strinWords = countWords(strin);
                int stroutWords = countWords(strout);
                int strout2Words = countWords(strout2);

                array1 = buildArr(strin, strinWords);
                array2 = buildArr(strout, stroutWords);
                array3 = buildArr(strout2, strout2Words);
                if (strcmp(array1[0], "cd") == 0 || strcmp(array2[0], "cd") == 0 || strcmp(array3[0], "cd") == 0) {
                    printf("Command not supported (Yet)\n");
                    wait(NULL);
                    continue;
                }
                if (strcmp(array1[0], "done") == 0 || strcmp(array2[0], "done") == 0 || strcmp(array3[0], "done") == 0) {
                    printf("Command not found\n");
                    wait(NULL);
                    continue;
                }
                twoPipes(array1, array2, array3);
                freeArray(array1, strinWords);
                freeArray(array2, stroutWords);
                freeArray(array3, strout2Words);

            }

        }
        if (pipes >= 3) {
            printf("Command not supported\n");
            continue;
        }

    }
}

int countWords(char * word) {

    int i;
    int counter = 0;
    int ignoreSpace = 0; // flag that ignores spaces (after the first space if theres many)

    if (word[0] == '\0' || word[0] == ' ') { // check if the first input is enter of space(if its enter decrease the counter so the output will be 0 words
        counter--;
    }

    for (i = 0; i < strlen(word); i++) {

        if ((word[i]) == ' ') {

            if (ignoreSpace == 0) {
                counter++;
                ignoreSpace = 1;
            }
        }
        if (word[i + 1] != ' ') {
            ignoreSpace = 0; //return the flag to its original to count again

        }
        if (word[i] == ' ' &&
            word[i + 1] == '\0') { //if the last word is space ignore addition of 1 more to the counter
            counter--;
        }

    }

    counter++;
    return counter;

}

char ** buildArr(char * word, int wordsCounter) {

    char ** Arr = malloc((wordsCounter + 1) * sizeof(char * ));

    if (Arr == NULL) {
        fprintf(stderr, "ERR");
        exit(1);
    }

    int charNum = 0; //count characters
    int arrCounter = 0;

    for (int j = 0; j < strlen(word); j++) {

        if (word[j] != ' ' && word[j] != '\0' && word[j] != '\n') {

            charNum++;

            if (word[j] == '"') { //support " " commands
                char * gWord = malloc((charNum + 1) * sizeof(char));
                int counter = 0;
                for (int k = j; k < strlen(word); k++) {
                    if (word[k + 1] == '"') {
                        gWord[counter] = word[k];
                        break;
                    }
                    gWord[counter] = word[k + 1];
                    counter++;
                }
                gWord[counter] = '\0';

                Arr[arrCounter] = (char * ) malloc(sizeof(char) * (charNum + 1));
                if (Arr[arrCounter] == NULL) {
                    free(gWord);
                    freeArray(Arr, wordsCounter);
                    fprintf(stderr, "ERR");
                    exit(1);
                }
                strcpy(Arr[arrCounter], gWord);
                free(gWord);
                Arr[arrCounter + 1] = NULL;
                break;
            }

            if (word[j + 1] == ' ' || word[j + 1] == '\0' || word[j + 1] == '\n') {

                char * newWord = malloc((charNum + 1) * sizeof(char));

                if (newWord == NULL) {
                    freeArray(Arr, wordsCounter);
                    fprintf(stderr, "ERR");;
                    exit(1);
                }
                //insert the current word into newWord

                int counter = 0;
                for (int k = j - charNum + 1; k < (j - charNum + 1) + charNum; k++) {
                    newWord[counter] = word[k];
                    counter++;
                }

                newWord[counter] = '\0';

                Arr[arrCounter] = (char * ) malloc(sizeof(char) * (charNum + 1));
                if (Arr[arrCounter] == NULL) {
                    free(newWord);
                    freeArray(Arr, wordsCounter);
                    fprintf(stderr, "ERR");
                    exit(1);
                }
                strcpy(Arr[arrCounter], newWord);
                free(newWord);
                Arr[arrCounter + 1] = NULL;
                arrCounter++;
                charNum = 0;
            }
        }
    }

    return Arr;
}

int pipeCount(char * word) {

    int pipeCounter = 0;

    for (int j = 0; j < strlen(word); j++) {
        if (word[j] == '|') {
            pipeCounter++;
        }

    }
    return pipeCounter;
}

void freeArray(char ** array, int numberofWords) {

    for (int setFree = 0; setFree < numberofWords; setFree++) {
        free(array[setFree]);
    }
    free(array);

}

void onePipe(char ** array1, char ** array2) {

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Pipe Failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid0, pid1;

    pid0 = fork();

    if (pid0 < 0) {
        perror("Fork Failed");
        exit(EXIT_FAILURE);
    }
    if (pid0 == 0) {

        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        if (execvp(array2[0], array2) < 0) {
            perror("command not found");
            exit(1);
        }

    } else {
        pid1 = fork();

        if (pid1 < 0) {
            perror("Fork Failed");
            exit(EXIT_FAILURE);
        }

        if (pid1 == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            if (execvp(array1[0], array1) < 0) {
                perror("command not found");
                exit(1);

            }

        }
        close(pipefd[1]);
        close(pipefd[0]);
        waitpid(pid1, NULL, WUNTRACED);
    }
}

void twoPipes(char ** array1, char ** array2, char ** array3) {

    int pipefd[2];
    int pipefd2[2];
    if (pipe(pipefd) == -1) {
        perror("Pipe Failed");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2) == -1) {
        perror("Pipe Failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid0, pid1, pid2;

    pid0 = fork();

    if (pid0 < 0) {
        perror("Fork Failed");
        exit(EXIT_FAILURE);
    }
    if (pid0 == 0) {

        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd2[0]);
        dup2(pipefd2[1], STDOUT_FILENO);
        close(pipefd2[1]);
        if (execvp(array2[0], array2) < 0) {
            perror("command not found");
            exit(1);
        }

    } else {
        pid1 = fork();

        if (pid1 < 0) {
            perror("Fork Failed");
            exit(EXIT_FAILURE);
        }

        if (pid1 == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            if (execvp(array1[0], array1) < 0) {
                perror("command not found");
                exit(1);

            }

        }
        close(pipefd[1]);
        close(pipefd[0]);
        waitpid(pid1, NULL, WUNTRACED);
    }

    pid2 = fork();

    if (pid2 < 0) {
        perror("Fork Failed");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0) {

        close(pipefd2[1]);
        dup2(pipefd2[0], STDIN_FILENO);
        close(pipefd2[0]);
        if (execvp(array3[0], array3) < 0) {
            perror("command not found");
            exit(1);
        }

    }
    close(pipefd2[1]);
    close(pipefd2[0]);
    waitpid(pid2, NULL, WUNTRACED);
}

void printDir() {

    char cwd[MAX];
    struct passwd * p;
    uid_t uid = 0;

    if ((p = getpwuid(uid)) == NULL) //get user name
        perror("getpwuid() error");
    else {
        printf("%s@", p -> pw_name);
    }
    if (getcwd(cwd, sizeof(cwd)) != NULL) { //get folder path
        printf("%s>", cwd);
    } else {
        perror("getcwd() error");
        exit(1);
    }
}

void sigtstpHandler(int num) {
    contChild = childpid;

}

void sigchldHandler(int num) {
    waitpid(-1, NULL, WNOHANG);
}
