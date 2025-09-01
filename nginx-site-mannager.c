#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <limits.h>

#define MAX_PATH_LEN 4096
#define INPUT_MAX 256
#define NGINX_DEFAULT_PATH "/etc/nginx/sites-available"
#define NGINX_DEFAULT_PATH_SYMLINK "/etc/nginx/sites-enabled"
#define CREATE_SITE_SCRIPT "/root/C_projects/nginx-cli/create-site.sh"
#define CURRENT_DIRECTORY "/root/C_projects/"

void change_directory(const char *path);
void list_files(const char *path);
bool verify_nginx(void);
bool exists_file(const char *name);
void wait_enter(void);
int  read_input(void);
int  execute_bash_create_site(const char *script, const char *site, const char *port);
void create_site(void);
void remove_file();
void certify_site();
char *get_input_string(const char *question);


int main(void) {
    int option = 0;
    change_directory(NGINX_DEFAULT_PATH);

    while (option != 5) {
        system("clear");
        printf("---------------------------------------------------\n");
        printf("NGINX SITE MANAGER <-> Adán Sanchez <-> https://adansanchez.dev\n");
        printf("---------------------------------------------------\n\n");
        printf("1.- List sites\n");
        printf("2.- Create site\n");
        printf("3.- Certify site -> Certbot\n");
        printf("4.- Update site\n");
        printf("5.- Delete site\n");
        printf("9.- Exit\n");
        printf("Select an option: ");
        option = read_input();

        switch (option) {
            case 1:
                list_files(NGINX_DEFAULT_PATH);
                break;
            case 2:
                create_site();
                break;
            case 3:
                // TODO: implementar
                wait_enter();
                break;
            case 4:
                //certbot --nginx -d $site
                break;
            case 5:
                remove_file();
                break;
            default:
                printf("Invalid option.\n");
                wait_enter();
                break;
        }
        wait_enter();
        printf("---------------------------------------------------\n");
    }
    return 0;
}

void change_directory(const char *path){
    char destination_directory[MAX_PATH_LEN + 4]; 
    int n = snprintf(destination_directory, sizeof(destination_directory), "cd %s", path); 
    
    if (n < 0 || (size_t)n >= sizeof(destination_directory)) 
        fprintf(stderr, "Route too long\n"); 
    
    if (chdir(path) != 0)
        fprintf(stderr, "chdir('%s') failed: %s\n", path, strerror(errno)); 
    
    if (chdir(path) != 0) 
        fprintf(stderr, "chdir('%s') failed: %s\n", path, strerror(errno));
    
}

void list_files(const char *path){
    system("clear");
    printf("--- SITES ---\n");
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Couldn't open the directory");
        return;
    }

    struct dirent *de;
    int counter = 0;
    while ((de = readdir(dir)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        printf("%d.- %s\n", ++counter, de->d_name);
    }
    if (counter == 0) printf("(empty)\n");

    closedir(dir);
}

bool exists_file(const char *name) {
    return access(name, F_OK) == 0;
}

bool verify_nginx(void) {
    return system("nginx -t") == 0;
}

void wait_enter(void) {
    printf("\nPress ENTER to continue...");
    fflush(stdout);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

int read_input(void) {
    char line[64];
    if (!fgets(line, sizeof line, stdin)) return 0;
    char *end;
    long v = strtol(line, &end, 10);
    if (end == line) return 0; 
    if (v < INT_MIN) v = INT_MIN;
    if (v > INT_MAX) v = INT_MAX;
    return (int)v;
}

void create_site(void)
{
    system("clear");
    char *site = get_input_string("Site name: ");
    char *port_str=get_input_string("Port: ");

    if(exists_file(site)){
        printf("There's already a site with this name");
        return;
    };

    char *end;
    long port = strtol(port_str, &end, 10);
    if (*end != '\0' || port < 1 || port > 65535) {
        printf("Invalid port: %s\n", port_str);
        return;
    }

    printf("%s:%ld\n", site, port);

    int rc = execute_bash_create_site(CREATE_SITE_SCRIPT, site, port_str);
    if (rc == 0) {
        puts("Site created successfully.");
        if (!verify_nginx()) {
            puts("Warning: nginx -t failed.");
        }
    } else 
        printf("Script failed with code %d\n", rc);
    

}

int execute_bash_create_site(const char *script, const char *site, const char *port) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        char * const argv[] = { "bash", (char*)script, (char*)site, (char*)port, NULL };
        execvp("bash", argv);
        perror("execvp");
        _exit(127);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return -1;
}


void remove_file(){
    char symlink[600];
    list_files(NGINX_DEFAULT_PATH);
    
    char *site = get_input_string("Site to delete: ");
    
    snprintf(symlink,sizeof(symlink), "%s%s", NGINX_DEFAULT_PATH_SYMLINK, site);

    if(exists_file(site)) 
        remove(site);
    
    if(exists_file(symlink)) 
        remove(symlink);
    
    
    printf("\nSite deleted");

    return;
}


void certify_site()
{
    list_files(NGINX_DEFAULT_PATH);

    char *site = get_input_string("Which site would you want to certify: ");
    if (!site || !*site) { free(site); return; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); free(site); return; }
    if (pid == 0) {
        // certbot --nginx -d <site>
        char * const argv[] = { "certbot", "--nginx", "-d", site, NULL };
        execvp("certbot", argv);
        perror("execvp");
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) perror("waitpid");
    free(site);
}

char *get_input_string(const char *question)
{
    char *stringInput = malloc(INPUT_MAX);

    printf("%s", question);
    
    if (!fgets(stringInput, sizeof stringInput, stdin)){
        free(stringInput);
        return NULL;
    }

    stringInput[strcspn(stringInput, "\n")] = '\0';
    

    return stringInput;
} 