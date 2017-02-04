#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "sFile.h"

int
main(int argc, char **argv)
{

    char key[]={0x00,0x01,0x02,0x03,
                0x04,0x05,0x06,0x07,
                0x14,0x15,0x16,0x17,
                0xa4,0xa5,0xa6,0xa7};

    char buf[BlockSize];
    int i,res;

    if(argc!=3){
        fprintf(stderr,"usage %s <src> <dest>\n",argv[0]);
        return 0;
    }

    int src=open(argv[1],O_RDONLY,0);
    sFile *dst=sopen(argv[2],O_WRONLY,O_CREAT,key);

    while((res=read(src,buf,BlockSize))>0){
        swrite(dst,buf,res);
    }

    return 0;
}
