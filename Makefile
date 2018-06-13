CC=g++
EXE=bitcounting
FLAGS=-std=c++11 -Wall -O3

default: main.cpp 
	$(CC) -o $(EXE) main.cpp $(FLAGS) 

clean:
	rm $(EXE)
