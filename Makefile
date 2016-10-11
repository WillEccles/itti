SRCPATH=src
MAIN=$(SRCPATH)/main.cpp
EXECUTABLE=itti

all:
	g++ -O $(MAIN) -o $(EXECUTABLE) -I $(SRCPATH)
