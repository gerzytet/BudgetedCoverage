CC=g++
DEPS = common.hpp random_utils.hpp sensor_generators.hpp
OBJ = sensors.o random_utils.o sensor_generators.o common.o
CXXFLAGS = -g

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sensors: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm -f *.o sensors
