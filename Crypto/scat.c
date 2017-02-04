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

    char buf[BlockSize+1];
    int i,res;

    if(argc!=2){
        fprintf(stderr,"usage %s <filename> \n",argv[0]);
        return 0;
    }

    sFile *xy=sopen(argv[1],O_RDONLY,0,key);
    while((res=sread(xy,buf,BlockSize))>0){
        buf[res]=0;
        printf("%s",buf);
    }

    return 0;
}
