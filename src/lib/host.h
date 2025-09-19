#ifndef HOST_H 
#define HOST_H

#define HOST_NAME_STRLEN 64
#define HOST_IP_STRLEN 16
#define HOST_USER_STRLEN 64

typedef struct {
    char name[HOST_NAME_STRLEN];
    char address[HOST_IP_STRLEN];
    char username[HOST_USER_STRLEN];
    int port;
} Host;

Host *hst_create(const char *name, const char *address, const char *username, int port);
Host *hst_load(const char *search, const char *filename);

int hst_save(const Host *host, const char *filename);
void hst_list(const char *filename);
void hst_free(Host *host);

#endif
