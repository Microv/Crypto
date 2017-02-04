CFLAGS=-c -std=c99 -g -DVERBOSE
#CFLAGS=-c -std=c99 
LDFLAGS=-lcrypto -ldl -lm

OBJ=sopen.o swrite.o sread.o slseek.o diskOps.o 
EXE=main scat makeSecure

all: ${EXE}

main: ${OBJ} main.o
	gcc ${OBJ} main.o -o $@ ${LDFLAGS} 

scat: ${OBJ} scat.o
	gcc ${OBJ} scat.o -o $@ ${LDFLAGS} 

makeSecure: ${OBJ} makeSecure.o
	gcc ${OBJ} makeSecure.o -o $@ ${LDFLAGS} 

clean:  
	rm -f ${OBJ}  ${EXE} main.o scat.o makeSecure.o

test:  makeSecure scat
	rm -f secDst
	touch secDst
	./makeSecure words.txt secDst
	./scat secDst > words1.txt
	diff words.txt words1.txt
    


