#ifdef VERBOSE
#include <stdio.h>   
#endif
#include <stdio.h>   

#include "sFile.h"

sFile *
sopen(const char *path, int flags, mode_t mode, char *key)
{

    char buf[32];
    int res;
    sFile *sf=malloc(sizeof(sFile));
	time_t t;
   	srand((unsigned) time(&t)); //Intializes random number generator
   	
    AES_set_encrypt_key(key,AESKeyLength,&(sf->key));
    AES_set_decrypt_key(key,AESKeyLength,&(sf->dkey));
    sf->ctr=malloc(sizeCTR);

    sf->fd=open(path,O_RDWR|(flags&O_TRUNC),mode);
    
    read(sf->fd,sf->ctr,sizeCTR);
    res=read(sf->fd,(char *)&(sf->rlen),sizeRLEN);
    if (res!=sizeRLEN){ /* new or truncated file */
        sf->rlen=0;
        {int i;
            //for(i=0;i<sizeCTR;i++)sf->ctr[i]=i;
            for(i=0;i<sizeCTR;i++)sf->ctr[i]=rand();
        }
        if(write(sf->fd,sf->ctr,sizeCTR)!=sizeCTR){
            return (sFile *)0;
        }
        if(write(sf->fd,(char *)&(sf->rlen),sizeRLEN)!=sizeRLEN){
            return (sFile *)0;
        }
    }
    sf->offset=sFileHeaderSize;
    lseek(sf->fd,sf->offset,SEEK_SET);
    return sf;
}
