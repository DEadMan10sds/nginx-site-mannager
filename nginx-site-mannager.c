#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>

#define MAX_PATH_LEN 4096

#define NGINX_DEFAULT_PATH "/etc/nginx/sites-enabled" 

void change_directory(char *path);
void list_files(char *path);

int main(void) {
    list_files(NGINX_DEFAULT_PATH);

    return 0;
}


void change_directory(char *path){
    char destination_directory[MAX_PATH_LEN + 4]; 
    int n = snprintf(destination_directory, sizeof(destination_directory), "cd %s", path);
    
    if (n < 0 || (size_t)n >= sizeof(destination_directory)) {
        fprintf(stderr, "Route too long\n");
    }

    if (chdir(path) != 0) {
        fprintf(stderr, "chdir('%s') failed: %s\n", path, strerror(errno)); 
    }
}

void list_files(char *path){
    int counter = 0;
    struct dirent *current_directory;
    DIR *directories = opendir(path);

    if(!directories)
       printf("Couldn't open the directory");
    
    while ((current_directory = readdir(directories)))
    {
        if(!strcmp(current_directory->d_name, ".") || !strcmp(current_directory->d_name,"..")) 
            continue;

        counter++;
        printf("%i.-  %s\n", counter, current_directory->d_name);
    }
    
    closedir(directories);
}