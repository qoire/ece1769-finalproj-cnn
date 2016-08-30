NN_CC = gcc
NN_CFLAGS = -Wall -O3
NN_DEPS = images.h layer-weights-bias.h layer1-bias.h layer1-weights.h layer3-bias.h layer3-weights.h layer5-bias.h layer5-weights.h layer7-bias.h layer7-weights.h
NN_OBJ = main.o
NN_TARGET = main.out

all: $(NN_TARGET)

%.o: %.c $(NN_DEPS)
	$(NN_CC) $(NN_CFLAGS) -c -o $@ $<

$(NN_TARGET): $(NN_OBJ)
	$(NN_CC) $(NN_CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -f $(NN_TARGET) $(NN_OBJ)
