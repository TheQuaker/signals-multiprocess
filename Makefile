OBJS = main.o workerList.o handlers.o functions.o
SRC = main.c workerList.c handlers.c functions.c
HEAD = workerList.h handlers.h functions.h
OUT = snifer
compiler = gcc
flags = -g -c
executable = snifer

all: $(OBJS)
	$(compiler) -g $(OBJS) -o $(OUT)
main.o: main.c workerList.h
	$(compiler) $(flags) main.c
workerList.o: workerList.c workerList.h
	$(compiler) $(flags) workerList.c
handlers.o: handlers.c handlers.h
	$(compiler) $(flags) handlers.c
functions.o: functions.c functions.h
	$(compiler) $(flags) functions.c

clean:
	rm $(executable) $(OBJS)
count:
	wc $(SRC) $(HEAD)
