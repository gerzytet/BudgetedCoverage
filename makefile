CC=g++
DEPS = common.hpp
OBJ = sensors.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

sensors: $(OBJ)
	$(CC) -o $@ $^
