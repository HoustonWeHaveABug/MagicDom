MAGICDOM_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

magicdom.exe: magicdom.o
	gcc -o magicdom.exe magicdom.o

magicdom.o: magicdom.c magicdom.make
	gcc ${MAGICDOM_C_FLAGS} -o magicdom.o magicdom.c
