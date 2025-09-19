#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lib/host.h"

#define HOSTS_FILE_PATH "storage/hosts.txt"

int save(char *argv[])
{
    const char *name = argv[2];
    const char *address = argv[3];
    const char *username = argv[4];
    int port = atoi(argv[5]);
    
    Host *host = hst_load(name, HOSTS_FILE_PATH);
    if (host) {
        fprintf(stderr, "Unable to save, entry: \"%s\" already exists.\n", name);
        hst_free(host);
        return 1;
    }
    
    host = hst_create(name, address, username, port);
    if (!host) return 1;
    if (hst_save(host, HOSTS_FILE_PATH) == 0)
        printf("Host '%s' successfully saved in %s\n", name, HOSTS_FILE_PATH);
    
    hst_free(host);
    return 0;
}

int connect(char *argv[])
{
    const char *name = argv[2];
    Host *host = hst_load(name, HOSTS_FILE_PATH);
    if (!host) {
        fprintf(stderr, "Unable to connect, entry \"%s\" not found.\n", name);
        return 1;
    }

    char port[16];
    snprintf(port, sizeof(port), "%d", host->port);

    char *ssh_args[] = {
        "ssh",
        "-p", port,
        "-l", host->username,
        host->address,
        NULL
    };

    printf("Connecting to %s@%s:%d...\n", host->username, host->address, host->port);
    execvp("ssh", ssh_args);

    perror("Unable to invoke ssh via execvp.");
    hst_free(host);
    return 1;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        fprintf(stderr, "Commands:\n");
        fprintf(stderr, " - save <name> <address> <username> <port>\n");
        fprintf(stderr, " - connect <name>\n");
        return 1;
    }
    
    const char *command = argv[1];
    if (strcmp(command, "save") == 0) {
        if (argc < 6) {
            fprintf(stderr, "Usage: %s save <name> <address> <username> <port>\n", argv[0]);
            return 1;
        }
        
        return save(argv);
    } else if (strcmp(command, "connect") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s connect <name>\n", argv[0]);
            return 1;
        }
        
        return connect(argv);
    }
    
    return 0;
}
