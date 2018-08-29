#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <dirent.h>
#include <linux/limits.h>
#include <string.h>
#include <openssl/md5.h>
#include <sys/wait.h>
#include <sys/select.h>

#define MAXHEADER 500
#define PATH_MAX 4096
#define FORBMAX 40


int fileParser();
void cacher(char *buffer);
