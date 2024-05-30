CC=g++
DEPS = socket.h storage.h
OBJ =  socket.o storage.o main.o
CXXFLAGS = -g

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tracitest: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm -f *.o tracitest
