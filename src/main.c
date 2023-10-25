#include <server.h>
#include <io.h>

int main()
{
    server_t *server = sopen(8080);
    while(1)
    {
        swritef("./index.html", server);
    }
}