#ifdef VERBOSE
#include <stdio.h>
#endif

#include "sFile.h"

int 
swrite(sFile *sf, const void *bbuf, size_t count) 
{

    unsigned char fromDisk[BlockSize];
    unsigned char clearFromDisk[BlockSize];
    char *buf=(char *)bbuf;

    int res, ok, i;
    int  headLen, headDest, tailLen, tailDest, bodyLen, bodyDest;
    long long headBlock, tailBlock, bodyBlock;
    char *headStart, *tailStart, *bodyStart;

#ifdef VERBOSE
    printf("****Starting a write op with offset=%d ",(int)sf->offset);
    {int i;
     printf("plaintext: \n\t");
     for(i=0;i<count;i++) printf("%02x ",*((unsigned char *)(buf+i)));
     printf("\nCount: %d\n",count);
    }
#endif

    headBlock=(long long)((sf->offset>>4)<<4);      /* starting offset of the head block */
    headDest=sf->offset%16;            /* offset of the head within the head block */
    headLen=16-headDest;
    if (headLen>count) headLen=count;  /* number of bytes in the head */
    headStart=buf+0;                   /* chars from the head start at buf+0 */

#ifdef VERBOSE
    printf("****Head: \n\t");
    printf("headBlock: %lld\theadDest: %d\theadLen: %d\theadStart: %d\n",headBlock,headDest,headLen,headStart-buf);
#endif

    tailLen=(count-headLen)%16;             /* number of bytes in the tail */
    tailBlock=(long long)(((sf->offset+count)>>4)<<4);   /* starting offset of the tail block */
    tailDest=0;                             /* offset of the tail within the tail block */
    tailStart=buf+count-tailLen;            /* ptr to first char of the tail in buf */

#ifdef VERBOSE
    printf("****Tail: \n\t");
    printf("tailBlock: %lld\ttailDest: %d\ttailLen: %d\ttailStart: %d\n",tailBlock,tailDest,tailLen,tailStart-buf);
#endif

    bodyLen=count-headLen-tailLen;         /* number of bytes in the body */
    bodyBlock=headBlock+BlockSize;         /* starting offset of the first block of the body */
    bodyDest=0;                            /* offset of the body within the first body block */
    bodyStart=buf+headLen;                 /* ptr to first char of the tail in buf */

#ifdef VERBOSE
    printf("****Body: \n\t");
    printf("bodyBlock: %lld\tbodyDest: %d\tbodyLen: %d\tbodyStart: %d\n",bodyBlock,bodyDest,bodyLen,bodyStart-buf);
#endif

    if (count==0) goto FINISH;

#ifdef VERBOSE
    printf("\n\nWriting the head:\n");
#endif

    res=readBlock(sf,clearFromDisk,&headBlock);
    if(res==0) memset(clearFromDisk,0,BlockSize);
#ifdef VERBOSE
    {int i;
     printf("plaintext from disk: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ", (unsigned char) clearFromDisk[i]);
     printf("\n");
    }
#endif
    memcpy(clearFromDisk+headDest,headStart,headLen);
#ifdef VERBOSE
    {int i;
     printf("dopo memcpy: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ", (unsigned char) clearFromDisk[i]);
     printf("\n");
    }
#endif
    writeBlock(sf,clearFromDisk,&headBlock);

    if (headLen==count) goto FINISH;    /* nothing left to write */
    
    while(bodyLen>0){
#ifdef VERBOSE
    {int i;
     printf("\n\nin body writing plaintext: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ", (unsigned char) bodyStart[i]);
     printf("\n");
    }
#endif
        writeBlock(sf,bodyStart,&bodyBlock); //check
        bodyLen-=BlockSize;
        bodyStart+=BlockSize;
        bodyBlock+=BlockSize;
    }

    if (tailLen==0) goto FINISH;

#ifdef VERBOSE
    printf("\n\nWriting the tail:\n");
#endif
	memset(clearFromDisk,0,BlockSize); //check
    readBlock(sf,clearFromDisk,&tailBlock);          /* read the whole block */

#ifdef VERBOSE
    {int i;
     printf("decrypted bytes: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ", (unsigned char) clearFromDisk[i]);
     printf("\n");
    }
#endif

    memcpy(clearFromDisk+tailDest,tailStart,tailLen);

#ifdef VERBOSE
    {int i;
     printf("dopo memcpy: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ", (unsigned char) clearFromDisk[i]);
     printf("\n");
    }
#endif

    writeBlock(sf,clearFromDisk,&tailBlock);

FINISH:
    sf->offset+=count;
    if(sf->offset>sf->rlen){
        sf->rlen=sf->offset;
        lseek(sf->fd,sFileStartRLEN,SEEK_SET);
        write(sf->fd,(char *)&(sf->rlen),sizeof(long long));
    }

#ifdef VERBOSE
printf("****End of write\n\n");
#endif

    return count;
}
