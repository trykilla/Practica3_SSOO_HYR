DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

#g++ archivo.cpp -o archivo -pthread -std=c++11

CFLAGS := -I$(DIRHEA) -pthread -std=c++17
CC := g++

all : dirs manager  #pa, pb


dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: 
	$(CC) -o $(DIREXE)manager $(DIRSRC)manager.cpp $(CFLAGS)	

test:
	./$(DIREXE)manager

solution:
	./$(DIREXE)manager

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~

clear_file: 
	echo "" > Files/Results.txt

