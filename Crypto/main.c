#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "sFile.h"

int
main()
{

    char key[]={0x00,0x01,0x02,0x03,
                0x04,0x05,0x06,0x07,
                0x14,0x15,0x16,0x17,
                0xa4,0xa5,0xa6,0xa7};

    char buf[100];
    int i,res;

    sFile *x=sopen("pippo.txt",O_WRONLY,0,key);
    /*swrite(x,"X",1);
    swrite(x,"X",1);
    swrite(x,"23456789ABCDEFG",15);
    slseek(x,45,SEEK_SET);
    swrite(x,"0123456789ABCDEF0123456789ABCDEFX",33);
    close(x->fd);

    x=sopen("pippo.txt",O_RDONLY,0,key);
    res=sread(x,buf,90);
    printf("Read %d bytes\n",res);
    for(i=0;i<res;i++) printf(" %02x",(unsigned char)buf[i]);
    printf("\n");
	*/
	// begin test
	
	slseek(x,45,SEEK_SET);
    swrite(x,"1023456789ABCDEFGHIJKLMNOPQRSTUVWXYZmm",36);    
    close(x->fd);
	
	printf("\n\n");
	
	x=sopen("pippo.txt",O_RDONLY,0,key);
	slseek(x,45,SEEK_SET);
	memset(buf, 0, 38);
	res=sread(x, buf, 38);	
	printf("buf = %s\t res=%d\n", buf, res);
    close(x->fd);	
		
    return 0;
}
