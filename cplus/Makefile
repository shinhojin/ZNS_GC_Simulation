CC=g++
CFLAGS=-g -Wall
OBJS=main.o controller.o
TARGET=example

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

main.o : controller.h main.cc
controller.o: controller.h controller.cc

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)