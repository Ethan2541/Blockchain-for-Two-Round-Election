all: CryptographyTools/main ProtectedDeclarations/main Lists/main HashTables/main Blocks/main


CryptographyTools/cryptotools.o: CryptographyTools/cryptotools.h CryptographyTools/cryptotools.c
	gcc -g -o CryptographyTools/cryptotools.o -c CryptographyTools/cryptotools.c

CryptographyTools/main.o: CryptographyTools/cryptotools.h CryptographyTools/main.c
	gcc -g -o CryptographyTools/main.o -c CryptographyTools/main.c

CryptographyTools/main: CryptographyTools/cryptotools.o CryptographyTools/main.o
	gcc -o CryptographyTools/main CryptographyTools/cryptotools.o CryptographyTools/main.o


ProtectedDeclarations/prdecl.o: CryptographyTools/cryptotools.h ProtectedDeclarations/prdecl.h ProtectedDeclarations/prdecl.c
	gcc -g -o ProtectedDeclarations/prdecl.o -c ProtectedDeclarations/prdecl.c

ProtectedDeclarations/main.o: CryptographyTools/cryptotools.h ProtectedDeclarations/prdecl.h ProtectedDeclarations/main.c
	gcc -g -o ProtectedDeclarations/main.o -c ProtectedDeclarations/main.c

ProtectedDeclarations/main: ProtectedDeclarations/prdecl.o ProtectedDeclarations/main.o
	gcc -o ProtectedDeclarations/main CryptographyTools/cryptotools.o ProtectedDeclarations/prdecl.o ProtectedDeclarations/main.o

	
Lists/list.o: CryptographyTools/cryptotools.h ProtectedDeclarations/prdecl.h Lists/list.h Lists/list.c
	gcc -g -o Lists/list.o -c Lists/list.c
	
Lists/main.o: CryptographyTools/cryptotools.h ProtectedDeclarations/prdecl.h Lists/list.h Lists/main.c
	gcc -g -o Lists/main.o -c Lists/main.c
	
Lists/main: Lists/list.o Lists/main.o
	gcc -o Lists/main CryptographyTools/cryptotools.o ProtectedDeclarations/prdecl.o Lists/list.o Lists/main.o

	
HashTables/htable.o: CryptographyTools/cryptotools.h ProtectedDeclarations/prdecl.h Lists/list.h HashTables/htable.h HashTables/htable.c
	gcc -g -o HashTables/htable.o -c HashTables/htable.c
	
HashTables/main.o: CryptographyTools/cryptotools.h ProtectedDeclarations/prdecl.h Lists/list.h HashTables/htable.h HashTables/main.c
	gcc -g -o HashTables/main.o -c HashTables/main.c
	
HashTables/main: HashTables/htable.o HashTables/main.o
	gcc -o HashTables/main CryptographyTools/cryptotools.o ProtectedDeclarations/prdecl.o Lists/list.o HashTables/htable.o HashTables/main.o -lm

	
Blocks/block.o: CryptographyTools/cryptotools.h ProtectedDeclarations/prdecl.h Lists/list.h Blocks/block.h Blocks/block.c
	gcc -g -o Blocks/block.o -c Blocks/block.c
	
Blocks/main.o: CryptographyTools/cryptotools.h ProtectedDeclarations/prdecl.h Lists/list.h Blocks/block.h Blocks/main.c
	gcc -g -o Blocks/main.o -c Blocks/main.c
	
Blocks/main: Blocks/block.o Blocks/main.o
	gcc -o Blocks/main CryptographyTools/cryptotools.o ProtectedDeclarations/prdecl.o Lists/list.o Blocks/block.o Blocks/main.o -lm -lssl -lcrypto


clean:
	rm -f CryptographyTools/*.o ProtectedDeclarations/*.o Lists/*.o HashTables/*.o Blocks/*.o CryptographyTools/main ProtectedDeclarations/main Lists/main HashTables/main Blocks/main
