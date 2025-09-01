#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_PATH_LEN 4096

#define NGINX_DEFAULT_PATH "/etc/nginx/sites-enabled" 

void change_directory(char *path);
void list_files(char *path);
bool verify_nginx();
bool exists_file(char *name);

int main(void) {
    int option;
    change_directory(NGINX_DEFAULT_PATH);
    while(option != 5)
    {
        system("clear");
        printf("---------------------------------------------------\n");
        printf("NGINX SITE MANNAGER -> Adán Sanchez -> https://adansanchez.dev\n");
        printf("---------------------------------------------------\n\n");
        printf("\n1.- List sites");
        printf("\n2.- Create site");
        printf("\n3.- Update site");
        printf("\n4.- Delete site");
        printf("\nSelect an option:");
        scanf("%i", &option);

        switch (option)
        {
        case 1:
            list_files(NGINX_DEFAULT_PATH);
            break;
        
        default:
            break;
        }


        printf("---------------------------------------------------\n");
    }


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


bool exists_file(char *name)
{
    //Negates the value 'cause access returns 1 if file doesn't exists -> https://stackoverflow-com.translate.goog/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c?_x_tr_sl=en&_x_tr_tl=es&_x_tr_hl=es&_x_tr_pto=tc
    return !access(name, F_OK);
}

bool verify_nginx()
{
    system("nginx -t");

    return false;
}