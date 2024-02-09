CC=g++
DEPS = common.hpp random_utils.hpp sensor_generators.hpp
OBJ = sensors.o random_utils.o sensor_generators.o common.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

sensors: $(OBJ)
	$(CC) -o $@ $^
