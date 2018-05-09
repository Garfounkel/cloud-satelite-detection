
#---------------------------------------------------------
#makefile permettant la compilation du programme principal
#---------------------------------------------------------
.KEEP_STATE:

#definition des variables
CC=gcc
OBJ=../OBJ
EXE=../EXE
CCFLAGS=-Wall -g `pkg-config --cflags gtk+-2.0`
LDFLAGS=`pkg-config --libs gtk+-2.0`

#compilation
OBJ_FILES=compute.o main.o
OBJ_FILES_AUTO=compute.o main_auto.o

all: imaProjet.exe imaProjet_auto.exe
	mv *.o ${OBJ}

.c.o:
	${CC} ${CCFLAGS} -o $@ -c $<

imaProjet.exe:${OBJ_FILES}
	${CC} -o ${EXE}/$@ ${OBJ_FILES} -lm ${LDFLAGS}

imaProjet_auto.exe:${OBJ_FILES_AUTO}
	${CC} -o ${EXE}/$@ ${OBJ_FILES_AUTO} -lm ${LDFLAGS}

clean:
	\rm -f ${OBJ}/*.o ../*/core ${EXE}/*.exe

install:
	cp *.h ${INC}

.SUFFIXES: .exe .o .c .h
