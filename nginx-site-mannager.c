#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // chdir
#include <limits.h>   // PATH_MAX
#include <errno.h>

#define MAX_PATH_LEN 4096

void change_directory(char *path);

int main(void) {
    
    change_directory("/root");
    system("ls -la");

    return 0;
}


void change_directory(char *path){
    char destination_directory[MAX_PATH_LEN + 4];  // "cd " + espacio + path + '\0'
    int n = snprintf(destination_directory, sizeof(destination_directory), "cd %s", path);
    if (n < 0 || (size_t)n >= sizeof(destination_directory)) {
        fprintf(stderr, "Ruta demasiado larga\n");
    }


    if (chdir(path) != 0) {
        fprintf(stderr, "chdir('%s') falló: %s\n", path, strerror(errno)); 
    }
}