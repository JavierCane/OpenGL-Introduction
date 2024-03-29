CC = g++
LIBRARIES = -lGL -lglut -lGLU
INCLUDES = ./Model/model.h ./Model/model.cpp
OBJ = main.o utils.o cameraControl.o lightControl.o
EXEC = main

all:  $(EXEC)

main: $(OBJ)
	g++ -o $(EXEC) $(OBJ) $(INCLUDES) $(LIBRARIES)

main.o: main.cpp
	g++ -c main.cpp $(INCLUDES) $(LIBRARIES)

utils.o: utils.cpp
	g++ -c utils.cpp $(INCLUDES) $(LIBRARIES)

cameraControl.o: cameraControl.cpp
	g++ -c cameraControl.cpp $(INCLUDES) $(LIBRARIES)

lightControl.o: lightControl.cpp
	g++ -c lightControl.cpp $(INCLUDES) $(LIBRARIES)

destroy:
	rm -f *.o $(EXEC)

clean:
	rm -f *.o

deall: destroy $(EXEC)

xdeall: destroy $(EXEC)
	./$(EXEC)

xall: all
	./$(EXEC)
