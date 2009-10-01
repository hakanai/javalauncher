
GCC_OPTS=-std=c99
JNI_INCLUDE=-I/System/Library/Frameworks/JavaVM.framework/Versions/1.6/Home/include
OBJ=o
EXE=

#all: launcher$(EXE) launcherW$(EXE)
all: launcher$(EXE)


launcher$(EXE): launcher.$(OBJ) javadll.$(OBJ) config.$(OBJ)
	gcc $(GCC_OPTS) -o launcher$(EXE) launcher.$(OBJ) javadll.$(OBJ) config.$(OBJ)

launcherW$(EXE): launcherres.$(OBJ) launcher.$(OBJ) javadll.$(OBJ) config.$(OBJ)
	gcc $(GCC_OPTS) -mwindows -o launcherW$(EXE) launcherres.$(OBJ) launcher.$(OBJ) javadll.$(OBJ) config.$(OBJ)


launcherres.$(OBJ): launcher.rc
	windres launcher.rc launcherres.$(OBJ)


launcher.$(OBJ): launcher.c javadll.h config.h
	gcc $(GCC_OPTS) $(JNI_INCLUDE) -c -o launcher.$(OBJ) launcher.c

javadll.$(OBJ): javadll.c javadll.h
	gcc $(GCC_OPTS) $(JNI_INCLUDE) -c -o javadll.$(OBJ) javadll.c

config.$(OBJ): config.c config.h
	gcc $(GCC_OPTS) $(JNI_INCLUDE) -c -o config.$(OBJ) config.c


clean:
	rm -f config.$(OBJ) javadll.$(OBJ) launcher.$(OBJ) launcherres.$(OBJ)
	rm -f launcher$(EXE) launcherW$(EXE)

