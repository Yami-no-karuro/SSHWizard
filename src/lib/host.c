#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host.h"

Host *hst_create(const char *name, const char *address, const char *username, int port)
{
    Host *host = malloc(sizeof(Host));
    if (!host) {
        perror("Unable to allocate enough memory for a Host instance.");
        return NULL;
    }
    
    strcpy(host->name, name);
    strcpy(host->address, address);
    strcpy(host->username, username);
    host->port = port;
    return host;
}

Host *hst_load(const char *search, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open hosts file in read mode.");
        return NULL;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        
        char name[HOST_NAME_STRLEN];
        char address[HOST_IP_STRLEN];
        char username[HOST_USER_STRLEN];
        int port;
        
        if (sscanf(line, "%63[^,],%15[^,],%63[^,],%d", name, address, username, &port) == 4) {
            if (strcmp(name, search) == 0) {
                fclose(file);
                return hst_create(name, address, username, port);
            }
        }
    }
   
    fclose(file);
    return NULL;
}

int hst_save(const Host *host, const char *filename) 
{
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Unable to open hosts file in append mode.");
        return -1;
    }
    
    fprintf(file, "%s,%s,%s,%d\n", host->name, host->address, host->username, host->port);
    fclose(file);
    return 0;
}

void hst_free(Host *host)
{
    free(host);
}
