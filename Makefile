CC=g++
EXE=bitcounting
FLAGS=-std=c++11 -Wall -O3

default: bitcounting.cpp
	$(CC) -o $(EXE) bitcounting.cpp $(FLAGS) 

clean:
	rm $(EXE)
