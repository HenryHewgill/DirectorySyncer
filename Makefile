C_COMPILATION = cc -std=c11 -Wall -Werror

mysync : mysync.o directorymanager.o sorter.o datastructures.o patternprocessor.o
	$(C_COMPILATION) -o mysync mysync.o directorymanager.o sorter.o datastructures.o patternprocessor.o

mysync.o : mysync.c mysync.h directorymanager.c directorymanager.h datastructures.c datastructures.h
	$(C_COMPILATION) -c mysync.c

directorymanager.o : mysync.h directorymanager.c directorymanager.h sorter.c sorter.h datastructures.c datastructures.h
	$(C_COMPILATION) -c directorymanager.c

sorter.o : mysync.h sorter.c sorter.h datastructures.c datastructures.h
	$(C_COMPILATION) -c sorter.c

datastructures.o : mysync.h datastructures.c datastructures.h
	$(C_COMPILATION) -c datastructures.c

patternprocessor.o : mysync.h patternprocessor.c patternprocessor.h
	$(C_COMPILATION) -c patternprocessor.c