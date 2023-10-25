#include <io.h>

/*
getfilesize: gets size of file
@filename: name of file whose size we need
returns size of file if it exists, otherwise fail
*/
long getfilesize(const char *filename)
{
    //Length of file
    long length = 0;
    //Open file for byte reading
    FILE *file = fopen(filename, "rb");
    //Check if we failed to open the file
    if(!file)
    {
        //We failed to open the file, print error and return fail
        printf("getfilesize: failed to open file %s\n", filename);
        perror("getfilesize");
        return FAIL;
    }

    //Seek to end of file
    fseek(file, 0L, SEEK_END);
    //Get length of file
    length = ftell(file);
    //Rewind to beginning
    rewind(file);
    //Close the file
    fclose(file);

    //And return length
    return length;
}

/*
readfile: reads content from file
@filename: file to be read
returns file content if file exists, otherwise null
*/
char *readfile(const char *filename)
{
    //Get size of file
    long size = getfilesize(filename);
    //Content from file
    char *content = (char *) malloc(size * sizeof(char));
    //Open file for byte reading
    FILE *file = fopen(filename, "rb");
    //Check if we failed to open file
    if(!file)
    {
        //We failed to open file, print error and return null
        printf("readfile: failed to read file %s\n", filename);
        perror("readfile");
        return NULL;
    }

    //Read the file's content
    fread(content, size, 1, file);
    //Close the file
    fclose(file);
    //And return content
    return content;
}