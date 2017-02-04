#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<time.h>

#define AESKeyLength 128
#define AESBlockLength 128
#define AESKeyLengthByte 16
#define AESBlockLengthByte 16

#define sizeCTR AESBlockLengthByte
#define sizeRLEN (sizeof(long long))
#define sFileStartRLEN 16
#define sizeUNUSED 8
#define sFileHeaderSize (sizeCTR+sizeRLEN+sizeUNUSED)

#define BlockSize AESBlockLengthByte

#include <openssl/aes.h>

typedef struct {
    int fd;
    off_t offset;
    long long rlen;
    char *ctr;
    AES_KEY key;
    AES_KEY dkey;
} sFile;


sFile *sopen(const char *path, int flags, mode_t mode, char *key);
int slseek(sFile *sf, off_t offset, int whence);
int  sread(sFile *sf, void *buf, size_t count);
int swrite(sFile *sf, const void *buf, size_t count);

int  readBlock(sFile *sf, char *buf, long long *blockNum);
int writeBlock(sFile *sf, char *buf, long long *blockNum);

