#ifdef VERBOSE
#include <stdio.h>
#endif

#include "sFile.h"


int 
slseek(sFile *sf, off_t offset, int whence) 
{

    switch (whence) {

        case SEEK_SET:
            sf->offset=offset+sFileHeaderSize;
            break;
        
        case SEEK_CUR:
            sf->offset+=offset;
            break;

        default:
            return -1;
    }

    return sf->offset-sFileHeaderSize;

}
