#ifndef _SERVER_H_
#define _SERVER_H_
#define FAIL -1
#define SUCCESS 0

#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>

typedef struct s_server server_t;
struct s_server
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    int server_socket;
    int client_socket;

    int op_res;
    void *data;
    size_t data_size;
};


/*
sopen: opens server on given port
@port: port to also serve on
returns new server instance
*/
server_t *sopen(int port);

/*
swritet: threaded function that runs write operations on client
@arg: argument passed to function (is server instance)
returns size of written bytes
*/
void *swritet(void *arg);

/*
swrite: writes data to client
@data: data to be sent to client
@size: size of data parameter
@server: server instance
returns number of elements written if successful, -1 if failed to write
*/
int swrite(void *data, size_t size, server_t *server);
/*
swritef: writes file data to client
@filename: file whose data we want to read
@server: server instance
returns number of elements written if successful, -1 if failed to write or failed to read file
*/
int swritef(const char *filename, server_t *server);

/*
sreadt: threaded function that reads data from client
@arg: function argument, is server instance
returns data received from client
*/
void *sreadt(void *arg);

/*
sread: reads data from client
@server: server instance
returns input from client, null if issues occurred
*/
void *sread(server_t *server);
#endif