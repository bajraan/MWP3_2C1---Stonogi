
__start__: a.exe
	./a.exe

a.exe: main.o printraport.o
	g++ main.o printraport.o 

main.o: main.cpp printraport/printraport.h
	g++ -c -Wall -pedantic main.cpp

printraport.o: printraport/printraport.cpp printraport/printraport.h
	g++ -c -Wall -pedantic ./printraport/printraport.cpp
