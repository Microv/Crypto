#ifdef VERBOSE
#include <stdio.h>
#endif
#include "sFile.h"

int 
writeBlock(sFile *sf, char *buf, long long *blockOffset)
{

    unsigned char tmpCiph[BlockSize], tmpPlain[BlockSize];
    unsigned char toDisk[BlockSize];
    int i, res;

    for(i=0;i<sizeof(long long);i++)
        tmpPlain[i]=sf->ctr[i]^((char *)blockOffset)[i];
    for(i=sizeof(long long);i<BlockSize;i++)
        tmpPlain[i]=sf->ctr[i];

#ifdef VERBOSE
    {int i;
     printf("wB tmpPlain: (%lld)\n\t",*blockOffset);
     for(i=0;i<BlockSize;i++) printf("%02x ",(unsigned char)tmpPlain[i]);
     printf("\n");
    }
#endif

    AES_ecb_encrypt(tmpPlain,tmpCiph,&(sf->key),AES_ENCRYPT); 
#ifdef VERBOSE
    {int i;
     printf("wB tmpCiph: (%lld)\n\t",*blockOffset);
     for(i=0;i<BlockSize;i++) printf("%02x ",(unsigned char)tmpCiph[i]);
     printf("\n");
    }
#endif
    for(i=0;i<BlockSize;i++)
        toDisk[i]=buf[i]^tmpCiph[i];

#ifdef VERBOSE
    {int i;
     printf("wB to Disk: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ",(unsigned char)toDisk[i]);
     printf("\n");
    }
#endif

    lseek(sf->fd,*blockOffset,SEEK_SET);
    res=write(sf->fd,toDisk,BlockSize);
   
    return res;
}


int 
readBlock(sFile *sf, char *buf, long long *blockOffset)
{

    unsigned char tmpCiph[BlockSize], tmpPlain[BlockSize];
    unsigned char fromDisk[BlockSize];
    
    
    int i, res, ok;

    lseek(sf->fd,*blockOffset,SEEK_SET);
    res=read(sf->fd,fromDisk,BlockSize);
	memset(buf,0,BlockSize);  //check
	
#ifdef VERBOSE
    {int i;
     printf("rB fromDisk (%d): \n\t",res);
     for(i=0;i<res;i++) printf("%02x ",(unsigned char)fromDisk[i]);
     printf("\n");
    }
#endif
	
    if (res==0) return 0; 

    for(ok=0,i=0;i<BlockSize;i++) if(fromDisk[i]!=0){ok=1;break;}
    if(!ok){return BlockSize;}

#ifdef VERBOSE
     printf("rB ok %d: \n\t",ok);
#endif

    for(i=0;i<sizeof(long long);i++)
        tmpPlain[i]=sf->ctr[i]^((char *)blockOffset)[i];
    for(i=sizeof(long long);i<BlockSize;i++)
        tmpPlain[i]=sf->ctr[i];
#ifdef VERBOSE
    {int i;
     printf("rB tmpPlain: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ",(unsigned char)tmpPlain[i]);
     printf("\n");
    }
#endif
    AES_ecb_encrypt(tmpPlain,tmpCiph,&(sf->key),AES_ENCRYPT); 
#ifdef VERBOSE
    {int i;
     printf("rB tmpCiph (%lld): \n\t",*blockOffset);
     for(i=0;i<BlockSize;i++) printf("%02x ",(unsigned char)tmpCiph[i]);
     printf("\n");
    }
#endif
    for(i=0;i<BlockSize;i++) buf[i]=tmpCiph[i]^fromDisk[i];
    
    return BlockSize;

}
