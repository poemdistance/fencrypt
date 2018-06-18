#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

/*default buffer length*/
#define DEF_BUFLEN 1024*1024

void start(char *purpose)
{
    char ch;
    char sourceName[256];
    char outputName[256];
    FILE *fsource, *foutput;

    while(1)
    {
        printf("Please input source file:\n>> ");
        scanf("%s", sourceName);
        if(access(sourceName, F_OK) == 0)
            break;
        else
        {
            printf("File doesn't exist\n");
            continue;
        }
    }

    while(1)
    {
        printf("Please input the output file:\n>> ");
        scanf("%s", outputName);
        getchar(); 
        if(strcmp(outputName,sourceName) == 0)
        {
            fprintf(stderr, "Please don't input the same name as source file :(\n");
            continue;
        }
        if(access(outputName, F_OK) == 0)
        {
            printf("File exist, o(overwrite)/r(rename)/q(quit)? : ");
            scanf("%c", &ch);
            if(ch == 'o')
                break;
            else if(ch == 'r')
                continue;
            else if(ch == 'q')
                exit(1);
        }
        break;
    }

    char *p;
    struct stat st;
    int loop = 1; /*loop times*/

    int space = DEF_BUFLEN;

    stat(sourceName, &st);

    /*if the file is so large, just allocate limited memery(1G) to it*/
    if(st.st_size > space)
    {
        loop = st.st_size / space + 1;
        if(st.st_size % space == 0)
            loop--;

        p = (char *)calloc(space,sizeof(char));
    }

    /*else allocate the equal space to it*/
    else
    {
        space = st.st_size;
        p = (char *)calloc(space,sizeof(char));
    }

    fsource = fopen(sourceName, "rb");
    assert(fsource != NULL);

    foutput = fopen(outputName, "wb");
    assert(foutput != NULL);

    char *temp = p;
    int length;
    int wlen, rlen;

    while(1)
    {
        if((rlen = fread(p, sizeof(char), space, fsource)) <= 0)
        {
            fprintf(stderr, "fread error\n");
            exit(1);
        }
        length = rlen;
        if(strcmp(purpose, "encrypt") == 0)
            while(rlen--)
                *p++ = (*p - 23) ^ 26;	//encrypt the file
        else
            while(rlen--)
                *p++ = (*p ^ 26) + 23;	//decrypt

        printf("read len = %d\tlen", length);

        if((wlen = fwrite(temp, sizeof(char), length, foutput)) != length)
        {
            fprintf(stderr, "fwrite error, length=%d, loop=%d,\
                    writelen=%d\n", length, loop, wlen);
            exit(1);
        }
        printf("write len = %d\n", wlen);
        p = temp;

        loop--;
        if(loop == 0)
            break;

        memset(temp, 0, sizeof(char) * space);	//fill the memery with 0
    }

    printf("%s successful :)\n", purpose);
    free(temp);
    fclose(fsource);
    fclose(foutput);
}

int main(int argc, char **argv)
{
    void encrypt();
    void decrypt();

    int choice = 0;

#ifdef _WIN32
    printf("Please input your choice:\n1:encrypt\t2:decrypt\n");
    scanf("%d", &choice);
#elif __linux__

    if(argc < 2)
    {
        fprintf(stderr, "USAGE: './main -e' or './main -d'\n");
        exit(1);
    }
    if(strcmp(argv[1], "-e") == 0)
        choice = 1;

    if(strcmp(argv[1], "-d") == 0)
        choice = 2;
#endif
    switch (choice)
    {
        case 1:  start("encrypt"); break;
        case 2:  start("decrypt"); break;
        default: 
                 printf("Input error\n");
    }
    return 0;
}

