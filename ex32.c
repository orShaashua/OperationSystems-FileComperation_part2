/**
 * Name: Or Shaashua
 * ID:311148811
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <wait.h>

#define SIZE 160
#define SPACE " "
#define ERROR "Error in system call\n"
#define FILE_DESCRIPTORS_STDERR 2
#define RESULT_PATH "result.csv"
#define IDENTICAL 3
#define SIMILAR  2
#define DIFFERENT 1



// Outputs of c files will redirected to this file.
char outFile[SIZE];
typedef enum bool {false=0, true=1}bool;

//func definitions
void getPathOfConfigurationFile(char * configurationFile,int argc, char **argv);
void readConfigurationFile(char * configurationFile ,char* checkFolder,
                           char* inputFile, char* outputFile);
void readLine(char* line,int fd_configurationFile);
void searchCFile(char* path, char* filename, char * cFilePath);
void compileCFile(char * path, char* c_filename,char* inputFile, char* outputFile,char* directoryName);
const char *get_filename_ext(const char *filename);
void  strcatOfPath(char * first, char * second,char * dst);
void writeResult(char *name , char* grade, char *reason);
bool runCFile (char*directoryName,char * inputFile);
void compareFiles(char* outputFile,char* directoryName);


int main(int argc, char **argv) {
    char configurationFile[SIZE];
    char path[SIZE];
    char newPath[SIZE];
    char inputFile[SIZE];
    char outputFile[SIZE];
    char filename[SIZE];
    char cFilePath [SIZE];


    if (argc < 2){ //error

        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }else{
         getPathOfConfigurationFile(configurationFile, argc, argv);
    }
    readConfigurationFile(configurationFile, path, inputFile, outputFile);
    snprintf(outFile, SIZE, "%s","out.txt");
    DIR *pDir;
    struct dirent *pDirent;
    if ( (pDir = opendir(path)) == NULL) {
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }
    int check;
    if ((check = open(RESULT_PATH,O_TRUNC| O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
    }
    close(check);
    // looping through the directory, printing the directory entry name
    while ( (pDirent = readdir(pDir) ) != NULL ) {
        if (strcmp(pDirent->d_name, "..") != 0 && strcmp(pDirent->d_name, ".") != 0) {
            strcatOfPath(path,pDirent->d_name, newPath);
            searchCFile(newPath, filename, cFilePath);
            if (strcmp(filename,  "") == 0){
                writeResult(pDirent->d_name,"0","NO_C_FILE");
            }else{
                compileCFile(cFilePath, filename, inputFile,outputFile,pDirent->d_name);
            }
        }
    }
    closedir( pDir );


    return 0;
}



/**
 * This function get the path of the configuration file
 * that the user give us (can include spaces).
 */
void getPathOfConfigurationFile(char * configurationFile,int argc, char **argv){
    int i;
    for (i = 1; i < argc; i++ )
    {
        if ( i == 1 )
        {
            strcpy( configurationFile, argv[i] );
        }
        else
        {
            strcat( configurationFile, SPACE );
            strcat( configurationFile, argv[i] );
        }
    }
}

/**
 *This function read the configuration file and get the path
 * of the folder we need to check the path of inputFile
 * and the path of the currect outputFile
 */
void readConfigurationFile(char * configurationFile ,char* checkFolder,
                           char* inputFile, char* outputFile){

    int fd_configurationFile = open(configurationFile, O_RDONLY );
    if (fd_configurationFile < 0) /* means file open did not take place */
    {
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }
    readLine(checkFolder, fd_configurationFile);
    readLine(inputFile, fd_configurationFile);
    readLine(outputFile, fd_configurationFile);
}
/**
 * This function read line from fd_configurationFile
 */
void readLine(char* line,int fd_configurationFile) {

    char ch[1];
    int returnValOfFile = read(fd_configurationFile, ch, 1);
    if (returnValOfFile < 0) {     //error in read one of the files
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }
    int count = 0;

    while ((ch[0] != '\n') && (ch[0] != EOF)) {

        line[count] = ch[0];
        count++;
        returnValOfFile = read(fd_configurationFile, ch, 1);
        if (returnValOfFile < 0) {     //error in read one of the files
            write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
            exit(EXIT_FAILURE);
        }
    }

    line[count] = '\0';
}
/**
 * This function search c file.
 */
void searchCFile(char* path, char* filename, char * cFilePath){

    DIR *pDir;
    struct dirent *pDirent;
    char subDir [SIZE];
    strcpy(filename,"");

    if ( (pDir = opendir(path)) == NULL) {
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }
    // looping through the directory, printing the directory entry name
    while ( (pDirent = readdir(pDir) ) != NULL ) {
        if (strcmp(pDirent->d_name, "..") != 0 && strcmp(pDirent->d_name, ".") != 0) {
            if (pDirent->d_type == DT_REG &&
                strcmp(get_filename_ext(pDirent->d_name), "c") == 0) {
                //find c file
                snprintf(cFilePath, SIZE, "%s", path);
                snprintf(filename, SIZE, "%s", pDirent->d_name);
                break;
            } else if (pDirent->d_type == DT_DIR) {
                strcatOfPath(path, pDirent->d_name, subDir);
                searchCFile(subDir, filename, cFilePath);

            }
        }
    }
    closedir( pDir );
}

/**
 *This function return the ext of filename.
 */
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

/*
 * This function compile c file
 */
void compileCFile(char * path, char* c_filename,char* inputFile, char* outputFile,char* directoryName){
    char newPath[SIZE];
    strcatOfPath(path,c_filename,newPath);
    char *args[] = {"gcc", newPath, "-o","a.out", NULL };//input i/o re
    int status;
    pid_t pid = fork();
    if (pid == 0)
    {
        // child gets here
        execvp(args [0],args);
        exit(EXIT_FAILURE);
    }else if (pid== -1){
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }

    else
    {
        // father waiting to son to finish.
        waitpid(pid, &status, 0);

        // son has compilation error.
        if (WEXITSTATUS(status) == EXIT_FAILURE) {
            writeResult(directoryName,"0", "COMPILATION_ERROR");
        }else {
            if(runCFile(directoryName,inputFile)){
                compareFiles(outputFile,directoryName);
            }
        }
    }
}

void compareFiles(char* outputFile,char* directoryName){
    char *args[] = {"./comp.out",outFile, outputFile,NULL };
    int status = 0;
    pid_t pid = fork();
    if (pid == 0)
    {
        // child gets here
        execvp(args [0],args);

        exit(EXIT_FAILURE);
    }else if (pid== -1){
        printf("fail");
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }
    else
    {
        // father waiting to son to finish.
        waitpid(pid, &status, 0);
        // if sons exit status is 3 meaning files identical.
        if (WEXITSTATUS(status) == IDENTICAL) {
            writeResult(directoryName , "100", "GREAT_JOB");
            // if son exit status is 2 meaning files are similar.
        } else if (WEXITSTATUS(status) == SIMILAR) {
            writeResult(directoryName ,"80", "SIMILAR_OUTPUT");
            // otherwise files are different.
        } else if(WEXITSTATUS(status) == DIFFERENT) {
            writeResult(directoryName, "60", "BAD_OUTPUT");
        }
        unlink(outFile);
    }
}



bool runCFile (char*directoryName,char * inputFile){

    int status;
    char temp[SIZE];
    strcpy(temp,outFile);
    char *args[] = {"./a.out",NULL};
    pid_t pid = fork();
    if (pid == 0)
    {
        int out = open(outFile,O_APPEND|O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (out < 0){
            write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        }

        //output direction
        dup2(out, 1);
        int in = open(inputFile, O_RDONLY);
        if (in < 0){
            write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        }
        //input direction
        dup2(in, 0);

        //run the exe file
        execvp(args[0],args);

        close(in);
        close(out);
        //compile error exit with fail
        exit(EXIT_FAILURE);
    }else{
        sleep(5);
        // parent gets here
        pid_t wait_pid = waitpid(pid, &status, WNOHANG);
        if (wait_pid == 0){
            writeResult(directoryName, "0", "TIMEOUT");
            return false;
        }
        //add to the end of the outputFile new line
        char newLine[SIZE] = "\n";
        int out = open(outFile, O_APPEND|O_RDWR);
        if (out < 0){
            write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
            exit(EXIT_FAILURE);
        }
        if (write(out,newLine,strlen(newLine))<0){
            write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
            exit(EXIT_FAILURE);
        }
        //removes the run file.
        unlink("a.out");
        return true;
    }
}


void  strcatOfPath(char * first, char * second,char * dst){
    strcpy(dst,first);
    strcat(dst,"/");
    strcat(dst,second);
}

void writeResult(char *name , char* grade, char *reason) {
    int fd;
    char output [SIZE];
    snprintf(output, SIZE, "%s,%s,%s\n", name, grade, reason);
    if ((fd = open(RESULT_PATH, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR)) < 0){
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }
    if (write(fd, output, strlen(output))<0){
        write(FILE_DESCRIPTORS_STDERR, ERROR, strlen(ERROR));
        exit(EXIT_FAILURE);
    }
    printf("%s",output);
    close(fd);
}