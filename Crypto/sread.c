#ifdef VERBOSE
#include <stdio.h>
#endif

#include "sFile.h"


int 
sread(sFile *sf, void *buf, size_t count) 
{

    unsigned char fromDisk[BlockSize], clearFromDisk[BlockSize];
    int bytesRead;

    int res, ok, i;
    int  headLen, headSrc, tailLen, tailSrc, bodyLen, bodySrc;
    long long  headBlock, tailBlock, bodyBlock;
    char *headBuf, *tailBuf, *bodyBuf;

#ifdef VERBOSE
    printf("****Starting a read op with offset=%d and count %d\n",(int)sf->offset,count);
#endif

    memset(buf,0,count);
    if (sf->offset>sf->rlen){bytesRead=0; goto FINISH;}

    headBlock=(long long)((sf->offset>>4)<<4);      /* starting offset of the head block */
    headSrc=sf->offset%16;             /* offset of the head within the head block */
    headLen=16-headSrc;
    if (headLen>count) headLen=count;  /* number of bytes in the head */
    headBuf=buf+0;                     /* chars from the head start at buf+0 */

#ifdef VERBOSE
    printf("****Head: \n\t");
    printf("headBlock: %lld\theadSrc: %d\theadLen: %d\theadBuf: %d\n",headBlock,headSrc,headLen,headBuf-(char *)buf);
#endif

    tailBlock=(long long) (((sf->offset+count)>>4)<<4); /* starting offset of the tail block */
    tailLen=(count-headLen)%16;           /* number of bytes in the tail */
    tailSrc=0;                            /* offset of the tail within the tail block */
    tailBuf=buf+count-tailLen;            /* ptr to first char of the tail in buf */

#ifdef VERBOSE
    printf("****Tail: \n\t");
    printf("tailBlock: %lld\ttailSrc: %d\ttailLen: %d\ttailBuf: %d\n",tailBlock,tailSrc,tailLen,tailBuf-(char *)buf);
#endif

    bodyBlock=headBlock+BlockSize;   /* starting offset of the first block of the body */
    bodyLen=count-headLen-tailLen;   /* number of bytes in the body */
    bodySrc=0;                       /* offset of the body within the body block */
    bodyBuf=buf+headLen;             /* ptr to first char of the body in buf */
    
#ifdef VERBOSE
    printf("****Body: \n\t");
    printf("bodyBlock: %lld\tbodySrc: %d\tbodyLen: %d\tbodyBuf: %d\n",bodyBlock,bodySrc,bodyLen,bodyBuf-(char *)buf);
#endif


#ifdef VERBOSE
    printf("\n\nStarting from the head\n");
#endif


    res=readBlock(sf,clearFromDisk,&headBlock);
    if (res==0){bytesRead=0; goto FINISH;} /* should never happen*/

#ifdef VERBOSE
    {int i;
     printf("plaintext: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ",(unsigned char)clearFromDisk[i]);
     printf("\n");
    }
#endif

    memcpy(headBuf,clearFromDisk+headSrc,headLen);
    if(sf->offset+headLen>sf->rlen){
    	bytesRead=sf->rlen-sf->offset;
        goto FINISH;
    }
    else
        bytesRead=headLen;
    
    if (headLen==count) goto FINISH;
#ifdef VERBOSE
    printf("\n\ncontinuing with the body\n");
#endif

    while(bodyLen>0){
        res=readBlock(sf,clearFromDisk,&bodyBlock);
        if (res==0) goto FINISH;
#ifdef VERBOSE
    {int i;
     printf("plaintext: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ",(unsigned char)clearFromDisk[i]);
     printf("\n");
    }
#endif
        memcpy(bodyBuf,clearFromDisk+bodySrc,BlockSize);
        if(bodyBlock+BlockSize>sf->rlen){
            bytesRead+=sf->rlen-bodyBlock;
            goto FINISH;
        }
        else
            bytesRead+=BlockSize;
        bodyLen-=BlockSize; bodyBuf+=BlockSize;
        bodyBlock+=BlockSize;
    }

#ifdef VERBOSE
    printf("\n\nand finishing with the tail\n");
#endif

    if (tailLen==0) goto FINISH;

    res=readBlock(sf,clearFromDisk,&bodyBlock);
    if (res==0) goto FINISH;
#ifdef VERBOSE
    {int i;
     printf("plaintext: \n\t");
     for(i=0;i<BlockSize;i++) printf("%02x ",(unsigned char)clearFromDisk[i]);
     printf("\n");
    }
#endif
    memcpy(tailBuf,clearFromDisk+tailSrc,tailLen);
    if(tailBlock+tailLen>sf->rlen) //check
    	bytesRead=sf->rlen-sf->offset;
   	else
   		bytesRead+=tailLen;

FINISH:
#ifdef VERBOSE
printf("****End of read\n\n");
#endif
    sf->offset+=bytesRead;
    return bytesRead;

}
