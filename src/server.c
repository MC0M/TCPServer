#include <server.h>
#include <io.h>
/*
sopen: opens server on given port
@port: port to also serve on
returns new server instance if successful, null otherwise
*/
server_t *sopen(int port)
{
    //Allocate space for current server instance
    server_t *server = (server_t *) malloc(sizeof(server_t));
    //Create our server socket
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&server->server_addr, sizeof(server->server_addr));

    //Prepare server address
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->server_addr.sin_port = htons(port);

    //Allow address and port reusage
    if(setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0 || 
        setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
    {
        //An issue prevented us from allowing the reuse of address and port, print error and return null.
        printf("sopen: failed to open server on port %i\n", port);
        perror("sopen");
        return NULL;
    }

    //Bind address to socket
    if((bind(server->server_socket, (struct sockaddr *) &server->server_addr, sizeof(server->server_addr))) != 0)
    {
        //An issue caused a failure to bind, print error and return null.
        printf("sopen: failed to open server on port %i\n", port);
        perror("sopen");
        return NULL;
    }
    
    //Print that a server is running on given port
    printf("Server running on localhost:%i\n", port);
    printf("Listening for connections...\n");
    
    //Listen for connection from client
    if((listen(server->server_socket, 5)) != 0)
    {
        //Some error happened, and thus failed to listen. print error and return null.
        printf("sopen: failed to open server on port %i\n", port);
        perror("sopen");
        return NULL;
    }

    //Return new server instance
    return server;
}

/*
swritet: threaded function that runs write operations on client
@arg: argument passed to function (is server instance)
returns size of written bytes
*/
void *swritet(void *arg)
{
    //Convert argument to server
    server_t *server = (server_t *) arg;

    //Detach current thread
    pthread_detach(pthread_self());

    //Write data to client
    int write_res = write(server->client_socket, server->data, server->data_size);

    //Set operation results to write results
    server->op_res = write_res;

    //And return write results
    pthread_exit(NULL);
}

/*
swrite: writes data to client
@data: data to be sent to client
@size: size of data parameter
@server: server instance
returns number of elements written if successful, -1 if failed to write
*/
int swrite(void *data, size_t size, server_t *server)
{
    //Our thread ID
    pthread_t thread_id;

    //Get size of client address
    int length = sizeof(server->client_addr);
    //Accept connection from client
    server->client_socket = accept(server->server_socket, (struct sockaddr *) &server->client_addr, &length);
    if(server->client_socket < 0)
    {
        printf("swrite: failed to write data to client.\n");
        perror("swrite");
        return FAIL;
    }

    //Write data to client and get if write was successful
    server->data = data;
    server->data_size = size;
    //Create a thread for our swritet function
    pthread_create(&thread_id, NULL, swritet, (void *) server);
    //Run our thread
    pthread_join(thread_id, NULL);

    //Close client connection
    close(server->client_socket);

    //Finally, return write results (the results of write operation return -1 if an issue happened)
    return server->op_res;
}

/*
swritef: writes file data to client
@filename: file whose data we want to read
@server: server instance
*/
int swritef(const char *filename, server_t *server)
{
    //Read the file
    char *content = readfile(filename);
    //Check if file was not read
    if(!content)
    {
        //Print that we could not write to client
        printf("swritef: failed to write file to client.\n");
        perror("swritef");
        return FAIL;
    }
    //Send the file's content to client
    return swrite(content, getfilesize(filename), server);
}


/*
sreadt: threaded function that reads data from client
@arg: function argument, is server instance
returns data received from client
*/
void *sreadt(void *arg)
{
    //Extract server instance from argument parameter
    server_t *server = (server_t *) arg;
    //Read data from client
    read(server->client_socket, server->data, server->data_size);
    //And exit this thread
    pthread_exit(NULL);
}

/*
sread: reads data from client
@server: server instance
returns input from client, null if issues occurred
*/
void *sread(server_t *server)
{
    //Thread id for sreadt
    pthread_t thread_id;
    //Get size of client address
    int length = sizeof(server->client_addr);
    
    //Check if we have not accepted a connection from a client
    if(server->client_socket == 0)
        //Accept connection from client
        server->client_socket = accept(server->server_socket, (struct sockaddr *) &server->client_addr, &length);
    //Make sure server is not null
    if(!server)
    {
        //Server is null, print error and return null
        printf("sread: failed to get input from client. Server was not set.\n");
        return NULL;
    }

    //Create a thread for our sreadt function
    pthread_create(&thread_id, NULL, sreadt, (void *) server);
    
    //Run our thread
    pthread_join(thread_id, NULL);

    //Return data received from  client
    return server->data;
}