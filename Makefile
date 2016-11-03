SRCPATH=src
WRESTD=wrestd/Code
FILES=$(SRCPATH)/*.cpp $(WRESTD)/*.cpp
EXECUTABLE=ittimain
STD=-std=c++14

all:
	g++ -O $(FILES) -o $(EXECUTABLE) -I $(SRCPATH) -I $(WRESTD) $(STD)
