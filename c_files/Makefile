CC=gcc
CFLAGS=-g -Wall
OBJS=main.o controller.o zns_simulation.o workload_creator.o
LIBS = -lpthread
TARGET=example

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

main.o : controller.h zns_simulation.h workload_creator.h main.c 
controller.o: controller.h controller.c
zns_simulation.o : zns_simulation.h zns_simulation.c
workload_creator.o : workload_creator.h workload_creator.c

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)