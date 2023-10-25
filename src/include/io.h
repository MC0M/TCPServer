#ifndef _IO_H_
#define _IO_H_
#include <server.h>

/*
getfilesize: gets size of file
@filename: name of file whose size we need
returns size of file if it exists, otherwise fail
*/
long getfilesize(const char *filename);
/*
readfile: reads content from file
@filename: file to be read
returns file content if file exists, otherwise null
*/
char *readfile(const char *filename);
#endif