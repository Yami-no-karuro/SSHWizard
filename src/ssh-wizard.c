#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lib/host.h"

int save(char *argv[], char *hosts)
{
    const char *name = argv[2];
    const char *address = argv[3];
    const char *username = argv[4];
    int port = atoi(argv[5]);
    
    Host *host = hst_load(name, hosts);
    if (host) {
        fprintf(stderr, "Exiting, entry: \"%s\" already exists.\n", name);
        hst_free(host);
        return 1;
    }
    
    host = hst_create(name, address, username, port);
    if (!host) {
        fprintf(stderr, "Exiting, an unexpected error occurred.\n");
        return 1;
    }
    
    if (hst_save(host, hosts) == 0) {
        printf("Host '%s' successfully saved in %s\n", name, hosts);
    } else {
        fprintf(stderr, "Exiting, an unexpected error occurred.\n");
        hst_free(host);
        return 1;
    }
    
    hst_free(host);
    return 0;
}

int connect(char *argv[], char *hosts)
{
    const char *name = argv[2];
    Host *host = hst_load(name, hosts);
    if (!host) {
        fprintf(stderr, "Exiting, entry \"%s\" not found.\n", name);
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

    printf("Connecting to: \"%s:%d\" as: \"%s\".\n", host->address, host->port, host->username);
    execvp("ssh", ssh_args);

    fprintf(stderr, "Exiting, an unexpected error occurred.\n");
    hst_free(host);
    return 1;
}

int list(char *hosts)
{
    hst_list(hosts);
    return 0;
}

int main(int argc, char *argv[]) 
{
    const char *home = getenv("HOME");
    char hosts[512];
    snprintf(hosts, sizeof(hosts), "%s/.ssh-wizard", home);
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        fprintf(stderr, "Commands:\n");
        fprintf(stderr, " - list\n");
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
        
        return save(argv, hosts);
    } else if (strcmp(command, "connect") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s connect <name>\n", argv[0]);
            return 1;
        }
        
        return connect(argv, hosts);
    } else if (strcmp(command, "list") == 0) {
        return list(hosts);
    }
    
    return 0;
}
